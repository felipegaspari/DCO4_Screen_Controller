


#include "Arduino.h"

#include <lvgl.h>
//#include <TFT_eSPI.h>
#define LGFX_USE_V1
#define LV_COLOR_16_SWAP 0

#include <LovyanGFX.hpp>
#include <lgfx_user/LGFX_RP2040_FELA.hpp>

#include <ui.h>

#include "globals.h"
#include "Serial.h"
#include "auxiliary.h"
#include "timers_millis.h"
#include "displayParams.h"

// High-level screen modes driven by serialSignal.
// Keep the numeric values to preserve the existing protocol.
enum class ScreenMode : uint8_t {
  PresetScroll      = 1,  // LOAD (PRESET SCROLL)
  LoadSaveExit      = 2,  // LOAD/SAVE EXIT
  SaveSelectPreset  = 3,  // SAVE MODE - Select destination preset
  SaveSetName       = 4,  // SAVE MODE - set preset name
  SaveCompleted     = 5,  // PRESET SAVED
  Silent            = 6,  // SCREEN SILENCE
  CalibrationMenu   = 7,  // CALIBRATION MENU
  ManualCalibration = 8   // MANUAL CALIBRATION
};

// Map serialSignal byte to ScreenMode enum.
inline ScreenMode getScreenMode() {
  return static_cast<ScreenMode>(serialSignal);
}

// Write ScreenMode into serialSignal (protocol byte).
inline void setScreenMode(ScreenMode mode) {
  serialSignal = static_cast<uint8_t>(mode);
}

static const uint16_t screenWidth = 480;
static const uint16_t screenHeight = 320;

enum { SCREENBUFFER_SIZE_PIXELS = screenWidth * screenHeight / 10 };
static lv_color_t buf[SCREENBUFFER_SIZE_PIXELS];

LGFX tft; /* TFT instance */

#if LV_USE_LOG != 0
/* Serial debugging */
void my_print(const char *buf) {
  Serial.printf(buf);
  Serial.flush();
}
#endif

/* Display flushing: push LVGL dirty area to LovyanGFX, then mark flush ready. */
void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *pixelmap) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  if (LV_COLOR_16_SWAP) {
    size_t len = lv_area_get_size(area);
    lv_draw_sw_rgb565_swap(pixelmap, len);
  }

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushPixels((uint16_t *)pixelmap, w * h, true);
  tft.endWrite();

  lv_disp_flush_ready(disp);
}

/* Touchpad read — unused (display-only; leaves data untouched). */
void my_touchpad_read(lv_indev_t *indev_driver, lv_indev_data_t *data) {
}

/* LVGL tick source: Arduino millis(). */
static uint32_t my_tick_get_cb(void) {
  return millis();
}



uint32_t paramChangeLastMillis = 0;
/////////////////////////////////////////////////////////// setup ///////////////////////////////////////////////////
// Core0 boot: USB debug Serial + UART1 (Input) + UART2 (Mainboard).
void setup() {
  //SPI.setClockDivider(SPI_CLOCK_DIV2);

  Serial.begin(1000000);

  Serial1.setRX(13);
  Serial1.setTX(12);
  Serial1.setPollingMode(true);
  Serial1.setFIFOSize(512);
  Serial1.begin(2500000);

  Serial2.setRX(21);
  Serial2.setTX(20);
  Serial2.setPollingMode(true);
  Serial2.setFIFOSize(512);
  Serial2.begin(2500000);

  // USBDevice.setManufacturerDescriptor("FELA         ");   /// Why doesnt it work?
  // USBDevice.setProductDescriptor("DCO4 Screen Controller       ");
}


// Core1 boot: LVGL + LovyanGFX display + SquareLine ui_init.
void setup1() {
  lv_init();

  tft.begin();        /* TFT init */
  tft.setRotation(3); /* Landscape orientation, flipped */

  static lv_disp_t *disp;
  disp = lv_display_create(screenWidth, screenHeight);
  lv_display_set_buffers(disp, buf, NULL, SCREENBUFFER_SIZE_PIXELS * sizeof(lv_color_t), LV_DISPLAY_RENDER_MODE_PARTIAL);
  lv_display_set_flush_cb(disp, my_disp_flush);

  static lv_indev_t *indev;
  indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, my_touchpad_read);

  lv_tick_set_cb(my_tick_get_cb);

  ui_init();
  lv_obj_set_style_anim_time(ui_PresetNewName, 140, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_anim_time(ui_PresetNewName, 140, LV_PART_CURSOR | LV_STATE_DEFAULT);
  lv_obj_set_style_anim_time(ui_PresetNewName, 140, LV_PART_MAIN | LV_STATE_FOCUSED);
  lv_obj_set_style_anim_time(ui_PresetNewName, 140, LV_PART_CURSOR | LV_STATE_FOCUSED);
}

/////////////////////////////////////////////////////////// loop ///////////////////////////////////////////////////
// Core0 hot path: drain Input (Serial1) and Mainboard (Serial2) parsers.
void loop(void) {
  serial_read_n();
  serial_read_n2();

  // if (timer200msFlag) {
  //   Serial.print("|");
  // }
}

// --- Helpers for loop1() ---

// On signalFlag: load LVGL screens / show-hide panels for the new ScreenMode.
static void handleScreenModeChange(ScreenMode mode) {
  if (!signalFlag) {
    return;
  }

  switch (mode) {
    case ScreenMode::PresetScroll:
        draw_preset_scroll_1();
        break;

    case ScreenMode::LoadSaveExit:
        lv_scr_load(ui_Main);
        lv_obj_add_flag(ui_PresetSavePanel, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_PresetNewName, LV_OBJ_FLAG_HIDDEN);
        draw_preset_scroll_1();
      setScreenMode(ScreenMode::PresetScroll);
        break;

    case ScreenMode::SaveSelectPreset: {
      // SAVE MODE - Select destination preset
        char str[3];
        itoa(presetNumber, str, 10);
        lv_textarea_set_text(ui_PresetNewName, (const char *)presetNameBytes);
        lv_label_set_text(ui_PresetNOLD, str);
        lv_label_set_text(ui_PresetNOLDShadow, str);
        lv_label_set_text(ui_PresetNameOLD, (const char *)presetNameBytes);
        lv_label_set_text(ui_PresetNameOLDShadow, (const char *)presetNameBytes);
        draw_preset_scroll_1();
        lv_obj_remove_flag(ui_PresetSavePanel, LV_OBJ_FLAG_HIDDEN);
        break;
    }

    case ScreenMode::SaveSetName:
      // SAVE MODE - set preset name
        presetChar = 0;
        lv_obj_remove_flag(ui_PresetNewName, LV_OBJ_FLAG_HIDDEN);
        lv_textarea_set_cursor_pos(ui_PresetNewName, presetChar);
        lv_textarea_set_cursor_pos(ui_PresetNewName, presetChar);
        break;

    case ScreenMode::SaveCompleted:
      // PRESET SAVED
        lv_obj_add_flag(ui_PresetNewName, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_PresetSavePanel, LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_flag(ui_PresetSavedMesage, LV_OBJ_FLAG_HIDDEN);
        paramChangeTimerFlag = true;
        paramChangeLastMillis = millis();
      setScreenMode(ScreenMode::PresetScroll);
        draw_preset_scroll_1();
        break;

    case ScreenMode::Silent:
      // SCREEN SILENCE - no immediate action
        break;

    case ScreenMode::CalibrationMenu:
      // CALIBRATION MENU
        lv_obj_add_flag(ui_manualCalibrationPanel, LV_OBJ_FLAG_HIDDEN);
        lv_scr_load(ui_MANUALCALIBRATION);
        lv_obj_add_flag(ui_manualCalibrationPanel, LV_OBJ_FLAG_HIDDEN);
        break;

    case ScreenMode::ManualCalibration:
      // MANUAL CALIBRATION
        lv_obj_remove_flag(ui_manualCalibrationPanel, LV_OBJ_FLAG_HIDDEN);
        lv_scr_load(ui_MANUALCALIBRATION);
        break;
    }

    signalFlag = false;
  }

// Modes ≤5: hide timed toasts; update preset scroll / param label / name cursor.
static void updateBottomMessageAndPresetUI(ScreenMode mode) {
  // Modes 0..5 share the same behavior.
  uint8_t rawSignal = serialSignal;
  if (rawSignal <= 5) {
      if (paramChangeTimerFlag) {
        if (millis() - paramChangeLastMillis > paramHideTimeMillis) {
          lv_obj_add_flag(ui_BottomMessagePanel, LV_OBJ_FLAG_HIDDEN);
          lv_obj_add_flag(ui_PresetSavedMesage, LV_OBJ_FLAG_HIDDEN);
          paramChangeTimerFlag = false;
        }
      }

      if (presetCharFlag) {
        lv_textarea_set_cursor_pos(ui_PresetNewName, presetChar);
      }

      if (presetScrollFlag || paramChangeFlag || signalFlag /*|| presetCharFlag*/) {
        if (presetScrollFlag) {
          lv_obj_add_flag(ui_BottomMessagePanel, LV_OBJ_FLAG_HIDDEN);
          draw_preset_scroll_1();
          presetScrollFlag = false;
        }
        if (paramChangeFlag) {
          draw_param_1();
          paramChangeFlag = false;
        }
      }
  }
}

// Push OSC1/OSC2/SUB level bars when levelBarFlag is set (all three in Silent).
static void updateLevelBars(ScreenMode mode) {
  if (levelBarFlag == 0) {
    return;
  }

  if (mode == ScreenMode::Silent) {
    // In silent mode, always update all three bars.
    lv_bar_set_value(ui_OSC1Level, OSC1Level, LV_ANIM_ON);
    lv_bar_set_value(ui_OSC2Level, OSC2Level, LV_ANIM_ON);
    lv_bar_set_value(ui_SUBLevel, SUBLevel, LV_ANIM_ON);
  } else {
        switch (levelBarFlag) {
          case 1:
            lv_bar_set_value(ui_OSC1Level, OSC1Level, LV_ANIM_ON);
            break;
          case 2:
            lv_bar_set_value(ui_OSC2Level, OSC2Level, LV_ANIM_ON);
            break;
          case 3:
            lv_bar_set_value(ui_SUBLevel, SUBLevel, LV_ANIM_ON);
            break;
          default:
            break;
        }
  }

        levelBarFlag = 0;
      }

// Refresh ADSR1/ADSR2 bar widgets when update flags are set from serial.
static void updateADSRBars() {
      if (updateADSR1Flag) {
        lv_bar_set_value(ui_ADSR1AttackBar, 0.03125f * ADSR1Attack, LV_ANIM_ON);
        lv_bar_set_value(ui_ADSR1DecayBar, 0.03125f * ADSR1Decay, LV_ANIM_ON);
        lv_bar_set_value(ui_ADSR1SustainBar, 0.03125f * ADSR1Sustain, LV_ANIM_ON);
        lv_bar_set_value(ui_ADSR1ReleaseBar, 0.03125f * ADSR1Release, LV_ANIM_ON);
        updateADSR1Flag = false;
      }
      if (updateADSR2Flag) {
        lv_bar_set_value(ui_ADSR2AttackBar, 0.03125f * ADSR2Attack, LV_ANIM_ON);
        lv_bar_set_value(ui_ADSR2DecayBar, 0.03125f * ADSR2Decay, LV_ANIM_ON);
        lv_bar_set_value(ui_ADSR2SustainBar, 0.03125f * ADSR2Sustain, LV_ANIM_ON);
        lv_bar_set_value(ui_ADSR2ReleaseBar, 0.03125f * ADSR2Release, LV_ANIM_ON);
        updateADSR2Flag = false;
      }
}

// Calibration menu tabs / manual calibration panel redraw for modes 7–8.
static void updateCalibrationUI(ScreenMode mode) {
  switch (mode) {
    case ScreenMode::CalibrationMenu:
      if (paramChangeFlag) {
        switch (paramNumber) {
          case 190:
            lv_tabview_set_active(ui_calibrationTabs, paramValue, LV_ANIM_ON);
            break;
          default:
            break;
        }
      }
      paramChangeFlag = false;
      break;

    case ScreenMode::ManualCalibration:
      // Always redraw the manual calibration panel so the displayed
      // oscillator index and offset track the latest state even if a
      // paramChangeFlag edge is missed or arrives slightly out of order.
      drawManualCalibration();
      paramChangeFlag = false;
      break;

    default:
      // No calibration-related UI updates in other modes.
      break;
  }
}

// Core1 hot path: apply serial-driven UI updates, then LVGL timer handler.
void loop1(void) {
  millisTimer();

  ScreenMode mode = getScreenMode();

  handleScreenModeChange(mode);
  updateBottomMessageAndPresetUI(mode);
  updateLevelBars(mode);
  updateADSRBars();
  updateCalibrationUI(mode);

  lv_timer_handler();
}
