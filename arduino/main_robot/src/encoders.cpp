#include "encoders.h"
#include "config.h"

static volatile long encoderCounts[ENCODER_COUNT] = {0, 0, 0, 0};
static int activeSideEncoderIndex = LEFT_MOVE_ENCODER_INDEX;
static long lastPrintedEncoderCounts[ENCODER_COUNT] = {0, 0, 0, 0};

static void updateEncoder(int encoderIndex) {
  const int encoderB = digitalRead(ENCODER_B_PINS[encoderIndex]);

  // Direction convention from the validated Role 4 encoder test:
  // when channel A rises, B LOW means positive travel.
  if (encoderB == LOW) {
    encoderCounts[encoderIndex]++;
  } else {
    encoderCounts[encoderIndex]--;
  }
}

static void updateEncoder0() {
  updateEncoder(0);
}

static void updateEncoder1() {
  updateEncoder(1);
}

static void updateEncoder2() {
  updateEncoder(2);
}

static void updateEncoder3() {
  updateEncoder(3);
}

void initEncoders() {
  for (int i = 0; i < ENCODER_COUNT; i++) {
    pinMode(ENCODER_A_PINS[i], INPUT);
    pinMode(ENCODER_B_PINS[i], INPUT_PULLUP);
  }

  resetAllEncoderCounts();

  if (!USE_DUMMY_ENCODERS) {
    attachInterrupt(digitalPinToInterrupt(ENCODER_A_PINS[0]), updateEncoder0, RISING);
    attachInterrupt(digitalPinToInterrupt(ENCODER_A_PINS[1]), updateEncoder1, RISING);
    attachInterrupt(digitalPinToInterrupt(ENCODER_A_PINS[2]), updateEncoder2, RISING);
    attachInterrupt(digitalPinToInterrupt(ENCODER_A_PINS[3]), updateEncoder3, RISING);
  }
}

void resetSideEncoderCount() {
  noInterrupts();
  encoderCounts[LEFT_MOVE_ENCODER_INDEX] = 0;
  encoderCounts[RIGHT_MOVE_ENCODER_INDEX] = 0;
  interrupts();
}

void resetAllEncoderCounts() {
  noInterrupts();
  for (int i = 0; i < ENCODER_COUNT; i++) {
    encoderCounts[i] = 0;
  }
  interrupts();
}

void selectSideEncoderForLeftMove() {
  activeSideEncoderIndex = LEFT_MOVE_ENCODER_INDEX;
}

void selectSideEncoderForRightMove() {
  activeSideEncoderIndex = RIGHT_MOVE_ENCODER_INDEX;
}

long getEncoderCount(int encoderIndex) {
  if (encoderIndex < 0 || encoderIndex >= ENCODER_COUNT) {
    return 0;
  }

  if (USE_DUMMY_ENCODERS) {
    noInterrupts();
    encoderCounts[encoderIndex] += DUMMY_ENCODER_COUNTS_PER_READ;
    const long count = encoderCounts[encoderIndex];
    interrupts();
    return count;
  }

  noInterrupts();
  const long count = encoderCounts[encoderIndex];
  interrupts();
  return count;
}

long peekEncoderCount(int encoderIndex) {
  if (encoderIndex < 0 || encoderIndex >= ENCODER_COUNT) {
    return 0;
  }

  noInterrupts();
  const long count = encoderCounts[encoderIndex];
  interrupts();
  return count;
}

long getSideEncoderCount() {
  return getEncoderCount(activeSideEncoderIndex);
}

long getSideConfirmEncoderCount() {
  const long m3Count = peekEncoderCount(LEFT_MOVE_ENCODER_INDEX);
  const long m4Count = peekEncoderCount(RIGHT_MOVE_ENCODER_INDEX);
  const long m3Abs = (m3Count < 0) ? -m3Count : m3Count;
  const long m4Abs = (m4Count < 0) ? -m4Count : m4Count;
  return (m3Abs < m4Abs) ? m3Abs : m4Abs;
}

static float encoderCountToDistanceMm(int encoderIndex, long count) {
  if (encoderIndex < 0 || encoderIndex >= ENCODER_COUNT) {
    return 0.0;
  }

  const long distanceCount = (count < 0) ? -count : count;
  return (distanceCount * ENCODER_CALIBRATION_DISTANCE_MM) / ENCODER_15CM_COUNTS[encoderIndex];
}

float getEncoderDistanceMm(int encoderIndex) {
  return encoderCountToDistanceMm(encoderIndex, peekEncoderCount(encoderIndex));
}

float getSideConfirmTravelMm() {
  const float m3Distance = getEncoderDistanceMm(LEFT_MOVE_ENCODER_INDEX);
  const float m4Distance = getEncoderDistanceMm(RIGHT_MOVE_ENCODER_INDEX);
  return (m3Distance + m4Distance) / 2.0;
}

float getForwardTravelMm() {
  const float m1Distance = getEncoderDistanceMm(0);
  const float m2Distance = getEncoderDistanceMm(1);
  return (m1Distance + m2Distance) / 2.0;
}

float getSideTravelMm() {
  const long count = getSideEncoderCount();
  return encoderCountToDistanceMm(activeSideEncoderIndex, count) + GAP_ENTRY_REDUNDANCY_MM;
}

bool gapEntryDistanceReached() {
  return getSideTravelMm() > (ROBOT_DIAGONAL_MM / 2.0);
}

static const char* directionFromDelta(long delta) {
  if (delta > 0) {
    return "POS";
  }
  if (delta < 0) {
    return "NEG";
  }
  return "STILL";
}

void printEncoders() {
  Serial.print("Encoders");

  for (int i = 0; i < ENCODER_COUNT; i++) {
    const long count = peekEncoderCount(i);
    const long delta = count - lastPrintedEncoderCounts[i];
    lastPrintedEncoderCounts[i] = count;

    Serial.print(" | M");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(count);
    Serial.print(" dir=");
    Serial.print(directionFromDelta(delta));
  }

  const long sideCount = peekEncoderCount(activeSideEncoderIndex);
  const float sideTravelMm =
      encoderCountToDistanceMm(activeSideEncoderIndex, sideCount) + GAP_ENTRY_REDUNDANCY_MM;

  Serial.print(" | activeSide=M");
  Serial.print(activeSideEncoderIndex + 1);
  Serial.print(" sideTravelMm=");
  Serial.print(sideTravelMm, 1);

  if (USE_DUMMY_ENCODERS) {
    Serial.print(" | dummyEncoder=ON");
  }

  Serial.println();
}
