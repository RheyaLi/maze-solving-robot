# maze-solving-robot

Arduino maze-solving robot project for Scenario B ELEC0035 UCL.

## Start Here

- [Project context](PROJECT_CONTEXT.md)
- [Robot behavior](docs/robot_behavior.md)
- [Pin map](docs/pin_map.md)
- [Interface spec](docs/interface_spec.md)
- [Arduino setup](docs/setup_arduino.md)

## Current Framework

The main sketch connects three subsystems:

- sensors: `readSensors()` returns front, rear, left-side, and right-side readings
- decision logic: `updateDecision()` and `executeAction()` run the FSM
- motors: `driveForward()`, `moveLeft()`, and `moveRight()` issue movement commands
- encoders: `gapEntryDistanceReached()` checks whether the robot body has entered a detected gap

B1/B2 PWM pins drive forward motion at the fixed `FORWARD_PWM` value. M3/M4 are separate self-made H-bridge side motors using inverted side logic: M3/M4 reverse for left movement and M3/M4 forward for right movement. Encoder readings are used for distance/gap checks only, not for motor speed control.

## Build and Upload

This project includes a PlatformIO configuration for Arduino Mega 2560.

```bash
pio run
pio run -e megaatmega2560
pio run -t upload
pio device monitor
```

The serial monitor baud rate is `9600`. See [Arduino setup](docs/setup_arduino.md) for the full VS Code and Arduino IDE workflow.

## Source Materials

- [Flowchart image](docs/assets/flowchart-UWL.png)
