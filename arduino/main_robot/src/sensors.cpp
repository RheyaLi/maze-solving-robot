#include "sensors.h"
#include "config.h"

// ----- Dummy sensor values -----
// Change these values when USE_DUMMY_SENSORS is true.
const unsigned long DUMMY_PHASE_MS   = 2000;

const int DUMMY_FRONT_CLEAR_RAW = 200;
const int DUMMY_FRONT_BLOCKED_RAW = 700;
const int DUMMY_REAR_RAW = 200;

const bool DUMMY_SIDE_CLEAR = false;
const bool DUMMY_SIDE_HIT = true;

static int leftStableState = LOW;
static int leftLastReading = LOW;
static unsigned long leftLastDebounceTime = 0;

static int rightStableState = LOW;
static int rightLastReading = LOW;
static unsigned long rightLastDebounceTime = 0;

static bool frontBlockedState = false;

static int readDebouncedSwitch(int pin, int &stableState, int &lastReading, unsigned long &lastDebounceTime) {
  const int reading = digitalRead(pin);

  if (reading != lastReading) {
    lastDebounceTime = millis();
  }

  if (millis() - lastDebounceTime > SWITCH_DEBOUNCE_MS) {
    stableState = reading;
  }

  lastReading = reading;
  return stableState;
}

static bool switchStateIsHit(int stableState) {
  if (SIDE_SWITCH_HIT_IS_HIGH) {
    return stableState == HIGH;
  }

  return stableState == LOW;
}

static bool updateFrontBlockedState(int frontRaw) {
  if (frontRaw >= FRONT_WALL_THRESHOLD) {
    frontBlockedState = true;
  } else if (frontRaw <= FRONT_CLEAR_THRESHOLD) {
    frontBlockedState = false;
  }

  return frontBlockedState;
}

static SensorData readDummySensors() {
  SensorData s;

  // Change the phase duration or the cases below to test different dummy scenarios.
  const unsigned long phase = (millis() / DUMMY_PHASE_MS) % 4;

  // Phase 0 default: front clear, no side wall hit.
  s.frontRaw = DUMMY_FRONT_CLEAR_RAW;
  s.rearRaw = DUMMY_REAR_RAW;
  s.frontBlocked = false;
  s.frontClear = true;
  s.rearWallDetected = false;
  s.leftSideRaw = DUMMY_SIDE_CLEAR ? HIGH : LOW;
  s.rightSideRaw = DUMMY_SIDE_CLEAR ? HIGH : LOW;
  s.leftWallHit = DUMMY_SIDE_CLEAR;
  s.rightWallHit = DUMMY_SIDE_CLEAR;

  if (phase == 1) {
    // Phase 1: front blocked, left side clear -> FSM should move left.
    s.frontRaw = DUMMY_FRONT_BLOCKED_RAW;
    s.frontBlocked = true;
    s.frontClear = false;
  } else if (phase == 2) {
    // Phase 2: front blocked, left side hit -> FSM should move right.
    s.frontRaw = DUMMY_FRONT_BLOCKED_RAW;
    s.frontBlocked = true;
    s.frontClear = false;
    s.leftSideRaw = DUMMY_SIDE_HIT ? HIGH : LOW;
    s.leftWallHit = DUMMY_SIDE_HIT;
  } else if (phase == 3) {
    // Phase 3: front clear again -> FSM should confirm gap, then move forward.
    s.frontRaw = DUMMY_FRONT_CLEAR_RAW;
    s.frontBlocked = false;
    s.frontClear = true;
  }

  return s;
}

void initSensors() {
  const int switchInputMode = SIDE_SWITCHES_USE_INTERNAL_PULLUPS ? INPUT_PULLUP : INPUT;
  pinMode(LEFT_SIDE_SWITCH_PIN, switchInputMode);
  pinMode(RIGHT_SIDE_SWITCH_PIN, switchInputMode);

  pinMode(FRONT_PHOTODIODE_PIN, INPUT);
  pinMode(REAR_PHOTODIODE_PIN, INPUT);
}

SensorData readSensors() {
  if (USE_DUMMY_SENSORS) {
    return readDummySensors();
  }

  SensorData s;

  s.frontRaw = analogRead(FRONT_PHOTODIODE_PIN);
  s.rearRaw = analogRead(REAR_PHOTODIODE_PIN);

  s.frontBlocked = updateFrontBlockedState(s.frontRaw);
  s.frontClear   = !s.frontBlocked;
  s.rearWallDetected = s.rearRaw > REAR_WALL_THRESHOLD;

  const int leftState = readDebouncedSwitch(
      LEFT_SIDE_SWITCH_PIN,
      leftStableState,
      leftLastReading,
      leftLastDebounceTime);

  const int rightState = readDebouncedSwitch(
      RIGHT_SIDE_SWITCH_PIN,
      rightStableState,
      rightLastReading,
      rightLastDebounceTime);

  s.leftWallHit = switchStateIsHit(leftState);
  s.rightWallHit = switchStateIsHit(rightState);
  s.leftSideRaw = leftState;
  s.rightSideRaw = rightState;

  return s;
}

void printSensors(const SensorData &s) {
  const float frontVoltage = s.frontRaw * (5.0 / 1023.0);

  Serial.print("Front: ");
  Serial.print(s.frontRaw);

  Serial.print(" | FrontVoltage: ");
  Serial.print(frontVoltage);
  Serial.print(" V");

  Serial.print(" | Rear: ");
  Serial.print(s.rearRaw);

  Serial.print(" | RearWall: ");
  Serial.print(s.rearWallDetected);

  Serial.print(" | FrontBlocked: ");
  Serial.print(s.frontBlocked);

  Serial.print(" | FrontClear: ");
  Serial.print(s.frontClear);

  Serial.print(" | left_side_return raw=");
  Serial.print(s.leftSideRaw);
  Serial.print(" hit=");
  Serial.print(s.leftWallHit ? "YES" : "NO");

  Serial.print(" | right_side_return raw=");
  Serial.print(s.rightSideRaw);
  Serial.print(" hit=");
  Serial.println(s.rightWallHit ? "YES" : "NO");
}
