# Dummy Framework Demo

This demo tests the software framework without real sensor or motor hardware.

## Enable Dummy Mode

Open `arduino/main_robot/src/config.h` and change:

```cpp
const bool USE_DUMMY_MOTORS = true;
const bool USE_DUMMY_SENSORS = true;
const bool USE_DUMMY_ENCODERS = true;
const bool ENABLE_DEBUG_PRINT = true;
```

The current checked-in configuration uses real motor outputs with dummy sensor
and encoder inputs:

```cpp
const bool USE_DUMMY_MOTORS = true;
const bool USE_DUMMY_SENSORS = true;
const bool USE_DUMMY_ENCODERS = true;
```

Set `USE_DUMMY_MOTORS` back to `true` only when running this fully dummy demo
without real motor movement.

## What It Demonstrates

- `readSensors()` can provide dummy sensor data without reading real pins.
- `updateDecision()` can use that data to update the robot state.
- `executeAction()` can call the motor interface.
- The dummy motor interface prints commands instead of writing PWM pins.
- The dummy encoder interface provides side-travel counts for the gap-entry check.

## Dummy Sensor Sequence

The dummy sensor values are defined near the top of `sensors.cpp`.

```cpp
const unsigned long DUMMY_PHASE_MS = 2000;

const int DUMMY_FRONT_CLEAR_RAW = 200;
const int DUMMY_FRONT_BLOCKED_RAW = 700;
const int DUMMY_REAR_RAW = 200;

const bool DUMMY_SIDE_CLEAR = false;
const bool DUMMY_SIDE_HIT = true;
```

The default sequence changes every `DUMMY_PHASE_MS`:

| Phase | Dummy condition | Expected control response |
|---|---|---|
| 0 | Front clear, sides clear | `STATE_FORWARD`, `FORWARD` |
| 1 | Front blocked, left clear | `STATE_MOVE_LEFT`, `LEFT` |
| 2 | Front blocked, left hit | `STATE_MOVE_RIGHT`, `RIGHT` |
| 3 | Front clear again | `STATE_CONFIRM_GAP` until encoder distance is reached, then `STATE_FORWARD` |

## Dummy Encoder Sequence

The dummy encoder values are defined in `config.h`.

```cpp
const bool USE_DUMMY_ENCODERS = true;
const long DUMMY_ENCODER_COUNTS_PER_READ = 2;
```

Each time the FSM checks whether the robot has fully entered the gap, the dummy side encoder count increases by `DUMMY_ENCODER_COUNTS_PER_READ`.

The gap-entry distance formula is:

```text
sideTravel = (sideEncoderCount / ENCODER_COUNTS_PER_REV) * WHEEL_CIRCUMFERENCE_MM + GAP_ENTRY_REDUNDANCY_MM
Move forward when sideTravel > ROBOT_DIAGONAL_MM / 2
```

## Expected Serial Output

With dummy mode and debug printing enabled, the Serial Monitor should show:

```text
Front: ... | Rear: ... | FrontBlocked: ... | FrontClear: ... | LeftHit: ... | RightHit: ...
ACTION = ...
Dummy motor: ... pwm=...
```

This is the evidence that the framework combines dummy sensors, dummy encoder distance, motor commands, and the FSM control process.
