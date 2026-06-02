# Robot Behavior

This page keeps the original flowchart logic in a code-friendly form.

## Main Logic

```text
There's no wall ahead
-> Keep going

There is a wall ahead
-> Is there a blockage on the left?
   Yes -> Move to the right
   No  -> Move left
```

## Flowchart Notes

```text
Use range instead of single number for a threshold to solve "an unstable front wall is detected";
Move left or right while searching for a gap;
Front-clear detection has priority over side-hit detection while moving sideways;
After detecting a gap, keep moving sideways until the encoder distance shows the robot body has entered the gap.
```

![Flowchart](assets/flowchart-UWL.png)
![alt text](image.png)

## Current State Machine

The current Arduino code represents the behavior with these states:

- `STATE_FORWARD`: move forward when the front is clear.
- `STATE_MOVE_LEFT`: move left while monitoring the front and left side sensors.
- `STATE_MOVE_RIGHT`: move right while monitoring the front and right side sensors.
- `STATE_CONFIRM_GAP`: keep side movement until the front stays clear and side travel is greater than half the robot diagonal, then move forward.
- `STATE_STOPPING`: stop all motors when the front is blocked and both side switches are hit. Forward PWM is set to 0 and the Motor Shield Rev3 A/B brake pins are driven HIGH.

Side movement rules:

- If moving left and the left side switch is hit, change to `STATE_MOVE_RIGHT`.
- If moving right and the right side switch is hit, change to `STATE_MOVE_LEFT`.
- If `frontClear` is true at the same time as a side hit, `frontClear` has priority.
- If `frontBlocked`, `leftWallHit`, and `rightWallHit` are all true, enter `STATE_STOPPING`.
- From `STATE_STOPPING`, move forward again when `frontClear` is true, or resume side movement if the front is blocked but one side is available. Forward brakes are released only when `driveForward()` runs.

Gap entry rule:

```text
sideTravel = encoderTurns * wheelCircumference + redundancy
encoderTurns = sideEncoderCount / countsPerRevolution
Move forward only when sideTravel > robotDiagonal / 2
```

Current placeholder constants are defined in `config.h` and must be replaced with measured robot values.

Current threshold behavior:

- `frontBlocked` is true when `frontRaw > FRONT_WALL_THRESHOLD`.
- `frontClear` is true when `frontBlocked` is false.
- `rearWallDetected` is true when `rearRaw > REAR_WALL_THRESHOLD`; the rear sensor is printed for debug/status but is not currently used by the FSM.
- Left/right side switches are debounced for 50 ms before updating `leftWallHit` and `rightWallHit`.

Current source files:

- `arduino/main_robot/src/fsm.h`
- `arduino/main_robot/src/fsm.cpp`
- `arduino/main_robot/src/sensors.h`
- `arduino/main_robot/src/sensors.cpp`
- `arduino/main_robot/src/encoders.h`
- `arduino/main_robot/src/encoders.cpp`
