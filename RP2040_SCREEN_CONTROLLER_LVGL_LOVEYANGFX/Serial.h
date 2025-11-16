#ifndef __SERIAL_H__
#define __SERIAL_H__

#include "serial_param_protocol.h"

void serial_read_n();

extern volatile byte    presetNumber;
extern String           presetNameString;

extern volatile bool    presetScrollFlag;

extern volatile byte    paramNumber;
extern volatile int32_t paramValue;
extern String           paramName;

extern volatile bool    paramChangeFlag;

extern volatile bool    updateADSR1Flag;
extern volatile bool    updateADSR2Flag;
 
extern volatile bool    signalFlag;
extern volatile byte    serialSignal;

extern volatile bool    presetCharFlag;
extern volatile byte    presetChar;

extern volatile byte    levelBarFlag;

// +1 for null terminator so LVGL/string APIs see a clean C-string.
extern volatile char    presetNameBytes[17];     // 16-char names + '\0'
extern volatile char    presetNameBytesOLD[17];
#endif

/*
SIGNAL LIST:

1 LOAD (PRESET SCROLL)
2 LOAD/SAVE EXIT
3 SAVE
4 SAVE SET NAME
5 SAVE COMPLETE
6 SAVE - SET NAME - CHAR SELECTION
7
8

*/

//1 bool saw --- bool
//2 bool tri --- bool
//3 bool sin --- bool
//4 bool sqr NO VA
//5 bool saw2 --- bool
//6 bool sqr2 NO VA
//7 SQR1 Level ---
//8 SQR2 Level ---
//9 SUB Level ---
//10 Octave ---- -36 / 12
//11 OSC2Detune  ---  -127
//12 OSC2Interval ---  -24
//13 OSC2LFO  ---
//14 LFO1ToPitch ---
//15 ADSR3ToPitch ---
//16 VelocityToVCF ---
//17 VelocityToVCA ?
//18 PW ?
//19 LFOToPWM
//20 ADSR3ToPWM  ---  -512
//21 LFO1Speed ---
//22 LFO1Shape --- howtodisplay?
//23 LFO2Speed  ---
//24 LFO2Shape ---
//25 LFO3Speed
//26 LFO3Shape
//27 Keytracking ---
//28 Portamento ---
//29 OscPhaseSync ---  mostrar de alguna forma
//30 ResonanceAmpCompensation --- bool
//31 ADSR1Restart
//32 ADSR2Restart --- bool
//33 ADSR3Restart
//34 ADSR1Curves  ----
//35 ADSR2Curves ----
//36
//37 LFO2TOVCF  ---
//38 LFO1TOPWM  ---
//39 LFO1TOVCA ---
//40 VCALEVEL ?
//41 FUNCTION_KEY
//42 ADSR3 ENABLED
