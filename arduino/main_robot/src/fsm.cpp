#include <Arduino.h>
#include "fsm.h"
#include "config.h"
#include "motors.h"
#include "encoders.h"

static RobotState currentState = STATE_FORWARD;
static RobotState previousState = STATE_FORWARD;

static bool gapConfirmActive = false;

static bool frontOpenForGap(const SensorData &s) {
  return s.frontRaw < FRONT_WALL_THRESHOLD;
}

static bool frontBlockedForGap(const SensorData &s) {
  return s.frontRaw >= FRONT_WALL_THRESHOLD;
}

void changeState(RobotState newState) {
  if (newState != currentState) {
    previousState = currentState;
    currentState = newState;
    gapConfirmActive = false;
    resetAllEncoderCounts();

    if (ENABLE_DEBUG_PRINT) {
      Serial.print("State changed to: ");
      Serial.println(getStateName(currentState));
    }
  }
}

void initFSM() {
  currentState = STATE_FORWARD;
  previousState = STATE_FORWARD;
  gapConfirmActive = false;
}

static void startGapConfirm(RobotState moveState) {
  gapConfirmActive = true;

  if (moveState == STATE_MOVE_LEFT) {
    selectSideEncoderForLeftMove();
  } else {
    selectSideEncoderForRightMove();
  }

  resetSideEncoderCount();
}

static void updateGapConfirm(RobotState moveState, const SensorData &s) {
  if (!gapConfirmActive) {
    if (frontOpenForGap(s)) {
      startGapConfirm(moveState);
    }
    return;
  }

  if (frontBlockedForGap(s)) {
    gapConfirmActive = false;
    return;
  }

  if (getSideConfirmEncoderCount() >= GAP_CONFIRM_ENCODER_COUNTS) {
    changeState(STATE_PASS_GAP);
  }
}

void updateDecision(const SensorData &s) {
  // Side-wall correction is below frontClear priority but above STOPPING:
  // left wall hit -> move right; right wall hit -> move left.
  if (!s.frontClear && s.leftWallHit && !s.rightWallHit && currentState != STATE_MOVE_RIGHT) {
    changeState(STATE_MOVE_RIGHT);
    return;
  }
  if (!s.frontClear && s.rightWallHit && !s.leftWallHit && currentState != STATE_MOVE_LEFT) {
    changeState(STATE_MOVE_LEFT);
    return;
  }

  switch (currentState) {
    case STATE_FORWARD:
      if (s.frontBlocked) {
        if (s.leftWallHit && !s.rightWallHit) {
          changeState(STATE_MOVE_RIGHT);
        } else if (s.rightWallHit && !s.leftWallHit) {
          changeState(STATE_MOVE_LEFT);
        } else if (!s.leftWallHit) {
          changeState(STATE_MOVE_LEFT);
        } else if (!s.rightWallHit) {
          changeState(STATE_MOVE_RIGHT);
        } else {
          changeState(STATE_STOPPING);
        }
      }
      break;

    case STATE_MOVE_LEFT:
      // If the robot hits the left side while moving left, move back right.
      if (s.leftWallHit) {
        changeState(STATE_MOVE_RIGHT);
      }
      else {
        updateGapConfirm(STATE_MOVE_LEFT, s);
      }
      break;

    case STATE_MOVE_RIGHT:
      // If the robot hits the right side while moving right, move back left.
      if (s.rightWallHit) {
        changeState(STATE_MOVE_LEFT);
      }
      else {
        updateGapConfirm(STATE_MOVE_RIGHT, s);
      }
      break;

    case STATE_PASS_GAP:
      if (getForwardTravelMm() >= PASS_GAP_FORWARD_MM) {
        changeState(STATE_FORWARD);
      }
      break;

    case STATE_STOPPING:
      if (s.frontClear) {
        changeState(STATE_FORWARD);
      } else if (s.leftWallHit && !s.rightWallHit) {
        changeState(STATE_MOVE_RIGHT);
      } else if (s.rightWallHit && !s.leftWallHit) {
        changeState(STATE_MOVE_LEFT);
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

    case STATE_PASS_GAP:
      driveForward();
      break;

    case STATE_STOPPING:
      stopMotors();
      break;

  }
}

RobotState getCurrentState() {
  return currentState;
}

bool isGapConfirming() {
  return gapConfirmActive;
}

const char* getStateName(RobotState state) {
  switch (state) {
    case STATE_FORWARD:
      return "FORWARD";
    case STATE_MOVE_LEFT:
      return "MOVE_LEFT";
    case STATE_MOVE_RIGHT:
      return "MOVE_RIGHT";
    case STATE_PASS_GAP:
      return "PASS_GAP";
    case STATE_STOPPING:
      return "STOPPING";
    default:
      return "UNKNOWN";
  }
}
