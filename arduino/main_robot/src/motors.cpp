#include "motors.h"
#include "config.h"
#include "encoders.h"

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
  // Keep the forward pair fully unpowered when not driving forward.
  // On this build, using brake HIGH caused unwanted FB motor movement.
  digitalWrite(FORWARD_MOTOR_A_BRAKE_PIN, LOW);
  digitalWrite(FORWARD_MOTOR_B_BRAKE_PIN, LOW);
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
  analogWrite(A1_H_BRIDGE_IN1_PIN, 0);
  analogWrite(A1_H_BRIDGE_IN2_PIN, 0);
  analogWrite(A2_H_BRIDGE_IN1_PIN, 0);
  analogWrite(A2_H_BRIDGE_IN2_PIN, 0);
}

static int clampInt(int value, int minValue, int maxValue) {
  if (value < minValue) {
    return minValue;
  }
  if (value > maxValue) {
    return maxValue;
  }
  return value;
}

static int clampPwm(int pwm) {
  return clampInt(pwm, 0, 255);
}

static int forwardBalanceCorrection() {
  if (!USE_ENCODER_CLOSED_LOOP) {
    return 0;
  }

  const float m1DistanceMm = getEncoderDistanceMm(0);
  const float m2DistanceMm = getEncoderDistanceMm(1);
  const float errorMm = m1DistanceMm - m2DistanceMm;
  const int correction = (int)(errorMm * FORWARD_BALANCE_KP);
  return clampInt(
      correction,
      -FORWARD_BALANCE_MAX_CORRECTION,
      FORWARD_BALANCE_MAX_CORRECTION);
}

static void writeSideMotor3Left() {
  analogWrite(A1_H_BRIDGE_IN1_PIN, SIDE_PWM);
  analogWrite(A1_H_BRIDGE_IN2_PIN, 0);
}

static void writeSideMotor4Left() {
  analogWrite(A2_H_BRIDGE_IN1_PIN, SIDE_PWM);
  analogWrite(A2_H_BRIDGE_IN2_PIN, 0);
}

static void writeSideMotor3Right() {
  // Right correction is used when the left wall switch is hit.
  analogWrite(A1_H_BRIDGE_IN1_PIN, 0);
  analogWrite(A1_H_BRIDGE_IN2_PIN, SIDE_PWM);
}

static void writeSideMotor4Right() {
  analogWrite(A2_H_BRIDGE_IN1_PIN, 0);
  analogWrite(A2_H_BRIDGE_IN2_PIN, SIDE_PWM);
}

static void writeSidePinsContinuous(bool leftDirection) {
  if (leftDirection) {
    writeSideMotor3Left();
    writeSideMotor4Left();
  } else {
    writeSideMotor3Right();
    writeSideMotor4Right();
  }
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

  // If M1 has travelled farther than M2, reduce M1 PWM and increase M2 PWM.
  const int correction = forwardBalanceCorrection();
  analogWrite(B1_FORWARD_PWM_PIN, clampPwm(FORWARD_PWM - correction));
  analogWrite(B2_FORWARD_PWM_PIN, clampPwm(FORWARD_PWM + correction));
}

void moveLeft() {
  if (USE_DUMMY_MOTORS) {
    printDummyMotorCommand("LEFT");
    return;
  }

  stopForwardMotors();
  engageForwardBrakes();
  writeSidePinsContinuous(true);
}  

void moveRight() {
  if (USE_DUMMY_MOTORS) {
    printDummyMotorCommand("RIGHT");
    return;
  }

  stopForwardMotors();
  engageForwardBrakes();
  writeSidePinsContinuous(false);
}
