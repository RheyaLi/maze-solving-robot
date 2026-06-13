#ifndef CONFIG_H
#define CONFIG_H
#include <Arduino.h>

// ----- Dummy subsystem test -----
// Keep these at the top so the framework demo can be enabled quickly.
// Set them to false only when the matching real subsystem is integrated.
const bool USE_DUMMY_MOTORS = false;
const bool USE_DUMMY_SENSORS = false;
const bool USE_DUMMY_ENCODERS = false;

// Dummy encoder count added each time the FSM checks gap-entry distance.
const long DUMMY_ENCODER_COUNTS_PER_READ = 2;

// =============================
// Integration TODOs to verify on the assembled robot
// - DONE; Confirm main-robot switch wiring on D7/D4. Role 2 tested D2/D3, but those
//   pins conflict with M1 encoder A and forward PWM in the integrated robot.
// - Re-check front/rear photodiode thresholds on A4/A5 under maze lighting.
//   Front clear has been observed around raw ~= 32-35; 5 cm is around raw ~= 205-212.
//   Current hysteresis triggers early: raw <= 45 is clear and raw >= 60 is blocked.
// - Confirm front sensor polarity on the assembled robot: current logic treats
//   raw > FRONT_WALL_THRESHOLD as wall detected.
// - Confirm side switch polarity and pull-up wiring. Current config uses
//   HIGH = hit and external pull-up/pull-down wiring, not internal pullups.
// - Confirm Motor Shield Rev3 forward direction: current convention is
//   A direction HIGH and B direction LOW.
// - Confirm Motor Shield brake behavior: D9/D8 HIGH should stop/hold B1/B2.
// - Confirm M3/M4 self-made H-bridge direction:
//   side movement is inverted from the original R3 motor logic.
// - Confirm M3 encoder remains reliable for both left and right gap-confirm
//   moves. Distance uses abs(count), so sign reversal is acceptable.
// - Encoder/mechanical team confirmed M1/M2 are front/back movement wheels and
//   M3/M4 are left/right movement wheels, so gap confirm currently uses M3.
// - Re-measure ENCODER_15CM_COUNTS if the wheel, encoder trigger mode, or
//   motor wiring changes.
// - Tune GAP_CONFIRM_ENCODER_COUNTS and GAP_ENTRY_REDUNDANCY_MM after full-car maze tests.
// =============================

// =============================
// Reserved pins
// Arduino Mega 2560 is the only integration target.
// D3 and D11 are reserved for forward motor PWM.
// D8 and D9 are reserved for Arduino Motor Shield Rev3 brake control.
// D12 and D13 are reserved for Arduino Motor Shield Rev3 direction control.
// D34/D36 and D38/D40 are reserved for side-motor H-bridge control.
// D2/D18/D19/D20 are reserved for encoder channel A interrupts.
// D14-D16 and D30 are reserved for state indicator LEDs.
// Use A4-A5 for the validated Role 2 photodiode sensors.
// =============================

// ----- Project pins -----
const int FORWARD_LED_PIN = 30;
const int MOVE_LEFT_LED_PIN = 14;
const int MOVE_RIGHT_LED_PIN = 15;
const int CONFIRM_GAP_LED_PIN = 16;

const int LEFT_SIDE_SWITCH_PIN  = 7;
const int RIGHT_SIDE_SWITCH_PIN = 4;

// ----- Side motor H-bridge pins -----
// M3 and M4 are side motors controlled by two self-made H-bridges.
// M3/M4 side direction is inverted from the original R3 motor logic.
const int A1_H_BRIDGE_IN1_PIN = 44;
const int A1_H_BRIDGE_IN2_PIN = 45;
const int A2_H_BRIDGE_IN1_PIN = 46;
const int A2_H_BRIDGE_IN2_PIN = 10;

// Role 2 validated sensor wiring used D2/D3 for switches, but those conflict
// with M1 encoder A (D2) and forward PWM (D3). Main integration keeps switches
// on D7/D4 and uses the validated photodiode analog pins A4/A5.
const int FRONT_PHOTODIODE_PIN    = A4;
const int REAR_PHOTODIODE_PIN     = A5;

// ----- Encoder pins -----
// Channel A pins use RISING interrupts. When A rises, the ISR reads channel B to
// decide direction. Encoder readings are used only for distance/gap checks,
// not for motor speed control.
const int ENCODER_COUNT = 4;
// 0 = M1 Front Left, 1 = M2 Rear Left, 2 = M3 Front Right, 3 = M4 Rear Right.
const int ENCODER_A_PINS[ENCODER_COUNT] = {2, 18, 19, 20};
const int ENCODER_B_PINS[ENCODER_COUNT] = {22, 24, 26, 28};

// Use M3/M4 side encoders for gap-confirm moves.
// Distance checks use abs(count), so the sign can differ by movement direction.
const int LEFT_MOVE_ENCODER_INDEX = 2;
const int RIGHT_MOVE_ENCODER_INDEX = 3;

// ----- Forward motor PWM pins -----
// B1 and B2 only drive the robot forward. They are not connected through H-bridges.
const int B1_FORWARD_PWM_PIN = 3;
const int B2_FORWARD_PWM_PIN = 11;

// ----- Arduino Motor Shield Rev3 direction pins -----
// Role 4 validated forward convention: A HIGH, B LOW.
const int FORWARD_MOTOR_A_DIRECTION_PIN = 12;
const int FORWARD_MOTOR_B_DIRECTION_PIN = 13;

// ----- Arduino Motor Shield Rev3 brake pins -----
// HIGH = brake engaged, LOW = brake released.
const int FORWARD_MOTOR_A_BRAKE_PIN = 9;
const int FORWARD_MOTOR_B_BRAKE_PIN = 8;

// ----- Tunable parameters -----
const bool SIDE_SWITCH_HIT_IS_HIGH = true;
const bool SIDE_SWITCHES_USE_INTERNAL_PULLUPS = false;
const unsigned long SWITCH_DEBOUNCE_MS = 50;
const unsigned long SIDE_HIT_CONFIRM_MS = 250;

const int FRONT_WALL_THRESHOLD    = 105;
const int FRONT_CLEAR_THRESHOLD   = 65;
const int REAR_WALL_THRESHOLD     = 90;

const int FORWARD_PWM = 70;
const int SIDE_PWM = 50;

// ----- Encoder distance calibration -----
// Measured calibration: manually moving 15 cm gives different counts on each
// encoder. Convert each encoder to distance before comparing motors.
// Formula: distanceMm = abs(counts) * ENCODER_CALIBRATION_DISTANCE_MM /
//                       ENCODER_15CM_COUNTS[encoderIndex].
// Enter gap when sideTravel > robotDiagonal / 2.
const float ENCODER_CALIBRATION_DISTANCE_MM = 150.0;
const float ENCODER_15CM_COUNTS[ENCODER_COUNT] = {237.0, 131.0, 233.0, 108.0};
const float GAP_ENTRY_REDUNDANCY_MM = 20.0;
const float ROBOT_DIAGONAL_MM = 260.0;  // Measured robot diagonal/side-entry basis: 26 cm.

// ----- Encoder closed-loop motor balance -----
// Closed-loop control compares distance in mm, not raw counts, because each
// encoder has a different counts-per-distance calibration.
const bool USE_ENCODER_CLOSED_LOOP = true;
const float FORWARD_BALANCE_KP = 1.0;
const int FORWARD_BALANCE_MAX_CORRECTION = 20;

// Gap confirm
// When front IR first sees clear while moving sideways, keep moving sideways
// until M3/M4 side encoders have produced this many raw ticks. If front IR
// becomes blocked before this count is reached, cancel the gap. After confirm,
// drive forward by PASS_GAP_FORWARD_MM using the M1/M2 encoder distance formula.
const long GAP_CONFIRM_ENCODER_COUNTS = 30;
const float PASS_GAP_FORWARD_MM = 180.0;
const unsigned long DEBUG_INTERVAL_MS  = 200;

// Debug
// Change this value to false when Serial Monitor output is not needed.
const bool ENABLE_DEBUG_PRINT = true;

#endif
