void draw_param_1() {
  //spr0_baseline();
  paramChangeLastMillis = millis();
  // spr[0].setTextColor(TFT_RED, TFT_BLACK);
  // //spr[0].loadFont(AA_FONT_LARGE);
  // spr[0].unloadFont();
  // spr[0].setFreeFont(ORBITRON32);
  // spr[0].setTextDatum(TR_DATUM);
  // spr[0].drawNumber(paramValue, IWIDTH - 5, 0);
  // //spr[0].loadFont(AA_FONT_LARGE);
  // spr[0].setFreeFont(ORBITRON24);
  // spr[0].setTextDatum(TL_DATUM);
  // spr[0].drawString(paramName, 2, 2);

  //spr2.fillSprite(TFT_BLACK);
  spr2.setTextColor(TFT_BLACK);
  spr2.fillRect(0, 0, 320, 30, TFT_RED);
  //spr2.loadFont(AA_FONT_LARGE);
  spr2.unloadFont();
  spr2.setFreeFont(ORBITRON24);
  spr2.setTextDatum(TR_DATUM);
  spr2.drawNumber(paramValue, IWIDTH - 10, 0);
  //spr2.loadFont(AA_FONT_LARGE);
  spr2.setFreeFont(ORBITRON24);
  spr2.setTextDatum(TL_DATUM);
  spr2.drawString(paramName, 8, 0);
}

// void display_preset_1() {
//   spr[0].setFreeFont(ORBITRON32);
//   spr[0].setTextDatum(MC_DATUM);
//   spr[0].drawNumber(presetNumber, IWIDTH / 2, IHEIGHT / 2);
//   //spr[0].loadFont(AA_FONT_LARGE);
//   spr[0].setFreeFont(ORBITRON24);
//   spr[0].setTextDatum(TL_DATUM);
//   // if (serialSignal == 4) {
//   //   spr[0].drawString("_", 6 + (presetChar * 4), 48);
//   // }
//   spr[0].drawString(presetNameString, 6, 40);
// }

void draw_preset_scroll_1() {
  spr[0].fillSprite(TFT_BLACK);
  switch (serialSignal) {
    case 1:
      // display_preset_1();
      break;
    case 2:
      draw_title("", TFT_BLACK, 0xf9e2, 0);
      //display_preset_1();
      break;
    case 4:
      draw_title(" PRESET NAME", TFT_BLACK, 0xf9e2, 0);
      //display_preset_1();
      break;
    case 6:
      break;
  }
  //draw_title("LOAD  PRESET  ", TFT_BLACK, 0xf9e2, 1);
  spr0_baseline();
  spr[0].setTextColor(TFT_RED, TFT_BLACK);
  //spr[0].loadFont(AA_FONT_LARGE);
  spr[0].setFreeFont(ORBITRON48);
  //spr[0].setFreeFont(TESTER24);
  spr[0].setTextDatum(MC_DATUM);
  spr[0].drawNumber(presetNumber, IWIDTH / 2, (IHEIGHT / 2) + 26);
  //spr[0].loadFont(AA_FONT_LARGE);
  //spr[0].setFreeFont(ORBITRON24);
  spr[0].setFreeFont(TESTER24);
  spr[0].setTextDatum(TL_DATUM);

  if (serialSignal == 4) {
    if (timer100msFlag == true) {
      if (timer100msBlink) {
        presetNameString = String((char*)presetNameBytes);
        spr[0].drawString(presetNameString, 12, 38);
      } else {

        byte presetNameBytesblink[12];
        for (int i = 0; i < 11; i++) {
          presetNameBytesblink[i] = presetNameBytes[i];
        }
        presetNameBytesblink[presetChar] = 32;
        presetNameString = String((char*)presetNameBytesblink);
        spr[0].drawString(presetNameString, 12, 38);
      }
    }
  } else {
    spr[0].drawString(presetNameString, 12, 8);
  }
  presetScrollFlag = false;

  presetNameString = String((char*)presetNameBytes);
}

void setDisplayParam() {
  switch (paramNumber) {
    case 1:
      paramName = " OSC1 SAW";
      break;
    case 2:
      paramName = " OSC2 SAW";
      break;
    case 3:
      paramName = " OSC1 TRI";
      break;
    case 4:
      paramName = " OSC1 SIN";
      break;
    case 5:
      paramName = " OSC1 SQR";
      break;
    case 6:
      paramName = " OSC2 SQR";
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
      paramValue -= 24;
      break;
    case 15:
      paramName = " OSC2 Detune";
      paramValue -= 127;
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
      paramName = " SQR1 Level";
      break;
    case 23:
      paramName = " SQR2 Level";
      break;
    case 24:
      paramName = " SUB Level";
      break;
    case 25:
      paramName = " CALIBRATION VAL";
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
      paramName = " ADSR1 Curve";
      break;
    case 49:
      paramName = " ADSR2 Curve";
      break;

    case 126:
      paramName = " ADSR3 ENABLED";
      break;
    case 127:
      paramName = " FUNCTION KEY";
      break;


    case 101:
      paramName = " CALIB MODE";
      break;

    case 885:
      paramName = " VOICE MODE";
      break;
    case 886:
      paramName = " UNISON DETUNE";
      break;

    case 990:
      paramName = " PW";
      break;
    case 991:
      paramName = " LFO3 Speed";
      break;
    case 992:
      paramName = " LFO3 Shape";
      break;
    case 994:
      paramName = " ADSR3 Restart";
      break;
    case 995:
      paramName = " VCA -> LEVEL";
      break;

    default:
      break;
  }
}

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
