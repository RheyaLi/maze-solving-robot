#ifndef SENSORS_H
#define SENSORS_H

struct SensorData {
  int frontRaw;
  int rearRaw;

  bool frontBlocked;
  bool frontClear;
  bool rearWallDetected;

  int leftSideRaw;
  int rightSideRaw;

  bool leftWallHit;
  bool rightWallHit;
};

void initSensors();
SensorData readSensors();
void printSensors(const SensorData &s);

#endif
