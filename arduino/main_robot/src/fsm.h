#ifndef FSM_H
#define FSM_H

#include "sensors.h"

enum RobotState {
  STATE_FORWARD,
  STATE_MOVE_LEFT,
  STATE_MOVE_RIGHT,
  STATE_PASS_GAP,
  STATE_STOPPING
};

void initFSM();

void updateDecision(const SensorData &s);
void executeAction();

RobotState getCurrentState();
bool isGapConfirming();
const char* getStateName(RobotState state);

#endif
