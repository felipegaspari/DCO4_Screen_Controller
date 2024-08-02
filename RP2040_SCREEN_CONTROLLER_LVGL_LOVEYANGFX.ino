


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

static const uint16_t screenWidth = 320;
static const uint16_t screenHeight = 240;

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

/* Display flushing */
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

/*Read the touchpad*/
void my_touchpad_read(lv_indev_t *indev_driver, lv_indev_data_t *data) {
}

/*Set tick routine needed for LVGL internal timings*/
static uint32_t my_tick_get_cb(void) {
  return millis();
}



uint32_t paramChangeLastMillis = 0;
/////////////////////////////////////////////////////////// setup ///////////////////////////////////////////////////
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
}


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
void loop(void) {
  serial_read_n();
  serial_read_n2();

  // if (timer200msFlag) {
  //   Serial.print("|");
  // }
}


void loop1(void) {

  millisTimer();


  if (signalFlag) {
    switch (serialSignal) {
      case 1:
        draw_preset_scroll_1();
        break;
      case 2:
        lv_scr_load(ui_Main);
        lv_obj_add_flag(ui_PresetSavePanel, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_PresetNewName, LV_OBJ_FLAG_HIDDEN);
        draw_preset_scroll_1();
        serialSignal = 1;
        break;
      case 3:  // SAVE MODE - Select destination preset

        //strcpy(presetNameBytesOLD, presetNameBytes);  // caused freeze on pi pico. ust be done on other core
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
      case 4:  // SAVE MODE - set preset name
        presetChar = 0;
        lv_obj_remove_flag(ui_PresetNewName, LV_OBJ_FLAG_HIDDEN);
        lv_textarea_set_cursor_pos(ui_PresetNewName, presetChar);
        lv_textarea_set_cursor_pos(ui_PresetNewName, presetChar);
        break;
      case 5:  // PRESET SAVED
        lv_obj_add_flag(ui_PresetNewName, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_PresetSavePanel, LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_flag(ui_PresetSavedMesage, LV_OBJ_FLAG_HIDDEN);
        paramChangeTimerFlag = true;
        paramChangeLastMillis = millis();
        serialSignal = 1;
        draw_preset_scroll_1();
        break;
      case 6:  // SCREEN SILENCE
        break;
      case 7:  // CALIBRATION MENU
        lv_obj_add_flag(ui_manualCalibrationPanel, LV_OBJ_FLAG_HIDDEN);
        lv_scr_load(ui_MANUALCALIBRATION);
        lv_obj_add_flag(ui_manualCalibrationPanel, LV_OBJ_FLAG_HIDDEN);
        break;
      case 8:  // MANUAL CALIBRATION
        lv_obj_remove_flag(ui_manualCalibrationPanel, LV_OBJ_FLAG_HIDDEN);
        lv_scr_load(ui_MANUALCALIBRATION);

        break;
    }
    signalFlag = false;
  }

  switch (serialSignal) {
    case 0 ... 5:
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

      if (levelBarFlag > 0) {
        switch (levelBarFlag) {
          case 1:
            lv_bar_set_value(ui_OSC1Level, paramValue, LV_ANIM_ON);
            break;
          case 2:
            lv_bar_set_value(ui_OSC2Level, paramValue, LV_ANIM_ON);
            break;
          case 3:
            lv_bar_set_value(ui_SUBLevel, paramValue, LV_ANIM_ON);
            break;
          default:
            break;
        }
        levelBarFlag = 0;
      }
      break;
    case 7:
      if (paramChangeFlag) {
        switch (paramNumber) {
          case 190:
            lv_tabview_set_active(ui_calibrationTabs, paramValue, LV_ANIM_ON);
        }
      }
      paramChangeFlag = false;
      break;
    case 8:
      if (paramChangeFlag) {
        drawManualCalibration();
      }
      paramChangeFlag = false;
      break;
  }


  lv_timer_handler();
}
