#include <Arduino.h>
#include "fsm.h"
#include "config.h"
#include "motors.h"
#include "encoders.h"

static RobotState currentState = STATE_FORWARD;
static RobotState previousState = STATE_FORWARD;

static unsigned long gapStartTime = 0;
static unsigned long leftHitStartTime = 0;
static unsigned long rightHitStartTime = 0;

static bool hitConfirmed(bool hit, unsigned long &hitStartTime) {
  if (!hit) {
    hitStartTime = 0;
    return false;
  }

  if (hitStartTime == 0) {
    hitStartTime = millis();
    return false;
  }

  return millis() - hitStartTime >= SIDE_HIT_CONFIRM_MS;
}

static void resetSideHitConfirmTimers() {
  leftHitStartTime = 0;
  rightHitStartTime = 0;
}

void changeState(RobotState newState) {
  if (newState != currentState) {
    previousState = currentState;
    currentState = newState;
    resetSideHitConfirmTimers();

    if (newState == STATE_CONFIRM_GAP) {
      if (previousState == STATE_MOVE_LEFT) {
        selectSideEncoderForLeftMove();
      } else if (previousState == STATE_MOVE_RIGHT) {
        selectSideEncoderForRightMove();
      }

      gapStartTime = millis();
      resetSideEncoderCount();
    }

    if (ENABLE_DEBUG_PRINT) {
      Serial.print("State changed to: ");
      Serial.println(getStateName(currentState));
    }
  }
}

void initFSM() {
  currentState = STATE_FORWARD;
  previousState = STATE_FORWARD;
  gapStartTime = 0;
  resetSideHitConfirmTimers();
}

void updateDecision(const SensorData &s) {
  // Safety stop: front blocked and both side contacts active means there is no
  // clear direction to continue moving.
  if (s.frontBlocked && s.leftWallHit && s.rightWallHit) {
    changeState(STATE_STOPPING);
    return;
  }

  switch (currentState) {
    case STATE_FORWARD:
      if (s.frontBlocked) {
        if (!s.leftWallHit) {
          changeState(STATE_MOVE_LEFT);
        } 
        else {
          changeState(STATE_MOVE_RIGHT);
        }
      }
      break;

    case STATE_MOVE_LEFT:
      // Front clear has priority over side hit, so the robot enters the gap when detected.
      if (s.frontClear) {
        changeState(STATE_CONFIRM_GAP);
      } 
      // If the robot hits the left side while moving left, move back right.
      else if (hitConfirmed(s.leftWallHit, leftHitStartTime)) {
        changeState(STATE_MOVE_RIGHT);
      }
      break;

    case STATE_MOVE_RIGHT:
      // Front clear has priority over side hit, so the robot enters the gap when detected.
      if (s.frontClear) {
        changeState(STATE_CONFIRM_GAP);
      } 
      // If the robot hits the right side while moving right, move back left.
      else if (hitConfirmed(s.rightWallHit, rightHitStartTime)) {
        changeState(STATE_MOVE_LEFT);
      }
      break;

    case STATE_CONFIRM_GAP:
      // Keep side movement until the front stays clear and encoder distance says the body has entered the gap.
      if (!s.frontClear) {
        if (previousState == STATE_MOVE_LEFT) {
          changeState(STATE_MOVE_LEFT);
        } else {
          changeState(STATE_MOVE_RIGHT);
        }
      } 
      else if (millis() - gapStartTime >= GAP_CONFIRM_MS &&
               (gapEntryDistanceReached() || millis() - gapStartTime >= GAP_CONFIRM_TIMEOUT_MS)) {
        changeState(STATE_FORWARD);
      }
      break;

    case STATE_STOPPING:
      if (s.frontClear) {
        changeState(STATE_FORWARD);
      } else if (s.frontBlocked && !s.leftWallHit) {
        changeState(STATE_MOVE_LEFT);
      } else if (s.frontBlocked && !s.rightWallHit) {
        changeState(STATE_MOVE_RIGHT);
      }
      break;

  }
}

void executeAction() {
  switch (currentState) {
    case STATE_FORWARD:
      driveForward();
      break;

    case STATE_MOVE_LEFT:
      moveLeft();
      break;

    case STATE_MOVE_RIGHT:
      moveRight();
      break;

    case STATE_CONFIRM_GAP:
      if (previousState == STATE_MOVE_LEFT) {
        moveLeft();
      } else {
        moveRight();
      }
      break;

    case STATE_STOPPING:
      stopMotors();
      break;

  }
}

RobotState getCurrentState() {
  return currentState;
}

const char* getStateName(RobotState state) {
  switch (state) {
    case STATE_FORWARD:
      return "FORWARD";
    case STATE_MOVE_LEFT:
      return "MOVE_LEFT";
    case STATE_MOVE_RIGHT:
      return "MOVE_RIGHT";
    case STATE_CONFIRM_GAP:
      return "CONFIRM_GAP";
    case STATE_STOPPING:
      return "STOPPING";
    default:
      return "UNKNOWN";
  }
}
