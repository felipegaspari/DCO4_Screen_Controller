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
  } else if (manualCalibrationStage == 1 || manualCalibrationStage == 5 || manualCalibrationStage == 9 || manualCalibrationStage == 14) {
    lv_label_set_text(ui_waveform, "TRI");
    lv_label_set_text(ui_waveformShadow, "TRI");
  } else {
    lv_label_set_text(ui_waveform, "SQR");
    lv_label_set_text(ui_waveformShadow, "SQR");
  }
}

// Apply parameter effects to the internal "model" state (levels, calibration,
// screen signals, etc.). This is kept separate from the user-facing text so
// setDisplayParam() is easier to reason about.
static void applyParamToModelAndSignals() {
  switch (paramNumber) {
    // Mixer levels -> bar values
    case 22:
      OSC1Level   = paramValue;
      levelBarFlag = 1;
      break;
    case 23:
      OSC2Level   = paramValue;
      levelBarFlag = 2;
      break;
    case 24:
      SUBLevel    = paramValue;
      levelBarFlag = 3;
      break;

    // Calibration flags / screen navigation
    case 150:  // AUTO CALIBRATION
      switch (paramValue) {
        case 0:
          serialSignal = 2;
          break;
        case 1:
          serialSignal = 7;
          break;
      }
      signalFlag = true;
      break;

    case 151:  // MANUAL CALIBRATION
      switch (paramValue) {
        case 1:
          serialSignal = 8;
          break;
        case 0:
          serialSignal = 7;
          break;
      }
      signalFlag = true;
      break;

    case 152:  // manual calibration stage
      manualCalibrationStage = paramValue;
      manualCalibrationOSCN  = manualCalibrationStage / 2;
      break;

    case 153:  // manual calibration offset
      offset = (int8_t)paramValue;
      break;

    case 154:  // manual calibration GAP
      calibrationGap = (int32_t)paramValue;
      break;

    case 199:  // EXIT CURRENT MENU
      switch (serialSignal) {
        case 7:
          serialSignal = 2;
          signalFlag   = true;
          break;
      }
      break;

    case 200:  // CALIBRATION MENU
      serialSignal = 7;
      signalFlag   = true;
      break;

    default:
      // Other parameters only affect display text right now.
      break;
  }
}

void setDisplayParam() {
  // First update internal model / screen state.
  applyParamToModelAndSignals();

  switch (paramNumber) {
    case 1:
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
    case 2:
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
    case 3:
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
    case 4:
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
    case 5:
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
    case 6:
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
    case 7:
      paramName = " ResoAmpComp";
      break;
    case 8:
      paramName = " ADSR1 Restart";
      break;
    case 9:
      paramName = " ADSR2 Restart";
      break;
    case 10:
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
    case 11:
      paramName = " LFO1 Shape";
      break;
    case 12:
      paramName = " LFO2 Shape";
      break;
    case 13:
      paramName = " Octave";
      paramValue = (paramValue - 36) / 12;
      break;
    case 14:
      paramName = " OSC2 Interval";
      paramValue -= 36;
      break;
    case 15:
      paramName = " OSC2 Detune";
      paramValue -= 256;
      break;
    case 16:
      paramName = " LFO2->OSC2 Pitch";
      break;
    case 17:
      paramName = " OscPhaseSync";
      break;
    case 18:
      paramName = " Portamento";
      break;
    case 19:
      paramName = " VCF Keytrack";
      break;
    case 20:
      paramName = " Velocity -> VCF";
      break;
    case 21:
      paramName = " Velocity -> VCA";
      break;
    case 22:
      paramName = " OSC1 Level";
      break;
    case 23:
      paramName = " OSC2 Level";
      break;
    case 24:
      paramName = " SUB Level";
      break;
    case 25:
      paramName = " CALIBRATION VAL";
      break;
    case 26:
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
    case 27:
      paramName = " Analog Detune";
      break;
    case 28:
      paramName = " Analog Drift";
      break;
    case 29:
      paramName = " Analog Drift Speed";
      break;
    case 30:
      paramName = " Analog Drift Spread";
      break;
    case 31:
      paramName = " Sync Mode";
      break;

    case 40:
      paramName = " LFO1 -> Pitch";
      break;
    case 41:
      paramName = " LFO1 Speed";
      break;
    case 42:
      paramName = " LFO2 Speed";
      break;
    case 43:
      paramName = " VCA -> LEVEL";
      break;
    case 44:
      paramName = " LFO1 -> VCA";
      break;
    case 45:
      paramName = " LFO2 -> PWM";
      break;
    case 46:
      paramName = " ADSR3 -> PWM";
      paramValue -= 512;
      break;
    case 47:
      paramName = " ADSR3 -> Pitch";
      break;
    case 48:
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
    case 49:
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
    case 50:
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
    case 51:
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

    case 120:
      paramName = " MAN FADERS";
      break;
    case 121:
      paramName = " MAN FADERS 1";
      break;
    case 122:
      paramName = " MAN FADERS 2";
      break;
    case 123:
      paramName = " MANUAL VCF";
      break;
    case 124:
      paramName = " MANUAL PWM";
      break;
    case 125:
      paramName = " ALL CONTROLS MANUAL";
      break;
    case 126:
      paramName = " ADSR3 ENABLED";
      break;
    case 127:
      paramName = " FUNCTION KEY";
      break;
    case 128:
      paramName = " MANUAL VCA";
      break;
    case 129:
      paramName = " MANUAL POTS";
      break;



    case 150:
      paramName = " AUTO CALIBRATION";
      break;
    case 151:
      paramName = " MANUAL CALIBRATION";
      break;
    case 152:  // manual calibration stage
      paramName = "OSCILLATOR N";
      break;
    case 153:  // manual calibration offset
      paramName = " OFFSET";
      break;
    case 154:  // manual calibration GAP
      paramName = " GAP";
      break;

    case 190:  // MENU POSITION
      break;

    case 199:  // EXIT CURRENT MENU (handled in applyParamToModelAndSignals)
      break;

    case 200:  // CALIBRATION MENU (handled in applyParamToModelAndSignals)
      break;

    case 210:
      paramName = " PW";
      break;
    case 211:
      paramName = " LFO3 Speed";
      break;
    case 212:
      paramName = " LFO3 Shape";
      break;
    case 214:
      paramName = " ADSR3 Restart";
      break;
    case 215:
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
