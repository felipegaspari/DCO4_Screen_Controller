#ifndef __DISPLAY_PARAMS_H__
#define __DISPLAY_PARAMS_H__

uint16_t paramHideTimeMillis = 3000;
bool paramChangeTimerFlag = false;

volatile int8_t offset;
volatile uint8_t manualCalibrationOSCN;
volatile uint8_t manualCalibrationStage;
volatile int32_t calibrationGap;

volatile uint8_t OSC1Level;
volatile uint8_t OSC2Level;
volatile uint8_t SUBLevel;

volatile uint16_t ADSR1Attack ;
volatile uint16_t ADSR1Decay ;
volatile uint16_t ADSR1Sustain ;
volatile uint16_t ADSR1Release ;

volatile uint16_t ADSR2Attack ;
volatile uint16_t ADSR2Decay ;
volatile uint16_t ADSR2Sustain ;
volatile uint16_t ADSR2Release ;




#endif