# Pin Map

## Project Pins

| Pin | Code Name | Function |
|---|---|---|
| D14 | `MOVE_LEFT_LED_PIN` | status LED, HIGH during `STATE_MOVE_LEFT` |
| D15 | `MOVE_RIGHT_LED_PIN` | status LED, HIGH during `STATE_MOVE_RIGHT` |
| D16 | `CONFIRM_GAP_LED_PIN` | status LED, HIGH during `STATE_CONFIRM_GAP` |
| D30 | `FORWARD_LED_PIN` | status LED, HIGH during `STATE_FORWARD` |
| D7 | `LEFT_SIDE_SWITCH_PIN` | left side switch |
| D4 | `RIGHT_SIDE_SWITCH_PIN` | right side switch |
| D12 | `FORWARD_MOTOR_A_DIRECTION_PIN` | Arduino Motor Shield Rev3 channel A direction, HIGH = forward convention |
| D13 | `FORWARD_MOTOR_B_DIRECTION_PIN` | Arduino Motor Shield Rev3 channel B direction, LOW = forward convention |
| D9 | `FORWARD_MOTOR_A_BRAKE_PIN` | Arduino Motor Shield Rev3 channel A brake, HIGH = brake |
| D8 | `FORWARD_MOTOR_B_BRAKE_PIN` | Arduino Motor Shield Rev3 channel B brake, HIGH = brake |
| D34 | `A1_H_BRIDGE_IN1_PIN` | M3_A side motor input, forward |
| D36 | `A1_H_BRIDGE_IN2_PIN` | M3_B side motor input, reverse |
| D38 | `A2_H_BRIDGE_IN1_PIN` | M4_A side motor input, forward |
| D40 | `A2_H_BRIDGE_IN2_PIN` | M4_B side motor input, reverse |
| D3 | `B1_FORWARD_PWM_PIN` | B1 forward motor PWM, fixed at 32 |
| D11 | `B2_FORWARD_PWM_PIN` | B2 forward motor PWM, fixed at 32 |
| A4 | `FRONT_PHOTODIODE_PIN` | front photodiode, Role 2 validated analog output |
| A5 | `REAR_PHOTODIODE_PIN` | rear photodiode, Role 2 validated analog output |

Current pin source of truth: `arduino/main_robot/src/config.h`.

Target board: Arduino Mega 2560.
The four status LEDs use Mega digital pins D14-D16 and D30. D13 is reserved
for Motor Shield Rev3 channel B direction and must not be used as an LED pin.

## Motor Control Pins

| Pin | Function | Note |
|---|---|---|
| D3 | `B1_FORWARD_PWM_PIN` | B1 forward motor PWM, fixed at `FORWARD_PWM = 32` |
| D11 | `B2_FORWARD_PWM_PIN` | B2 forward motor PWM, fixed at `FORWARD_PWM = 32` |
| D12 | `FORWARD_MOTOR_A_DIRECTION_PIN` | A direction, HIGH for the Role 4 forward convention |
| D13 | `FORWARD_MOTOR_B_DIRECTION_PIN` | B direction, LOW for the Role 4 forward convention |
| D9 | `FORWARD_MOTOR_A_BRAKE_PIN` | A brake, HIGH in `stopMotors()`, LOW in `driveForward()` |
| D8 | `FORWARD_MOTOR_B_BRAKE_PIN` | B brake, HIGH in `stopMotors()`, LOW in `driveForward()` |
| D34/D36 | `A1_H_BRIDGE_IN1_PIN`, `A1_H_BRIDGE_IN2_PIN` | M3 side motor, inverted side direction control |
| D38/D40 | `A2_H_BRIDGE_IN1_PIN`, `A2_H_BRIDGE_IN2_PIN` | M4 side motor, inverted side direction control |

B1/B2 use Motor Shield Rev3 PWM/brake/direction control. M3/M4 each use one
self-made H-bridge. Side movement is inverted from the original R3 logic: left
sets M3/M4 reverse, and right sets M3/M4 forward.

## Encoder Pins

| Encoder | Motor | Channel A | Channel B | Note |
|---|---|---|---|---|
| Encoder 1 | M1 Front Left | D2 | D22 | A uses RISING interrupt; B uses `INPUT_PULLUP` and is read inside the ISR |
| Encoder 2 | M2 Rear Left | D18 | D24 | A uses RISING interrupt; B uses `INPUT_PULLUP` and is read inside the ISR |
| Encoder 3 | M3 Front Right | D19 | D26 | A uses RISING interrupt; B uses `INPUT_PULLUP` and is read inside the ISR |
| Encoder 4 | M4 Rear Right | D20 | D28 | A uses RISING interrupt; B uses `INPUT_PULLUP` and is read inside the ISR |

The encoder pins are initialized in `initEncoders()`. In the current checked-in
configuration, `USE_DUMMY_ENCODERS = true`, so the FSM uses dummy encoder
counts for integration testing. Set `USE_DUMMY_ENCODERS` to `false` to use real
interrupt counts from the four encoder inputs. The real encoder direction
convention follows the validated Role 4 encoder test: when channel A rises,
channel B LOW increments the count; channel B HIGH decrements the count.
Encoder/mechanical team confirmation: M1/M2 are front/back movement wheels;
M3/M4 are left/right movement wheels.
Encoder readings are used only for distance/gap checks, not for motor speed control.
During `STATE_CONFIRM_GAP`, the FSM selects exactly one encoder: left movement
uses `LEFT_MOVE_ENCODER_INDEX`, and right movement uses
`RIGHT_MOVE_ENCODER_INDEX`. Both are currently set to M3 Front Right because
the encoder team confirmed M3 is used for left/right movement. The distance
calculation uses `abs(count)`, so opposite sign during the opposite side
movement is acceptable.

D2 is reserved for Encoder 1 channel A and D3 is reserved for forward PWM.
The Role 2 standalone sensor test used D2/D3 for switches, but main
integration keeps the side switches on D7/D4 to avoid those conflicts.
Side switch debounce, hit polarity, and optional internal pull-ups are
configured in `config.h`.

## Current Dummy Switches

| Switch | Current Value | Meaning |
|---|---:|---|
| `USE_DUMMY_MOTORS` | `false` | Motor commands write real outputs |
| `USE_DUMMY_SENSORS` | `true` | FSM uses dummy sensor sequence instead of A4/A5 and D7/D4 |
| `USE_DUMMY_ENCODERS` | `true` | FSM uses dummy encoder counts instead of real interrupts |

## Analog Pins

Use A4-A5 for the validated Role 2 photodiode sensors in the current project
layout.

| Pin | Use |
|---|---|
| A0 | Spare |
| A1 | Spare |
| A2 | Spare |
| A3 | Spare |
| A4 | front photodiode |
| A5 | rear photodiode |
