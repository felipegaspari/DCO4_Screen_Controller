#ifndef PARAMS_DEF_MAINBOARD_H
#define PARAMS_DEF_MAINBOARD_H

#include <stdint.h>

// Shared / central definition of parameter IDs for the DCO4 system.
//
// This file is used by the *mainboard* firmware. The DCO firmware has its own
// params_def.h with the same numeric values for the overlapping IDs so that
// parameters sent over Serial are interpreted consistently.
//
// IMPORTANT:
//   - Do not change numeric values of existing IDs.
//   - New parameters should get new, unused numbers.
//   - The meaning of each ID (name + number) should be stable across MCUs.

enum ParamId : uint8_t {
  // --- Oscillator wave enable (mainboard-local) ---------------------
  PARAM_SAW_STATUS               = 1,   // sawStatus
  PARAM_SAW2_STATUS              = 2,   // saw2Status
  PARAM_TRI_STATUS               = 3,   // triStatus
  PARAM_SINE_STATUS              = 4,   // sineStatus

  // Shared with DCO: 5, 10.. etc
  PARAM_SQR1_STATUS              = 5,   // sqr1Status
  PARAM_SQR2_STATUS              = 6,   // sqr2Status (mainboard-local)

  PARAM_RESONANCE_COMPENSATION   = 7,   // RESONANCEAmpCompensation (mainboard-local)
  PARAM_VCA_ADSR_RESTART         = 8,   // VCAADSRRestart (mainboard-local)
  PARAM_VCF_ADSR_RESTART         = 9,   // VCFADSRRestart (mainboard-local)

  // --- Shared routing / oscillator parameters ----------------------
  PARAM_ADSR3_TO_OSC_SELECT      = 10,  // ADSR3ToOscSelect

  PARAM_LFO1_WAVEFORM            = 11,
  PARAM_LFO2_WAVEFORM            = 12,

  PARAM_OSC1_INTERVAL            = 13,  // OSC1Interval / OSC1_interval
  PARAM_OSC2_INTERVAL            = 14,  // OSC2Interval / OSC2_interval

  PARAM_OSC2_DETUNE_VAL          = 15,  // OSC2Detune / OSC2DetuneVal
  PARAM_LFO2_TO_DETUNE2          = 16,  // LFO2toOSC2DETUNE / LFO2toDETUNE2

  PARAM_OSC_SYNC_MODE            = 17,  // oscSyncMode / oscSync

  PARAM_PORTAMENTO_TIME          = 18,  // portamentoTime / portamento_time

  // --- Mainboard-local filter/velocity routing ---------------------
  PARAM_VCF_KEYTRACK             = 19,  // VCFKeytrack
  PARAM_VELOCITY_TO_VCF          = 20,  // velocityToVCFVal
  PARAM_VELOCITY_TO_VCA          = 21,  // velocityToVCAVal
  // 22,23,24 are also mainboard-local levels:
  PARAM_SQR1_LEVEL               = 22,  // SQR1LevelVal
  PARAM_SQR2_LEVEL               = 23,  // SQR2LevelVal
  PARAM_SUB_LEVEL                = 24,  // SubLevelVal

  // --- Shared calibration / voice mode ------------------------------
  PARAM_CALIBRATION_VALUE        = 25,  // calibrationVal (reserved on DCO)

  PARAM_VOICE_MODE               = 26,
  PARAM_UNISON_DETUNE            = 27,

  PARAM_ANALOG_DRIFT_AMOUNT      = 28,
  PARAM_ANALOG_DRIFT_SPEED       = 29,
  PARAM_ANALOG_DRIFT_SPREAD      = 30,

  PARAM_SYNC_MODE                = 31,  // DCO syncMode

  // 32: DCO-only portamento mode selector (currently not used on mainboard)
  PARAM_PORTAMENTO_MODE          = 32,

  // --- LFO routing (shared) -----------------------------------------
  PARAM_LFO1_TO_DCO              = 40,
  PARAM_LFO1_SPEED               = 41,
  PARAM_LFO2_SPEED               = 42,

  // --- Mainboard-only VCA routing ----------------------------------
  PARAM_VCA_LEVEL                = 43,  // VCALevel
  PARAM_LFO1_TO_VCA              = 44,  // LFO1toVCA

  // --- PWM / ADSR to PWM / detune (shared with DCO where relevant) -
  PARAM_LFO2_TO_PW               = 45,  // LFO2toPWM / LFO2toPW
  PARAM_ADSR3_TO_PWM             = 46,  // ADSR3toPWM / ADSR1toPWM on DCO
  PARAM_ADSR3_TO_DETUNE1         = 47,  // ADSR3toDETUNE1 / ADSR1toDETUNE1 on DCO

  // ADSR curve shaping (mainboard-local; DCO has its own ADSR params)
  PARAM_ADSR1_ATTACK_CURVE       = 48,  // ADSR1AttackCurveVal
  PARAM_ADSR1_DECAY_CURVE        = 49,  // ADSR1DecayCurveVal
  PARAM_ADSR2_ATTACK_CURVE       = 50,  // ADSR2AttackCurveVal
  PARAM_ADSR2_DECAY_CURVE        = 51,  // ADSR2DecayCurveVal

  // --- Misc / control / UI flags -----------------------------------
  // Calibration mode selector (screen/UI only for now)
  PARAM_CALIBRATION_MODE         = 101, // "CALIB MODE" on screen

  // Global/manual control flags (input+screen; mainboard/DCO may ignore)
  PARAM_FADERS_CONTROL_MANUAL    = 120, // MAN FADERS (both rows)
  PARAM_FADER_ROW1_CONTROL_MANUAL= 121, // MAN FADERS 1
  PARAM_FADER_ROW2_CONTROL_MANUAL= 122, // MAN FADERS 2
  PARAM_VCF_POTS_CONTROL_MANUAL  = 123, // MANUAL VCF
  PARAM_PWM_POTS_CONTROL_MANUAL  = 124, // MANUAL PWM
  PARAM_ALL_CONTROLS_MANUAL      = 125, // ALL CONTROLS MANUAL

  PARAM_ADSR3_ENABLED            = 126, // ADSR3Enabled (mainboard/input/screen)
  PARAM_FUNCTION_KEY             = 127, // FUNCTION KEY

  PARAM_VCA_POTS_CONTROL_MANUAL  = 128, // MANUAL VCA
  PARAM_POTS_CONTROL_MANUAL      = 129, // MANUAL POTS (global pot manual)

  // UI navigation / calibration helper parameters (screen-focused)
  PARAM_UI_MENU_POSITION         = 190, // menuPos on input/screen
  PARAM_UI_CALIBRATION_DISMISS   = 199, // hide calibration UI / dialog
  PARAM_UI_CALIBRATION_MENU_MODE = 200, // enter/exit calibration menu

  // Reserved / screen-only extras (future expansion)
  PARAM_PW_VALUE                 = 210, // "PW" on screen (alternative to 'f' block)
  PARAM_LFO3_SPEED               = 211, // "LFO3 Speed" (future)
  PARAM_LFO3_WAVEFORM            = 212, // "LFO3 Shape" (future)
  PARAM_ADSR3_RESTART            = 214, // "ADSR3 Restart" (future)
  PARAM_VCA_LEVEL_ALT            = 215, // second VCA level mapping (screen-only)

  // --- Calibration flags (shared) ----------------------------------
  PARAM_CALIBRATION_FLAG         = 150,
  PARAM_MANUAL_CALIBRATION_FLAG  = 151,
  PARAM_MANUAL_CALIBRATION_STAGE = 152,
  PARAM_MANUAL_CALIBRATION_OFFSET= 153,

  // PARAM 154: 32-bit "gap from DCO" (forwarded to screen on mainboard)
  PARAM_GAP_FROM_DCO             = 154,

  // 155: manual calibration offsets reported from DCO back to mainboard/screen.
  PARAM_MANUAL_CALIBRATION_OFFSET_FROM_DCO = 155
};

#endif  // PARAMS_DEF_MAINBOARD_H


