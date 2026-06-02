# Arduino Setup

## Current Setup

Main sketch:

```text
arduino/main_robot/main_robot.ino
```

The sketch includes local project headers:

```cpp
#include "src/config.h"
#include "src/sensors.h"
#include "src/motors.h"
#include "src/fsm.h"
```

The current code uses Arduino core functions such as `pinMode`, `digitalWrite`, `analogWrite`, `analogRead`, `millis`, and `Serial`.

No third-party Arduino library is currently required.

## PlatformIO in VS Code

This repository includes `platformio.ini` for Arduino Mega 2560.

```ini
[platformio]
src_dir = arduino/main_robot

[env:megaatmega2560]
platform = atmelavr
board = megaatmega2560
framework = arduino
monitor_speed = 9600
```

1. Open the repository root folder in VS Code.
2. Install the PlatformIO IDE extension if it is not already installed.
3. Wait for PlatformIO Core to finish initializing.
4. Open the PlatformIO panel.
5. Select `megaatmega2560`.
6. Click Build to compile the project.
7. Connect the Arduino Mega 2560 and select the correct serial port.
8. Click Upload to flash the board.
9. Click Monitor to open the serial monitor at `9600` baud.

Equivalent terminal commands:

```bash
pio run
pio run -e megaatmega2560
pio run -t upload
pio device monitor
```

## Arduino IDE

1. Open `arduino/main_robot/main_robot.ino`.
2. Select the correct Arduino board.
3. Select the correct port.
4. Click Verify to compile.
5. Click Upload after the code compiles.
6. Open Serial Monitor at `9600` baud for debug output.

## VS Code Arduino Workflow

1. Open this repository folder in VS Code.
2. Install an Arduino extension if needed.
3. Open `arduino/main_robot/main_robot.ino`.
4. Select the board and port in the extension.
5. Run verify/compile before upload.

## Before Running on Hardware

- Check [pin map](pin_map.md).
- Check [interface spec](interface_spec.md).
- Confirm M3/M4 H-bridge direction before running side movement.
- Confirm B1/B2 forward PWM wiring before running forward movement.
- Confirm D14-D16 and D30 state LEDs indicate `STATE_MOVE_LEFT`, `STATE_MOVE_RIGHT`, `STATE_CONFIRM_GAP`, and `STATE_FORWARD`.
- Confirm encoder channel A/B wiring on D2/D18/D19/D20 and D22/D24/D26/D28. Channel A uses `RISING`; channel B uses `INPUT_PULLUP`.
- Confirm Role 2 sensor integration: switches are on D7/D4 in the main robot because D2/D3 are reserved, and photodiodes are on A4/A5 with wall threshold 100.
- Both `LEFT_MOVE_ENCODER_INDEX` and `RIGHT_MOVE_ENCODER_INDEX` currently use M1 Front Left because that encoder has been validated.
- `USE_DUMMY_SENSORS` and `USE_DUMMY_ENCODERS` are currently `false` because the Role 2 sensor wiring and Role 4 encoder wiring have been validated.
- Test with wheels raised before testing in the maze.

## Future Libraries

If a third-party Arduino library is added later, document it here with:

- library name
- reason for use
- install method
- example include line
