// Bay 3 - Encoder evidence sketch
// IMPORTANT: Do not power the motors during this test.
// Move the robot by hand or rotate each wheel by hand.
//
// Mark 1 evidence:
// - All four encoders show changing counts.
// - Direction changes between POS and NEG when the wheel is turned the other way.
//
// Mark 2 evidence:
// - Press z to reset counts.
// - Move the robot by hand across the surface by 15 cm.
// - Show the average distance estimate is close to 150 mm.

const int ENCODER_COUNT = 4;

const int ENCODER_A_PINS[ENCODER_COUNT] = {2, 18, 19, 20};
const int ENCODER_B_PINS[ENCODER_COUNT] = {22, 24, 26, 28};

const char *ENCODER_NAMES[ENCODER_COUNT] = {
  "M1_FL",
  "M2_RL",
  "M3_FR",
  "M4_RR"
};

// Measured calibration: manually moving 15 cm gives about 147 encoder counts.
// Formula: distance_mm = abs(counts) * 150.0 / 147.0.
const float ENCODER_CALIBRATION_COUNTS = 147.0;
const float ENCODER_CALIBRATION_DISTANCE_MM = 150.0;
const float TARGET_DISTANCE_MM = 150.0;
const float TARGET_TOLERANCE_MM = 10.0;

volatile long encoderCounts[ENCODER_COUNT] = {0, 0, 0, 0};
long lastPrintedCounts[ENCODER_COUNT] = {0, 0, 0, 0};

unsigned long lastPrintAt = 0;
const unsigned long PRINT_INTERVAL_MS = 250;

void updateEncoder(int index) {
  const int encoderB = digitalRead(ENCODER_B_PINS[index]);

  if (encoderB == LOW) {
    encoderCounts[index]++;
  } else {
    encoderCounts[index]--;
  }
}

void updateEncoder0() {
  updateEncoder(0);
}

void updateEncoder1() {
  updateEncoder(1);
}

void updateEncoder2() {
  updateEncoder(2);
}

void updateEncoder3() {
  updateEncoder(3);
}

long readEncoderCount(int index) {
  noInterrupts();
  const long count = encoderCounts[index];
  interrupts();
  return count;
}

void resetEncoderCount(int index) {
  noInterrupts();
  encoderCounts[index] = 0;
  interrupts();
  lastPrintedCounts[index] = 0;
}

void resetAllEncoderCounts() {
  noInterrupts();
  for (int i = 0; i < ENCODER_COUNT; i++) {
    encoderCounts[i] = 0;
  }
  interrupts();

  for (int i = 0; i < ENCODER_COUNT; i++) {
    lastPrintedCounts[i] = 0;
  }
}

float countToDistanceMm(long count) {
  const long absCount = (count < 0) ? -count : count;
  return (absCount * ENCODER_CALIBRATION_DISTANCE_MM) / ENCODER_CALIBRATION_COUNTS;
}

const char *directionFromDelta(long delta) {
  if (delta > 0) {
    return "POS";
  }
  if (delta < 0) {
    return "NEG";
  }
  return "STILL";
}

void handleSerialCommands() {
  if (Serial.available() <= 0) {
    return;
  }

  const char command = Serial.read();

  if (command == 'z' || command == 'Z') {
    resetAllEncoderCounts();
    Serial.println("RESET all encoder counts. Now move the robot by hand for 15 cm.");
  } else if (command >= '1' && command <= '4') {
    const int index = command - '1';
    resetEncoderCount(index);
    Serial.print("RESET ");
    Serial.println(ENCODER_NAMES[index]);
  }
}

void printEncoderEvidence() {
  float distanceTotal = 0.0;

  for (int i = 0; i < ENCODER_COUNT; i++) {
    const long count = readEncoderCount(i);
    const long delta = count - lastPrintedCounts[i];
    lastPrintedCounts[i] = count;

    const float distanceMm = countToDistanceMm(count);
    distanceTotal += distanceMm;

    Serial.print(ENCODER_NAMES[i]);
    Serial.print(" count=");
    Serial.print(count);
    Serial.print(" dir=");
    Serial.print(directionFromDelta(delta));
    Serial.print(" dist_mm=");
    Serial.print(distanceMm, 1);

    if (i < ENCODER_COUNT - 1) {
      Serial.print(" | ");
    }
  }

  const float averageDistanceMm = distanceTotal / ENCODER_COUNT;
  const bool targetOk =
      averageDistanceMm >= (TARGET_DISTANCE_MM - TARGET_TOLERANCE_MM) &&
      averageDistanceMm <= (TARGET_DISTANCE_MM + TARGET_TOLERANCE_MM);

  Serial.print(" | avg_dist_mm=");
  Serial.print(averageDistanceMm, 1);
  Serial.print(" | target_15cm=");
  Serial.println(targetOk ? "YES" : "NO");
}

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < ENCODER_COUNT; i++) {
    pinMode(ENCODER_A_PINS[i], INPUT);
    pinMode(ENCODER_B_PINS[i], INPUT_PULLUP);
  }

  resetAllEncoderCounts();

  attachInterrupt(digitalPinToInterrupt(ENCODER_A_PINS[0]), updateEncoder0, RISING);
  attachInterrupt(digitalPinToInterrupt(ENCODER_A_PINS[1]), updateEncoder1, RISING);
  attachInterrupt(digitalPinToInterrupt(ENCODER_A_PINS[2]), updateEncoder2, RISING);
  attachInterrupt(digitalPinToInterrupt(ENCODER_A_PINS[3]), updateEncoder3, RISING);

  Serial.println("Bay 3 encoder test started.");
  Serial.println("Motors must NOT be powered. Move wheels/robot by hand only.");
  Serial.println("Commands: z reset all, 1/2/3/4 reset one encoder.");
  Serial.println("For 2 marks: press z, move robot 15 cm, show avg_dist_mm around 150.");
}

void loop() {
  handleSerialCommands();

  if (millis() - lastPrintAt >= PRINT_INTERVAL_MS) {
    lastPrintAt = millis();
    printEncoderEvidence();
  }
}
