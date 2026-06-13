#ifndef ENCODERS_H
#define ENCODERS_H

void initEncoders();
void resetSideEncoderCount();
void resetAllEncoderCounts();
void selectSideEncoderForLeftMove();
void selectSideEncoderForRightMove();

long getEncoderCount(int encoderIndex);
long peekEncoderCount(int encoderIndex);
float getEncoderDistanceMm(int encoderIndex);
long getSideEncoderCount();
long getSideConfirmEncoderCount();
float getSideConfirmTravelMm();
float getForwardTravelMm();
float getSideTravelMm();
bool gapEntryDistanceReached();
void printEncoders();

#endif
