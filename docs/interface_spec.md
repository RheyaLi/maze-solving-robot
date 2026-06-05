# Main Interfaces

## 1. Pin and Signal Interface

| Subsystem | Item | Direction | Type | Pin / Value | Code Name / Function | Current Convention |
|---|---|---|---|---|---|---|
| Motor | B1 forward PWM | Output | PWM | D3 | `B1_FORWARD_PWM_PIN` | Fixed `FORWARD_PWM = 32`; no H-bridge |
| Motor | B2 forward PWM | Output | PWM | D11 | `B2_FORWARD_PWM_PIN` | Fixed `FORWARD_PWM = 32`; no H-bridge |
| Motor Shield | Forward motor A direction | Output | Digital | D12 | `FORWARD_MOTOR_A_DIRECTION_PIN` | HIGH for Role 4 forward convention |
| Motor Shield | Forward motor B direction | Output | Digital | D13 | `FORWARD_MOTOR_B_DIRECTION_PIN` | LOW for Role 4 forward convention |
| Motor Shield | Forward motor A brake | Output | Digital | D9 | `FORWARD_MOTOR_A_BRAKE_PIN` | HIGH = brake engaged, LOW = released |
| Motor Shield | Forward motor B brake | Output | Digital | D8 | `FORWARD_MOTOR_B_BRAKE_PIN` | HIGH = brake engaged, LOW = released |
| H-bridge | M3 side motor inputs | Output | Digital | D34/D36 | `A1_H_BRIDGE_IN1_PIN`, `A1_H_BRIDGE_IN2_PIN` | Inverted side logic: reverse for left, forward for right |
| H-bridge | M4 side motor inputs | Output | Digital | D38/D40 | `A2_H_BRIDGE_IN1_PIN`, `A2_H_BRIDGE_IN2_PIN` | Inverted side logic: reverse for left, forward for right |
| LED | Forward status LED | Output | Digital | D30 | `FORWARD_LED_PIN` | HIGH during `STATE_FORWARD`, otherwise LOW |
| LED | Move-left status LED | Output | Digital | D14 | `MOVE_LEFT_LED_PIN` | HIGH during `STATE_MOVE_LEFT`, otherwise LOW |
| LED | Move-right status LED | Output | Digital | D15 | `MOVE_RIGHT_LED_PIN` | HIGH during `STATE_MOVE_RIGHT`, otherwise LOW |
| LED | Confirm-gap status LED | Output | Digital | D16 | `CONFIRM_GAP_LED_PIN` | HIGH during `STATE_CONFIRM_GAP`, otherwise LOW |
| Sensor | Left side switch | Input | Digital | D7 | `LEFT_SIDE_SWITCH_PIN` | Debounced 50 ms; hit polarity from `SIDE_SWITCH_HIT_IS_HIGH` |
| Sensor | Right side switch | Input | Digital | D4 | `RIGHT_SIDE_SWITCH_PIN` | Debounced 50 ms; hit polarity from `SIDE_SWITCH_HIT_IS_HIGH` |
| Sensor | Front photodiode | Input | Analog | A4 | `FRONT_PHOTODIODE_PIN` | Role 2 calibrated logic: wall when raw > 200 |
| Sensor | Rear photodiode | Input | Analog | A5 | `REAR_PHOTODIODE_PIN` | Role 2 validated; wall when raw > 100, debug/status only |
| Encoder | Encoder 1 / M1 Front Left | Input | A RISING interrupt / B read | D2 / D22 | `ENCODER_A_PINS[0]`, `ENCODER_B_PINS[0]` | A rising ISR reads B; B LOW increments count |
| Encoder | Encoder 2 / M2 Rear Left | Input | A RISING interrupt / B read | D18 / D24 | `ENCODER_A_PINS[1]`, `ENCODER_B_PINS[1]` | A rising ISR reads B; B LOW increments count |
| Encoder | Encoder 3 / M3 Front Right | Input | A RISING interrupt / B read | D19 / D26 | `ENCODER_A_PINS[2]`, `ENCODER_B_PINS[2]` | A rising ISR reads B; B LOW increments count |
| Encoder | Encoder 4 / M4 Rear Right | Input | A RISING interrupt / B read | D20 / D28 | `ENCODER_A_PINS[3]`, `ENCODER_B_PINS[3]` | A rising ISR reads B; B LOW increments count |

D2 is reserved for Encoder 1 channel A and D3 is reserved for forward PWM.
The Role 2 standalone sensor test used D2/D3 for switches, but main
integration keeps the side switches on D7/D4 to avoid those conflicts.

## 2. Motor Interface

```cpp
void stopMotors();
void driveForward();

void moveLeft();
void moveRight();
```

| Function | Input | Output / Effect | Notes |
|---|---|---|---|
| `stopMotors()` | none | B1/B2 PWM pins write 0; A/B brakes HIGH; M3/M4 H-bridge inputs LOW | Safety stop / `STATE_STOPPING` |
| `driveForward()` | none | Direction A HIGH, direction B LOW; A/B brakes LOW; B1/B2 PWM pins write `FORWARD_PWM = 32`; M3/M4 side motors are off | Forward movement only |
| `moveLeft()` | none | B1/B2 off; A/B brakes HIGH; M3/M4 reverse: IN1 LOW, IN2 HIGH | Left/side movement |
| `moveRight()` | none | B1/B2 off; A/B brakes HIGH; M3/M4 forward: IN1 HIGH, IN2 LOW | Right/side movement |

No public backward command is currently part of the framework.

## 3. Sensor Interface

```cpp
SensorData readSensors();
void printSensors(const SensorData &s);
```

| Data Field | Type | Source | Meaning |
|---|---|---|---|
| `frontRaw` | `int` | A4 when real / dummy sequence when enabled | Raw front photodiode value |
| `rearRaw` | `int` | A5 when real / dummy sequence when enabled | Raw rear photodiode value |
| `frontBlocked` | `bool` | `frontRaw > FRONT_WALL_THRESHOLD` | Front wall detected using the Role 2 calibrated single-threshold logic |
| `frontClear` | `bool` | `!frontBlocked` | Gap/front clear detected |
| `rearWallDetected` | `bool` | `rearRaw > REAR_WALL_THRESHOLD` | Rear wall/debug status |
| `leftWallHit` | `bool` | D7 debounced when real / dummy sequence when enabled | Left side switch hit |
| `rightWallHit` | `bool` | D4 debounced when real / dummy sequence when enabled | Right side switch hit |

## 4. FSM Interface

```cpp
void updateDecision(const SensorData &s);
void executeAction();
RobotState getCurrentState();
```

| Function | Input | Output / Effect |
|---|---|---|
| `updateDecision(const SensorData &s)` | Latest `SensorData` | Updates `RobotState` |
| `executeAction()` | Current `RobotState` | Calls motor interface function |
| `getCurrentState()` | None | Returns current `RobotState` |
| `getStateName(RobotState state)` | State enum | Returns debug string |

## 5. Encoder Interface

```cpp
void initEncoders();
void resetSideEncoderCount();
void resetAllEncoderCounts();
long getEncoderCount(int encoderIndex);
long peekEncoderCount(int encoderIndex);
long getSideEncoderCount();
float getSideTravelMm();
bool gapEntryDistanceReached();
void printEncoders();
```

| Function | Input | Output / Effect |
|---|---|---|
| `initEncoders()` | None | Initializes four encoder subsystems, sets channel B pins to `INPUT_PULLUP`, and attaches channel A `RISING` interrupts |
| `resetSideEncoderCount()` | None | Resets side movement count when gap confirmation starts |
| `resetAllEncoderCounts()` | None | Resets all four encoder counts |
| `selectSideEncoderForLeftMove()` | None | Selects M3 Front Right for left-side gap confirmation |
| `selectSideEncoderForRightMove()` | None | Selects M3 Front Right for right-side gap confirmation |
| `getEncoderCount(int encoderIndex)` | 0-3 | Returns one encoder count |
| `peekEncoderCount(int encoderIndex)` | 0-3 | Returns one encoder count without changing dummy counts |
| `getSideEncoderCount()` | Active side encoder, or dummy mode | Returns only the selected encoder for the current left/right gap-confirm move; dummy mode increments by `DUMMY_ENCODER_COUNTS_PER_READ` |
| `getSideTravelMm()` | Encoder count constants | Returns `encoderTurns * wheelCircumference + redundancy` |
| `gapEntryDistanceReached()` | None | True when `sideTravel > robotDiagonal / 2` |
| `printEncoders()` | None | Prints M1-M4 counts, direction since last debug print, active side encoder, and side travel |

Gap-entry constants are currently:

| Constant | Current Value | Meaning |
|---|---:|---|
| `ENCODER_COUNTS_PER_REV` | `187.0` | Used for side-travel distance checks |
| `WHEEL_CIRCUMFERENCE_MM` | `345.6` | Wheel circumference from 110 mm wheel diameter |
| `GAP_ENTRY_REDUNDANCY_MM` | `10.0` | Extra safety distance |
| `ROBOT_DIAGONAL_MM` | `283.0` | Approx. 20 cm x 20 cm robot diagonal |

Both `LEFT_MOVE_ENCODER_INDEX` and `RIGHT_MOVE_ENCODER_INDEX` currently use
M3 Front Right (`2`) because the encoder team confirmed M3 is used for
left/right movement. The encoder/mechanical team also confirmed M1/M2 are
front/back movement wheels and M3/M4 are left/right movement wheels.
`getSideTravelMm()` uses the absolute value of the selected
encoder count, so opposite movement direction still contributes positive
travel distance.

## 6. State Machine

```cpp
enum RobotState {
  STATE_FORWARD,
  STATE_MOVE_LEFT,
  STATE_MOVE_RIGHT,
  STATE_CONFIRM_GAP,
  STATE_STOPPING
};
```

| State | Condition / Meaning | Action |
|---|---|---|
| `STATE_FORWARD` | Front not blocked | `driveForward()` |
| `STATE_MOVE_LEFT` | Front blocked and left side clear | `moveLeft()` |
| `STATE_MOVE_RIGHT` | Front blocked and left side hit, or right-side correction | `moveRight()` |
| `STATE_CONFIRM_GAP` | Front clear while moving sideways | Keep side movement until time and encoder-distance conditions pass |
| `STATE_STOPPING` | `frontBlocked && leftWallHit && rightWallHit` | `stopMotors()` engages A/B brakes until the front clears or a side direction becomes available |

## 7. Dummy Mode Interface

| Switch | Current Value | Effect |
|---|---:|---|
| `USE_DUMMY_MOTORS` | `false` | Motor commands write real PWM/digital outputs |
| `USE_DUMMY_SENSORS` | `true` | `readSensors()` returns dummy sensor sequence |
| `USE_DUMMY_ENCODERS` | `true` | Encoder count increases by `DUMMY_ENCODER_COUNTS_PER_READ` |
| `ENABLE_DEBUG_PRINT` | `true` | Serial Monitor prints sensor/action debug output |

For full dummy mode without real motor motion, set `USE_DUMMY_MOTORS` back to
`true`.

## 8. File Overview

```cpp
config.h     // Set pin numbers, thresholds, and fixed PWM values
sensors.h    // Declare sensor related function and SensorData
sensors.cpp  // Achieve readSensors()
motors.h     // Declare motor related function
motors.cpp   // Achieve driveForward(), moveLeft(), moveRight()
encoders.h   // Declare encoder related function
encoders.cpp // Achieve dummy/real encoder distance check for gap entry
fsm.h        // Declare FSM
fsm.cpp      // Achieve updateDecision(), executeAction()
main_robot.ino // Only be responsible of setup() and loop()
```
