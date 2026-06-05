#include "src/config.h"
#include "src/sensors.h"
#include "src/motors.h"
#include "src/encoders.h"
#include "src/fsm.h"

unsigned long lastDebugTime = 0;

void initStatusLeds() {
  pinMode(FORWARD_LED_PIN, OUTPUT);
  pinMode(MOVE_LEFT_LED_PIN, OUTPUT);
  pinMode(MOVE_RIGHT_LED_PIN, OUTPUT);
  pinMode(CONFIRM_GAP_LED_PIN, OUTPUT);
}

void updateStatusLeds() {
  const RobotState state = getCurrentState();

  if (state == STATE_STOPPING) {
    digitalWrite(FORWARD_LED_PIN, HIGH);
    digitalWrite(MOVE_LEFT_LED_PIN, HIGH);
    digitalWrite(MOVE_RIGHT_LED_PIN, HIGH);
    digitalWrite(CONFIRM_GAP_LED_PIN, HIGH);
    return;
  }

  digitalWrite(FORWARD_LED_PIN, state == STATE_FORWARD ? HIGH : LOW);
  digitalWrite(MOVE_LEFT_LED_PIN, state == STATE_MOVE_LEFT ? HIGH : LOW);
  digitalWrite(MOVE_RIGHT_LED_PIN, state == STATE_MOVE_RIGHT ? HIGH : LOW);
  digitalWrite(CONFIRM_GAP_LED_PIN, state == STATE_CONFIRM_GAP ? HIGH : LOW);
}

void setup() {
  Serial.begin(9600);

  initStatusLeds();
  updateStatusLeds();

  initSensors();
  initMotors();
  initEncoders();
  initFSM();

  if (ENABLE_DEBUG_PRINT) {
    Serial.println("Robot framework started.");
  }
}

void loop() {
  SensorData s = readSensors();

  updateDecision(s);
  executeAction();
  updateStatusLeds();

  if (ENABLE_DEBUG_PRINT && millis() - lastDebugTime > DEBUG_INTERVAL_MS) {
    Serial.print("STATE = ");
    Serial.println(getStateName(getCurrentState()));

    Serial.print("BlockedBooleans | frontBlocked=");
    Serial.print(s.frontBlocked);
    Serial.print(" | leftBlocked=");
    Serial.print(s.leftWallHit);
    Serial.print(" | rightBlocked=");
    Serial.println(s.rightWallHit);

    printSensors(s);
    printEncoders();

    Serial.print("ACTION = ");
    Serial.println(getStateName(getCurrentState()));

    lastDebugTime = millis();
  }
}
