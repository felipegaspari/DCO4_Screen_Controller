#ifndef __DISPLAY_PARAMS_H__
#define __DISPLAY_PARAMS_H__

uint16_t paramHideTimeMillis = 3000;
bool paramChangeTimerFlag = false;

volatile int8_t offset;
volatile uint8_t manualCalibrationOSCN;
volatile uint8_t manualCalibrationStage;
volatile int32_t calibrationGap;

#endif