// Bay 1 - Motor Control evidence sketch
//
// Upload this before the Bay 1 test.
// During the test, disconnect the laptop and run from the robot battery/power board.
//
// Evidence:
// 1. Each motor is controlled separately.
// 2. The robot performs a timed 180-degree turn.

// Arduino Motor Shield Rev3 forward/back pair.
const int SH_M1_DIR = 12;
const int SH_M1_PWM = 3;
const int SH_M1_BRAKE = 9;

const int SH_M2_DIR = 13;
const int SH_M2_PWM = 11;
const int SH_M2_BRAKE = 8;

// Student-built H-bridge side motors.
const int HB_M3_IN1 = 34;
const int HB_M3_IN2 = 36;
const int HB_M4_IN1 = 38;
const int HB_M4_IN2 = 40;

const int FORWARD_PWM = 80;

// Keep the standby short for Bay 1. Increase only if the robot needs more
// time after battery power is switched on.
const unsigned long STARTUP_WAIT_MS = 1000;
const unsigned long SINGLE_MOTOR_RUN_MS = 800;
const unsigned long STEP_PAUSE_MS = 400;

// Tune this on the floor until the robot turns close to 180 degrees.
const unsigned long TURN_180_MS = 1100;

void stopAllMotors() {
  analogWrite(SH_M1_PWM, 0);
  analogWrite(SH_M2_PWM, 0);
  digitalWrite(SH_M1_BRAKE, HIGH);
  digitalWrite(SH_M2_BRAKE, HIGH);

  digitalWrite(HB_M3_IN1, LOW);
  digitalWrite(HB_M3_IN2, LOW);
  digitalWrite(HB_M4_IN1, LOW);
  digitalWrite(HB_M4_IN2, LOW);
}

void runShieldMotor1Forward() {
  stopAllMotors();
  digitalWrite(SH_M1_BRAKE, LOW);
  digitalWrite(SH_M1_DIR, HIGH);
  analogWrite(SH_M1_PWM, FORWARD_PWM);
}

void runShieldMotor2Forward() {
  stopAllMotors();
  digitalWrite(SH_M2_BRAKE, LOW);
  digitalWrite(SH_M2_DIR, LOW);
  analogWrite(SH_M2_PWM, FORWARD_PWM);
}

void runSideMotor3Forward() {
  stopAllMotors();
  digitalWrite(HB_M3_IN1, HIGH);
  digitalWrite(HB_M3_IN2, LOW);
}

void runSideMotor4Forward() {
  stopAllMotors();
  digitalWrite(HB_M4_IN1, HIGH);
  digitalWrite(HB_M4_IN2, LOW);
}

void turn180Degrees() {
  stopAllMotors();

  // Timed spin using the Motor Shield pair in opposite directions.
  // If the robot turns the wrong way or not in place, swap one DIR value.
  digitalWrite(SH_M1_BRAKE, LOW);
  digitalWrite(SH_M2_BRAKE, LOW);

  digitalWrite(SH_M1_DIR, HIGH);
  digitalWrite(SH_M2_DIR, HIGH);

  analogWrite(SH_M1_PWM, FORWARD_PWM);
  analogWrite(SH_M2_PWM, FORWARD_PWM);
}

void runStep(void (*motorAction)(), unsigned long durationMs) {
  motorAction();
  delay(durationMs);
  stopAllMotors();
  delay(STEP_PAUSE_MS);
}

void setup() {
  pinMode(SH_M1_DIR, OUTPUT);
  pinMode(SH_M1_PWM, OUTPUT);
  pinMode(SH_M1_BRAKE, OUTPUT);

  pinMode(SH_M2_DIR, OUTPUT);
  pinMode(SH_M2_PWM, OUTPUT);
  pinMode(SH_M2_BRAKE, OUTPUT);

  pinMode(HB_M3_IN1, OUTPUT);
  pinMode(HB_M3_IN2, OUTPUT);
  pinMode(HB_M4_IN1, OUTPUT);
  pinMode(HB_M4_IN2, OUTPUT);

  stopAllMotors();
  delay(STARTUP_WAIT_MS);
}

void loop() {
  runStep(runShieldMotor1Forward, SINGLE_MOTOR_RUN_MS);
  runStep(runShieldMotor2Forward, SINGLE_MOTOR_RUN_MS);
  runStep(runSideMotor3Forward, SINGLE_MOTOR_RUN_MS);
  runStep(runSideMotor4Forward, SINGLE_MOTOR_RUN_MS);
  runStep(turn180Degrees, TURN_180_MS);

  stopAllMotors();
  delay(2000);
}
