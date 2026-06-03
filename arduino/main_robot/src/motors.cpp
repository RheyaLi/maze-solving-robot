#include "motors.h"
#include "config.h"

// ----- Dummy motor output -----
// Enable USE_DUMMY_MOTORS in config.h to print motor commands instead of writing pins.
static void printDummyMotorCommand(const char* action) {
  Serial.print("Dummy motor: ");
  Serial.print(action);
  Serial.print(" forward_pwm=");
  Serial.print(FORWARD_PWM);
  Serial.println();
}

static void stopForwardMotors() {
  analogWrite(B1_FORWARD_PWM_PIN, 0);
  analogWrite(B2_FORWARD_PWM_PIN, 0);
}

static void engageForwardBrakes() {
  digitalWrite(FORWARD_MOTOR_A_BRAKE_PIN, HIGH);
  digitalWrite(FORWARD_MOTOR_B_BRAKE_PIN, HIGH);
}

static void releaseForwardBrakes() {
  digitalWrite(FORWARD_MOTOR_A_BRAKE_PIN, LOW);
  digitalWrite(FORWARD_MOTOR_B_BRAKE_PIN, LOW);
}

static void setForwardMotorDirection() {
  digitalWrite(FORWARD_MOTOR_A_DIRECTION_PIN, HIGH);
  digitalWrite(FORWARD_MOTOR_B_DIRECTION_PIN, LOW);
}

static void stopSideMotors() {
  digitalWrite(A1_H_BRIDGE_IN1_PIN, LOW);
  digitalWrite(A1_H_BRIDGE_IN2_PIN, LOW);
  digitalWrite(A2_H_BRIDGE_IN1_PIN, LOW);
  digitalWrite(A2_H_BRIDGE_IN2_PIN, LOW);
}

void initMotors() {
  pinMode(B1_FORWARD_PWM_PIN, OUTPUT);
  pinMode(B2_FORWARD_PWM_PIN, OUTPUT);
  pinMode(FORWARD_MOTOR_A_DIRECTION_PIN, OUTPUT);
  pinMode(FORWARD_MOTOR_B_DIRECTION_PIN, OUTPUT);
  pinMode(FORWARD_MOTOR_A_BRAKE_PIN, OUTPUT);
  pinMode(FORWARD_MOTOR_B_BRAKE_PIN, OUTPUT);

  pinMode(A1_H_BRIDGE_IN1_PIN, OUTPUT);
  pinMode(A1_H_BRIDGE_IN2_PIN, OUTPUT);
  pinMode(A2_H_BRIDGE_IN1_PIN, OUTPUT);
  pinMode(A2_H_BRIDGE_IN2_PIN, OUTPUT);

  // Set safe startup output values before the FSM starts issuing commands.
  stopForwardMotors();
  setForwardMotorDirection();
  engageForwardBrakes();
  stopSideMotors();
}

void stopMotors() {
  if (USE_DUMMY_MOTORS) {
    printDummyMotorCommand("STOP");
    return;
  }

  stopForwardMotors();
  engageForwardBrakes();
  stopSideMotors();
}

void driveForward() {
  if (USE_DUMMY_MOTORS) {
    printDummyMotorCommand("FORWARD");
    return;
  }

  // B1/B2 only drive forward. M3/M4 side motors are off during forward motion.
  stopSideMotors();
  setForwardMotorDirection();
  releaseForwardBrakes();
  analogWrite(B1_FORWARD_PWM_PIN, FORWARD_PWM);
  analogWrite(B2_FORWARD_PWM_PIN, FORWARD_PWM);
}

void moveLeft() {
  if (USE_DUMMY_MOTORS) {
    printDummyMotorCommand("LEFT");
    return;
  }

  // R3 logic: M3 and M4 both run forward for left movement.
  stopForwardMotors();
  engageForwardBrakes();
  digitalWrite(A1_H_BRIDGE_IN1_PIN, HIGH);
  digitalWrite(A1_H_BRIDGE_IN2_PIN, LOW);
  digitalWrite(A2_H_BRIDGE_IN1_PIN, HIGH);
  digitalWrite(A2_H_BRIDGE_IN2_PIN, LOW);
}  

void moveRight() {
  if (USE_DUMMY_MOTORS) {
    printDummyMotorCommand("RIGHT");
    return;
  }

  // R3 logic: M3 and M4 both run reverse for right movement.
  stopForwardMotors();
  engageForwardBrakes();
  digitalWrite(A1_H_BRIDGE_IN1_PIN, LOW);
  digitalWrite(A1_H_BRIDGE_IN2_PIN, HIGH);
  digitalWrite(A2_H_BRIDGE_IN1_PIN, LOW);
  digitalWrite(A2_H_BRIDGE_IN2_PIN, HIGH);
}
