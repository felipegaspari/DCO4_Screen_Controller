#include "params_def.h"
#include "param_router.h"

// Type alias for the screen's parameter router value type.
// NOTE: avoid using this alias in function parameter lists because the Arduino
// build system generates prototypes before these typedefs.
using ScreenParamValueT = int32_t;
using ScreenParamDescriptor = ParamDescriptorT<ScreenParamValueT>;

// ---------------------------------------------------------------------------
// Router-backed "apply" functions for parameters that affect the internal
// model state or screen signals (separate from the user-facing text).
// ---------------------------------------------------------------------------

// Mixer levels -> bar values
static void apply_param_sqr1_level(int32_t v) {
  OSC1Level    = (uint8_t)v;
  levelBarFlag = 1;
}

static void apply_param_sqr2_level(int32_t v) {
  OSC2Level    = (uint8_t)v;
  levelBarFlag = 2;
}

static void apply_param_sub_level(int32_t v) {
  SUBLevel     = (uint8_t)v;
  levelBarFlag = 3;
}

// Calibration flags / screen navigation
static void apply_param_calibration_flag(int32_t v) {
  switch ((int32_t)v) {
    case 0:
      serialSignal = 2;
      break;
    case 1:
      serialSignal = 7;
      break;
    default:
      break;
  }
  signalFlag = true;
}

static void apply_param_manual_calibration_flag(int32_t v) {
  switch ((int32_t)v) {
    case 1:
      serialSignal = 8;
      break;
    case 0:
      serialSignal = 7;
      break;
    default:
      break;
  }
  signalFlag = true;
}

static void apply_param_manual_calibration_stage(int32_t v) {
  manualCalibrationStage = (uint8_t)v;
  manualCalibrationOSCN  = manualCalibrationStage / 2;
}

static void apply_param_manual_calibration_offset(int32_t v) {
  offset = (int8_t)v;
}

static void apply_param_gap_from_dco(int32_t v) {
  calibrationGap = (int32_t)v;
}

static void apply_param_ui_calibration_dismiss(int32_t) {
  // EXIT CURRENT MENU
  switch (serialSignal) {
    case 7:
      serialSignal = 2;
      signalFlag   = true;
      break;
    default:
      break;
  }
}

static void apply_param_ui_calibration_menu_mode(int32_t) {
  // CALIBRATION MENU
  serialSignal = 7;
  signalFlag   = true;
}

// Parameter descriptor table for the screen controller.
static const ScreenParamDescriptor screenParamTable[] = {
  { ParamId::PARAM_SQR1_LEVEL,                     apply_param_sqr1_level                       },
  { ParamId::PARAM_SQR2_LEVEL,                     apply_param_sqr2_level                       },
  { ParamId::PARAM_SUB_LEVEL,                      apply_param_sub_level                        },
  { ParamId::PARAM_CALIBRATION_FLAG,               apply_param_calibration_flag                 },
  { ParamId::PARAM_MANUAL_CALIBRATION_FLAG,        apply_param_manual_calibration_flag          },
  { ParamId::PARAM_MANUAL_CALIBRATION_STAGE,       apply_param_manual_calibration_stage         },
  { ParamId::PARAM_MANUAL_CALIBRATION_OFFSET,      apply_param_manual_calibration_offset        },
  { ParamId::PARAM_GAP_FROM_DCO,                   apply_param_gap_from_dco                     },
  { ParamId::PARAM_UI_CALIBRATION_DISMISS,         apply_param_ui_calibration_dismiss           },
  { ParamId::PARAM_UI_CALIBRATION_MENU_MODE,       apply_param_ui_calibration_menu_mode         },
};

static const size_t screenParamTableSize =
  sizeof(screenParamTable) / sizeof(screenParamTable[0]);

void draw_param_1() {

  paramChangeLastMillis = millis();
  paramChangeTimerFlag = true;

  char str[3];
  itoa(paramValue, str, 10);
  lv_label_set_text(ui_CommandMessage, paramName.c_str());
  lv_label_set_text(ui_CommandMessageShadow, paramName.c_str());
  lv_label_set_text(ui_CommandValueShadow, str);
  lv_label_set_text(ui_CommandValue, str);

  lv_obj_remove_flag(ui_BottomMessagePanel, LV_OBJ_FLAG_HIDDEN);
}

void draw_preset_scroll_1() {

  char str[3];
  itoa(presetNumber, str, 10);
  switch (serialSignal) {
    case 1:
      lv_label_set_text(ui_PresetN, str);
      lv_label_set_text(ui_PresetNShadow, str);
      lv_label_set_text(ui_PresetName, (const char *)presetNameBytes);
      lv_label_set_text(ui_PresetNameShadow, (const char *)presetNameBytes);
      break;
    case 2:
      lv_label_set_text(ui_PresetN, str);
      lv_label_set_text(ui_PresetNShadow, str);
      lv_label_set_text(ui_PresetName, (const char *)presetNameBytes);
      lv_label_set_text(ui_PresetNameShadow, (const char *)presetNameBytes);
      break;
    case 3:
      lv_label_set_text(ui_PresetNNew, str);
      lv_label_set_text(ui_PresetNNewShadow, str);
      lv_label_set_text(ui_PresetNameNew, (const char *)presetNameBytes);
      lv_label_set_text(ui_PresetNameNewShadow, (const char *)presetNameBytes);
      break;
    case 4:

      //lv_obj_add_state(ui_PresetNewName, LV_STATE_FOCUSED);
      //lv_textarea_set_text(ui_PresetNewName, (const char *)presetNameBytes);
      lv_textarea_delete_char_forward(ui_PresetNewName);
      lv_textarea_add_char(ui_PresetNewName, (char)presetNameBytes[presetChar]);
      lv_textarea_set_cursor_pos(ui_PresetNewName, presetChar);
      break;
    case 5:
      break;
    case 6:
      break;
  }
}

void drawManualCalibration() {
  char str[3];
  char strLong[8];

  itoa(offset, str, 10);
  lv_label_set_text(ui_calibrationOffset, str);
  lv_label_set_text(ui_calibrationOffsetShadow, str);

  itoa(manualCalibrationOSCN, str, 10);
  lv_label_set_text(ui_oscillatorN, str);
  lv_label_set_text(ui_oscillatorNShadow, str);


  ltoa(calibrationGap, strLong, 10);
  lv_label_set_text(ui_calibrationGap, strLong);
  lv_label_set_text(ui_calibrationGapShadow, strLong);

  if ((manualCalibrationStage % 2) == 0) {
    lv_label_set_text(ui_waveform, "SAW");
    lv_label_set_text(ui_waveformShadow, "SAW");
  } else if (manualCalibrationStage == 1 || manualCalibrationStage == 5 || manualCalibrationStage == 9 || manualCalibrationStage == 13) {
    lv_label_set_text(ui_waveform, "TRI");
    lv_label_set_text(ui_waveformShadow, "TRI");
  } else {
    lv_label_set_text(ui_waveform, "SQR");
    lv_label_set_text(ui_waveformShadow, "SQR");
  }
}

// Apply parameter effects to the internal "model" state (levels, calibration,
// screen signals, etc.) via the shared param_router table above. This is kept
// separate from the user-facing text so setDisplayParam() is easier to reason
// about.
static void applyParamToModelAndSignals() {
  param_router_apply<ScreenParamValueT>(
    screenParamTable,
    screenParamTableSize,
    paramNumber,
    (ScreenParamValueT)paramValue
  );
}

void setDisplayParam() {
  // First update internal model / screen state.
  applyParamToModelAndSignals();

  switch (static_cast<ParamId>(paramNumber)) {
    case ParamId::PARAM_SAW_STATUS:
      paramName = " OSC1 SAW";
      switch (paramValue) {
        case 0:
          paramName = paramName + " OFF";
          break;
        case 1:
          paramName = paramName + " ON";
          break;
        default:
          break;
      }

      break;
    case ParamId::PARAM_SAW2_STATUS:
      paramName = " OSC2 SAW";
      switch (paramValue) {
        case 0:
          paramName = paramName + " OFF";
          break;
        case 1:
          paramName = paramName + " ON";
          break;
        default:
          break;
      }
      break;
    case ParamId::PARAM_TRI_STATUS:
      paramName = " OSC1 TRI";
      switch (paramValue) {
        case 0:
          paramName = paramName + " OFF";
          break;
        case 1:
          paramName = paramName + " ON";
          break;
        default:
          break;
      }
      break;
    case ParamId::PARAM_SINE_STATUS:
      paramName = " OSC1 SIN";
      switch (paramValue) {
        case 0:
          paramName = paramName + " OFF";
          break;
        case 1:
          paramName = paramName + " ON";
          break;
        default:
          break;
      }
      break;
    case ParamId::PARAM_SQR1_STATUS:
      paramName = " OSC1 SQR";
      switch (paramValue) {
        case 0:
          paramName = paramName + " OFF";
          break;
        case 1:
          paramName = paramName + " ON";
          break;
        default:
          break;
      }
      break;
    case ParamId::PARAM_SQR2_STATUS:
      paramName = " OSC2 SQR";
      switch (paramValue) {
        case 0:
          paramName = paramName + " OFF";
          break;
        case 1:
          paramName = paramName + " ON";
          break;
        default:
          break;
      }
      break;
    case ParamId::PARAM_RESONANCE_COMPENSATION:
      paramName = " ResoAmpComp";
      break;
    case ParamId::PARAM_VCA_ADSR_RESTART:
      paramName = " ADSR1 Restart";
      break;
    case ParamId::PARAM_VCF_ADSR_RESTART:
      paramName = " ADSR2 Restart";
      break;
    case ParamId::PARAM_ADSR3_TO_OSC_SELECT:
      switch (paramValue) {
        case 0:
          paramName = " ADSR3 TO OSC1";
          break;
        case 1:
          paramName = " ADSR3 TO OSC2";
          break;
        case 2:
          paramName = " ADSR3 TO BOTH";
          break;
      }
      break;
    case ParamId::PARAM_LFO1_WAVEFORM:
      paramName = " LFO1 Shape";
      break;
    case ParamId::PARAM_LFO2_WAVEFORM:
      paramName = " LFO2 Shape";
      break;
    case ParamId::PARAM_OSC1_INTERVAL:
      paramName = " Octave";
      paramValue = (paramValue - 36) / 12;
      break;
    case ParamId::PARAM_OSC2_INTERVAL:
      paramName = " OSC2 Interval";
      paramValue -= 36;
      break;
    case ParamId::PARAM_OSC2_DETUNE_VAL:
      paramName = " OSC2 Detune";
      paramValue -= 256;
      break;
    case ParamId::PARAM_LFO2_TO_DETUNE2:
      paramName = " LFO2->OSC2 Pitch";
      break;
    case ParamId::PARAM_OSC_SYNC_MODE:
      paramName = " OscPhaseSync";
      break;
    case ParamId::PARAM_PORTAMENTO_TIME:
      paramName = " Portamento";
      break;
    case ParamId::PARAM_VCF_KEYTRACK:
      paramName = " VCF Keytrack";
      break;
    case ParamId::PARAM_VELOCITY_TO_VCF:
      paramName = " Velocity -> VCF";
      break;
    case ParamId::PARAM_VELOCITY_TO_VCA:
      paramName = " Velocity -> VCA";
      break;
    case ParamId::PARAM_SQR1_LEVEL:
      paramName = " OSC1 Level";
      break;
    case ParamId::PARAM_SQR2_LEVEL:
      paramName = " OSC2 Level";
      break;
    case ParamId::PARAM_SUB_LEVEL:
      paramName = " SUB Level";
      break;
    case ParamId::PARAM_CALIBRATION_VALUE:
      paramName = " CALIBRATION VAL";
      break;
    case ParamId::PARAM_VOICE_MODE:
      switch (paramValue) {
        case 0:
          paramName = " MONO";
          break;
        case 1:
          paramName = " POLY";
          break;
        case 2:
          paramName = " UNISON";
          break;
        default:
          break;
      }
      break;
    case ParamId::PARAM_UNISON_DETUNE:
      paramName = " Analog Detune";
      break;
    case ParamId::PARAM_ANALOG_DRIFT_AMOUNT:
      paramName = " Analog Drift";
      break;
    case ParamId::PARAM_ANALOG_DRIFT_SPEED:
      paramName = " Analog Drift Speed";
      break;
    case ParamId::PARAM_ANALOG_DRIFT_SPREAD:
      paramName = " Analog Drift Spread";
      break;
    case ParamId::PARAM_SYNC_MODE:
      paramName = " Sync Mode";
      break;

    case ParamId::PARAM_LFO1_TO_DCO:
      paramName = " LFO1 -> Pitch";
      break;
    case ParamId::PARAM_LFO1_SPEED:
      paramName = " LFO1 Speed";
      break;
    case ParamId::PARAM_LFO2_SPEED:
      paramName = " LFO2 Speed";
      break;
    case ParamId::PARAM_VCA_LEVEL:
      paramName = " VCA -> LEVEL";
      break;
    case ParamId::PARAM_LFO1_TO_VCA:
      paramName = " LFO1 -> VCA";
      break;
    case ParamId::PARAM_LFO2_TO_PW:
      paramName = " LFO2 -> PWM";
      break;
    case ParamId::PARAM_ADSR3_TO_PWM:
      paramName = " ADSR3 -> PWM";
      paramValue -= 512;
      break;
    case ParamId::PARAM_ADSR3_TO_DETUNE1:
      paramName = " ADSR3 -> Pitch";
      break;
    case ParamId::PARAM_ADSR1_ATTACK_CURVE:
      switch (paramValue) {
        case 0:
          paramName = " EXP";
          break;
        case 1:
          paramName = " SOFT";
          break;
        case 2:
          paramName = " STEEP";
          break;
        case 3:
          paramName = " CONCAVE";
          break;
        case 4:
          paramName = " FAST S";
          break;
        case 5:
          paramName = " SLOW THEN LIN";
          break;
        case 6:
          paramName = " ALMOST LIN";
          break;
        case 7:
          paramName = " LINEAR";
          break;
        case 100:
          paramName = " ADSR1 Curves";
          break;
        default:
          break;
      }
      break;
    case ParamId::PARAM_ADSR1_DECAY_CURVE:
      switch (paramValue) {
        case 0:
          paramName = " EXP";
          break;
        case 1:
          paramName = " SOFT";
          break;
        case 2:
          paramName = " STEEP";
          break;
        case 3:
          paramName = " CONVEX";
          break;
        case 4:
          paramName = " FAST START S";
          break;
        case 5:
          paramName = " SLOW THEN LIN";
          break;
        case 6:
          paramName = " FAST THEN LIN";
          break;
        case 7:
          paramName = " ALMOST LIN";
          break;
        case 8:
          paramName = " LINEAR";
          break;
        case 100:
          paramName = " ADSR1 Decay";
          break;
        default:
          break;
      }
      break;
    case ParamId::PARAM_ADSR2_ATTACK_CURVE:
      switch (paramValue) {
        case 0:
          paramName = " EXP";
          break;
        case 1:
          paramName = " SOFT";
          break;
        case 2:
          paramName = " STEEP";
          break;
        case 3:
          paramName = " CONCAVE";
          break;
        case 4:
          paramName = " FAST S";
          break;
        case 5:
          paramName = " SLOW THEN LIN";
          break;
        case 6:
          paramName = " ALMOST LIN";
          break;
        case 7:
          paramName = " LINEAR";
          break;
        case 100:
          paramName = " ADSR2 Curves";
          break;
        default:
          break;
      }
      break;
    case ParamId::PARAM_ADSR2_DECAY_CURVE:
      switch (paramValue) {
        case 0:
          paramName = " EXP";
          break;
        case 1:
          paramName = " SOFT";
          break;
        case 2:
          paramName = " STEEP";
          break;
        case 3:
          paramName = " CONVEX";
          break;
        case 4:
          paramName = " FAST START S";
          break;
        case 5:
          paramName = " SLOW THEN LIN";
          break;
        case 6:
          paramName = " FAST THEN LIN";
          break;
        case 7:
          paramName = " ALMOST LIN";
          break;
        case 8:
          paramName = " LINEAR";
          break;
        case 100:
          paramName = " ADSR2 Decay";
          break;
        default:
          break;
      }
      break;

    case ParamId::PARAM_FADERS_CONTROL_MANUAL:
      paramName = " MAN FADERS";
      break;
    case ParamId::PARAM_FADER_ROW1_CONTROL_MANUAL:
      paramName = " MAN FADERS 1";
      break;
    case ParamId::PARAM_FADER_ROW2_CONTROL_MANUAL:
      paramName = " MAN FADERS 2";
      break;
    case ParamId::PARAM_VCF_POTS_CONTROL_MANUAL:
      paramName = " MANUAL VCF";
      break;
    case ParamId::PARAM_PWM_POTS_CONTROL_MANUAL:
      paramName = " MANUAL PWM";
      break;
    case ParamId::PARAM_ALL_CONTROLS_MANUAL:
      paramName = " ALL CONTROLS MANUAL";
      break;
    case ParamId::PARAM_ADSR3_ENABLED:
      paramName = " ADSR3 ENABLED";
      break;
    case ParamId::PARAM_FUNCTION_KEY:
      paramName = " FUNCTION KEY";
      break;
    case ParamId::PARAM_VCA_POTS_CONTROL_MANUAL:
      paramName = " MANUAL VCA";
      break;
    case ParamId::PARAM_POTS_CONTROL_MANUAL:
      paramName = " MANUAL POTS";
      break;



    case ParamId::PARAM_CALIBRATION_FLAG:
      paramName = " AUTO CALIBRATION";
      break;
    case ParamId::PARAM_MANUAL_CALIBRATION_FLAG:
      paramName = " MANUAL CALIBRATION";
      break;
    case ParamId::PARAM_MANUAL_CALIBRATION_STAGE:  // manual calibration stage
      paramName = "OSCILLATOR N";
      break;
    case ParamId::PARAM_MANUAL_CALIBRATION_OFFSET:  // manual calibration offset
      paramName = " OFFSET";
      break;
    case ParamId::PARAM_GAP_FROM_DCO:  // manual calibration GAP
      paramName = " GAP";
      break;

    case ParamId::PARAM_UI_MENU_POSITION:  // MENU POSITION
      break;

    case ParamId::PARAM_UI_CALIBRATION_DISMISS:  // EXIT CURRENT MENU (handled in applyParamToModelAndSignals)
      break;

    case ParamId::PARAM_UI_CALIBRATION_MENU_MODE:  // CALIBRATION MENU (handled in applyParamToModelAndSignals)
      break;

    case ParamId::PARAM_PW_VALUE:
      paramName = " PW";
      break;
    case ParamId::PARAM_LFO3_SPEED:
      paramName = " LFO3 Speed";
      break;
    case ParamId::PARAM_LFO3_WAVEFORM:
      paramName = " LFO3 Shape";
      break;
    case ParamId::PARAM_ADSR3_RESTART:
      paramName = " ADSR3 Restart";
      break;
    case ParamId::PARAM_VCA_LEVEL_ALT:
      paramName = " VCA -> LEVEL";
      break;

    default:
      break;
  }
}
//1 bool saw
//2 bool tri
//3 bool sin
//4 bool sqr
//5 bool saw2
//6 bool sqr2
//7 sqrLevel
//8 sqr2Level
//9 SUBLevel
//10 Octave
//11 OSC2Detune
//12 OSC2Interval
//13 OSC2LFO
//14 LFO1ToPitch
//15 ADSR3ToPitch
//16 VelocityToVCF
//17 VelocityToVCA
//18 PW
//19 LFOToPWM
//20 ADSR3ToPWM
//21 LFO1Speed
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
//43 ADSR3 TO OSC SELECT
//44 VOICE MODE
//45 UNISON DETUNE
//46 CALIBRATION MODE
//47 CALIBRATION VAL

/*
// bytes
  TG_SAW1, 1
  TG_SAW2, 2
  TG_TRI, 3
  TG_SIN, 4
  TG_SQR1, 5
  TG_SQR2, 6
  TG_RESO_AMP_COMP, 7
  TG_ADSR1_RESTART, 8
  TG_ADSR2_RESTART, 9
  TG_ADSR3_TO_OSC_SELECT, 10
  TG_LFO1_WAVE, 11
  TG_LFO2_WAVE, 12
// bytes
  ACTION_octave, 13
  ACTION_OSC2_interval, 14
  ACTION_OSC2_detune, 15
  ACTION_LFO2_to_OSC2_detune, 16
  ACTION_osc_sync_mode, 17
  ACTION_portamento_time, 18
  ACTION_VCF_keytrack, 19
  ACTION_velocity_to_VCF, 20
  ACTION_velocity_to_VCA, 21
  ACTION_SQR1_level, 22
  ACTION_SQR2_level, 23
  ACTION_SUB_level, 24
  ACTION_calibration, 25

  TG_VOICE_MODE, 26
  ACTION_UNISON_DETUNE, 27

  //uint16_t
  ACTION_LFO1_to_DCO, 40
  ACTION_LFO1_speed, 41
  ACTION_LFO2_speed, 42
  ACTION_VCA_level, 43
  ACTION_LFO1_to_VCA, 44
  ACTION_LFO2_to_PWM, 45
  ACTION_ADSR3_to_PWM, 46
  ACTION_ADSR3_to_DETUNE1, 47

// solo a SCREEN_CONTROLLER
  ACTION_select_preset, 100
  ACTION_calibration, 101
  // solo a SCREEN_CONTROLLER
  SELECT_LFO_N, 110
  WORK_WITH_PRESETS, 111
  PRESET_SAVE_MODE, 112
  SAVE_PRESET, 113

    // no envian datos ?
  TG_MAN_FADERS, 120
  TG_MAN_FADER_ROW1, 121
  TG_MAN_FADER_ROW2, 122
  TG_MANUAL_VCF_POTS, 123
  TG_MANUAL_PWM_POTS, 124
  TG_MANUAL_ALL, 125
  TG_ENABLE_ADSR3, 126
  TG_FUNC, 127
  SELECT_ENC_ACTION, 128
  */
