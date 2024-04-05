 // TFT_eSPI library demo, principally for STM32F processors with DMA:
// https://en.wikipedia.org/wiki/Direct_memory_access

// Tested with ESP32, Nucleo 64 STM32F446RE and Nucleo 144 STM32F767ZI
// TFT's with SPI can use DMA, the sketch also works with 8 bit
// parallel TFT's (tested with ILI9341 and ILI9481)

// The sketch will run on processors without DMA and also parallel
// interface TFT's. Comment out line 29 for no DMA.

// NOTE: FOR SPI DISPLAYS ONLY
//#define USE_DMA_TO_TFT

// Color depth has to be 16 bits if DMA is used to render image
#define COLOR_DEPTH 16

// 128x128 for a 16 bit colour Sprite (32Kbytes RAM)
// Maximum is 181x181 (64Kbytes) for DMA -  restricted by processor design
#define TITLE_WIDTH 320
#define TITLE_HEIGHT 30

#define BOTTOM_WIDTH 320
#define BOTTOM_HEIGHT 30

#define IWIDTH 320
#define IHEIGHT 180


#include "Arduino.h"
//#include "User_Setup_Select.h"
#include "TFT_eSPI.h"
#include "NotoSansBold15.h"
#include "NotoSansBold36.h"
#include "ep-boxi-24.h"
#include "software-tester-24.h"
#include "psilent-24.h"
#include "orbitron-semibold-48.h"

#define GFXFF 1

#define TESTER24 &software_tester_7_regular24pt8b
#define TESTER48 &software_tester_7_regular48pt8b
#define BOXI24 &ep_boxi_regular24pt8b
#define PSILENT24 &Psilent24pt7b
#define ORBITRON48 &Orbitron_SemiBold48pt8b

#include "globals.h"
#include "Serial.h"
#include "auxiliary.h"
#include "timers_millis.h"

// The font names are arrays references, thus must NOT be in quotes ""
#define AA_FONT_SMALL NotoSansBold15
#define AA_FONT_LARGE NotoSansBold36

// Library instance
TFT_eSPI tft = TFT_eSPI();  // Declare object "tft"

// Create two sprites for a DMA toggle buffer
TFT_eSprite spr[2] = {
  TFT_eSprite(&tft),
  TFT_eSprite(&tft)
  //TFT_eSprite(&tft),
};

TFT_eSprite spr2 = TFT_eSprite(&tft);
//TFT_eSprite spr[2] = { TFT_eSprite(&tft), TFT_eSprite(&tft) };
// Toggle buffer selection
bool sprSel = 0;

// Pointers to start of Sprites in RAM
//uint16_t* sprPtr[3];
uint16_t* sprPtr[2];


uint16_t pixelColor(uint16_t x, uint16_t y) {
  return tft.readPixel(x, y);
}

#define ORBITRON24 &Orbitron_Light_24
#define ORBITRON32 &Orbitron_Light_32

#include "src/felanew_U8g2/U8g2lib.h"

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#define U8G2_USE_DYNAMIC_ALLOC
uint8_t draw_state = 0;
U8G2_SSD1309_128X64_NONAME0_F_2ND_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/16, /* dc=*/17, /* reset=*/U8X8_PIN_NONE);

uint32_t paramChangeLastMillis = 0;
/////////////////////////////////////////////////////////// setup ///////////////////////////////////////////////////
void setup() {
  //SPI.setClockDivider(SPI_CLOCK_DIV2);

  Serial.begin(115200);

    Serial2.setFIFOSize(256);
  Serial2.setPollingMode(true);
   Serial2.setRX(21);
   Serial2.setTX(20);
  Serial2.begin(2500000);

  tft.setRotation(1);

  tft.setCallback(pixelColor);  // Switch on color callback for anti-aliased fonts

  //spr[0].setCallback(pixelColor);
  // spr[1].setCallback(pixelColor);
  // spr[2].setCallback(pixelColor);

  if (tft.width() >= 320)
    tft.loadFont(AA_FONT_LARGE);
  else
    tft.loadFont(AA_FONT_SMALL);

  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.init();

  tft.fillScreen(TFT_BLACK);

  // Define cprite colour depth
  spr[0].setColorDepth(8);
  spr[1].setColorDepth(COLOR_DEPTH);
  spr2.setColorDepth(8);
  //  spr[3].setColorDepth(COLOR_DEPTH);

  sprPtr[0] = (uint16_t*)spr[0].createSprite(IWIDTH, IHEIGHT);
  sprPtr[1] = (uint16_t*)spr[1].createSprite(TITLE_WIDTH, TITLE_HEIGHT);
  //sprPtr[2] = (uint16_t*)spr[2].createSprite(BOTTOM_WIDTH, BOTTOM_HEIGHT);
  spr2.createSprite(BOTTOM_WIDTH, BOTTOM_HEIGHT);

  // Define text datum and text colour for Sprites
  spr[0].setTextColor(TFT_BLACK);
  spr[0].setTextDatum(BR_DATUM);
  spr[1].setTextColor(TFT_BLACK);
  spr[1].setTextDatum(MC_DATUM);
  spr2.setTextColor(TFT_BLACK);
  spr2.setTextDatum(BR_DATUM);

//#define USE_DMA_TO_TFT
#ifdef USE_DMA_TO_TFT
  // DMA - should work with ESP32, STM32F2xx/F4xx/F7xx processors
  // NOTE: >>>>>> DMA IS FOR SPI DISPLAYS ONLY <<<<<<
  tft.initDMA();  // Initialise the DMA engine (tested with STM32F446 and STM32F767)
#endif

}

uint8_t *buf;

void setup1() {
  SPI1.setSCK(14);
  SPI1.setTX(15);

  u8g2.setBusClock(8000000);
  buf = (uint8_t *)malloc(u8g2.getBufferSize());
  u8g2.setBufferPtr(buf);
  u8g2.initDisplay();
  u8g2.clearDisplay();
  u8g2.setBusClock(8000000);
  //u8g2.begin();
  u8g2.setPowerSave(0);
}

/////////////////////////////////////////////////////////// loop ///////////////////////////////////////////////////
void loop(void) {

  int wait = 0;  //random (20);
  draw_title("  THIS IS MADNESS", TFT_BLACK, TFT_RED, 1);
  // Grab exclusive use of the SPI bus
  tft.startWrite();
  spr[0].setFreeFont(ORBITRON24);
  //spr[0].loadFont(AA_FONT_SMALL);    // Must load the font first
  spr[0].setTextColor(TFT_RED, TFT_BLACK);  // TFT_BLACK is used for anti-aliasing only

  spr[1].setFreeFont(ORBITRON24);           // Must load the font first
  spr[1].setTextColor(TFT_RED, TFT_BLACK);  // TFT_BLACK is used for anti-aliasing only

  spr2.setFreeFont(ORBITRON24);           // Must load the font first
  spr2.setTextColor(TFT_RED, TFT_BLACK);  // TFT_BLACK is used for anti-aliasing only

  //  spr[2].loadFont(AA_FONT_LARGE);    // Must load the font first
  //  spr[2].setTextColor(TFT_RED, TFT_BLACK); // TFT_BLACK is used for anti-aliasing only


  spr[0].drawRoundRect(0, -10, 320, 220, 10, 0xf9e2);

  spr2.drawRect(0, 0, 320, 30, TFT_RED);
  // spr[1].setFreeFont(ORBITRON24);
  // spr[1].setTextColor(TFT_BLACK);
  // spr[1].setTextDatum(TL_DATUM);
  // spr[1].drawString("  THIS IS MADNESS", 10, 2);

#ifdef USE_DMA_TO_TFT
  tft.pushImageDMA(0, 0, TITLE_WIDTH, TITLE_HEIGHT, sprPtr[1]);
  spr[0].pushSprite(0, 30);
  //tft.pushImageDMA(0, 30, IWIDTH, IHEIGHT, sprPtr[0]);
  //tft.pushImageDMA(0, 210, IWIDTH, IHEIGHT, sprPtr[2]);
  spr2.pushSprite(0, 210);  // Blocking write (no DMA) 115fps
#else
  spr[1].pushSprite(0, 0);       // Blocking write (no DMA) 115fps
  spr[0].pushSprite(0, 30);      // Blocking write (no DMA) 115fps
  spr2.pushSprite(0, 210);       // Blocking write (no DMA) 115fps
#endif
  //spr2.pushSprite(0, 210);       // Blocking write (no DMA) 115fps

  /*--------------------------------------------------------------------------------------------------------------------------------------------------*/
  /*--------------------------------------------------------------------------------------------------------------------------------------------------*/

  while (1) {

    millisTimer();

    serial_read_n();

    sprSel = 0;

    //spr2.fillSprite(TFT_BLACK);
    if (serialSignal == 4) {
      if (timer200msFlag == true) {
        if (timer200msBlink) {
          draw_title(" SAVE  PRESET ", 0xf9e2, TFT_BLACK, 1);
          //draw_title(" PRESET NAME", 0xf9e2, TFT_BLACK, 0);
          spr[1].pushSprite(0, 0);
        } else {
          draw_title(" SAVE  PRESET ", TFT_BLACK, 0xf9e2, 1);
          //draw_title(" PRESET NAME ", TFT_BLACK, 0xf9e2, 0);
          spr[1].pushSprite(0, 0);
        }
      }
      if (timer100msFlag == true) {
        draw_preset_scroll_1();
        spr[0].pushSprite(0, 30);
      }
    }

    if (presetScrollFlag || paramChangeFlag || signalFlag /*|| presetCharFlag*/) {
      if (presetScrollFlag) {
        if (serialSignal != 4) {
          draw_preset_scroll_1();
        }
      }
      if (paramChangeFlag) {
        draw_param_1();
        paramChangeFlag = false;
      }

      if (signalFlag) {
        switch (serialSignal) {
          case 1:
            draw_title(" LOAD  PRESET  ", TFT_BLACK, 0xf9e2, 1);
            spr0_baseline();
            draw_preset_scroll_1();
            //draw_title("LOAD  PRESET  ", TFT_BLACK, 0xf9e2, 0);
            break;
          case 2:
            draw_title("", TFT_BLACK, 0xf9e2, 1);
            break;
          case 3:
            draw_title(" SAVE  PRESET  ", TFT_BLACK, 0xf9e2, 1);
            spr0_baseline();
            draw_preset_scroll_1();
            break;
          case 4:
            draw_title(" PRESET NAME :", TFT_BLACK, 0xf9e2, 0);
            draw_preset_scroll_1();
            break;
          case 5:
            spr[0].fillSprite(TFT_BLACK);
            draw_title(" PRESET SAVED !!!", TFT_BLACK, 0xf9e2, 1);
            //draw_title("", TFT_BLACK, 0xf9e2, 1);
            spr0_baseline();
            draw_preset_scroll_1();
            break;
        }
        signalFlag = false;
      }

#ifdef USE_DMA_TO_TFT
      tft.pushImageDMA(0, 0, TITLE_WIDTH, TITLE_HEIGHT, sprPtr[1]);
      //tft.pushImageDMA(0, 30, IWIDTH, IHEIGHT, sprPtr[0]);
      spr[0].pushSprite(0, 30);  // Blocking write (no DMA) 115fps
      spr2.pushSprite(0, 210);   // Blocking write (no DMA) 115fps
#else
      spr[1].pushSprite(0, 0);   // Blocking write (no DMA) 115fps
      spr[0].pushSprite(0, 30);  // Blocking write (no DMA) 115fps
      spr2.pushSprite(0, 210);   // Blocking write (no DMA) 115fps
#endif
    }
    if (millis() - paramChangeLastMillis > 4000) {
      spr2.fillSprite(TFT_BLACK);
      spr2.drawRect(0, 0, 320, 30, TFT_RED);
      spr2.pushSprite(0, 210);  // Blocking write (no DMA) 115fps
    }
  }
}
volatile uint16_t i = 0;
void u8g2_prepare(void) {
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}

void u8g2_box_frame(uint8_t a) {
  u8g2.drawStr(0, 0, "drawBox");
  u8g2.drawBox(5, 10, 20, 10);
  u8g2.drawBox(10 + a, 15, 30, 7);
  u8g2.drawStr(0, 30, "drawFrame");
  u8g2.drawFrame(5, 10 + 30, 20, 10);
  u8g2.drawFrame(10 + a, 15 + 30, 30, 7);
}

void u8g2_disc_circle(uint8_t a) {
  u8g2.drawStr(0, 0, "drawDisc");
  u8g2.drawDisc(10, 18, 9);
  u8g2.drawDisc(24 + a, 16, 7);
  u8g2.drawStr(0, 30, "drawCircle");
  u8g2.drawCircle(10, 18 + 30, 9);
  u8g2.drawCircle(24 + a, 16 + 30, 7);
}

void u8g2_r_frame(uint8_t a) {
  u8g2.drawStr(0, 0, "drawRFrame/Box");
  u8g2.drawRFrame(5, 10, 40, 30, a + 1);
  u8g2.drawRBox(50, 10, 25, 40, a + 1);
}

void u8g2_string(uint8_t a) {
  u8g2.setFontDirection(0);
  u8g2.drawStr(30 + a, 31, " 0");
  u8g2.setFontDirection(1);
  u8g2.drawStr(30, 31 + a, " 90");
  u8g2.setFontDirection(2);
  u8g2.drawStr(30 - a, 31, " 180");
  u8g2.setFontDirection(3);
  u8g2.drawStr(30, 31 - a, " 270");
}

void u8g2_line(uint8_t a) {
  u8g2.drawStr(0, 0, "drawLine");
  u8g2.drawLine(7 + a, 10, 40, 55);
  u8g2.drawLine(7 + a * 2, 10, 60, 55);
  u8g2.drawLine(7 + a * 3, 10, 80, 55);
  u8g2.drawLine(7 + a * 4, 10, 100, 55);
}

void u8g2_triangle(uint8_t a) {
  uint16_t offset = a;
  u8g2.drawStr(0, 0, "drawTriangle");
  u8g2.drawTriangle(14, 7, 45, 30, 10, 40);
  u8g2.drawTriangle(14 + offset, 7 - offset, 45 + offset, 30 - offset, 57 + offset, 10 - offset);
  u8g2.drawTriangle(57 + offset * 2, 10, 45 + offset * 2, 30, 86 + offset * 2, 53);
  u8g2.drawTriangle(10 + offset, 40 + offset, 45 + offset, 30 + offset, 86 + offset, 53 + offset);
}

void u8g2_ascii_1() {
  char s[2] = " ";
  uint8_t x, y;
  u8g2.drawStr(0, 0, "ASCII page 1");
  for (y = 0; y < 6; y++) {
    for (x = 0; x < 16; x++) {
      s[0] = y * 16 + x + 32;
      u8g2.drawStr(x * 7, y * 10 + 10, s);
    }
  }
}

void u8g2_ascii_2() {
  char s[2] = " ";
  uint8_t x, y;
  u8g2.drawStr(0, 0, "ASCII page 2");
  for (y = 0; y < 6; y++) {
    for (x = 0; x < 16; x++) {
      s[0] = y * 16 + x + 160;
      u8g2.drawStr(x * 7, y * 10 + 10, s);
    }
  }
}

void u8g2_extra_page(uint8_t a) {
  u8g2.drawStr(0, 0, "Unicode");
  u8g2.setFont(u8g2_font_unifont_t_symbols);
  u8g2.setFontPosTop();
  u8g2.drawUTF8(0, 24, "☀ ☁");
  switch (a) {
    case 0:
    case 1:
    case 2:
    case 3:
      u8g2.drawUTF8(a * 3, 36, "☂");
      break;
    case 4:
    case 5:
    case 6:
    case 7:
      u8g2.drawUTF8(a * 3, 36, "☔");
      break;
  }
}

#define cross_width 24
#define cross_height 24
static const unsigned char cross_bits[] U8X8_PROGMEM = {
  0x00,
  0x18,
  0x00,
  0x00,
  0x24,
  0x00,
  0x00,
  0x24,
  0x00,
  0x00,
  0x42,
  0x00,
  0x00,
  0x42,
  0x00,
  0x00,
  0x42,
  0x00,
  0x00,
  0x81,
  0x00,
  0x00,
  0x81,
  0x00,
  0xC0,
  0x00,
  0x03,
  0x38,
  0x3C,
  0x1C,
  0x06,
  0x42,
  0x60,
  0x01,
  0x42,
  0x80,
  0x01,
  0x42,
  0x80,
  0x06,
  0x42,
  0x60,
  0x38,
  0x3C,
  0x1C,
  0xC0,
  0x00,
  0x03,
  0x00,
  0x81,
  0x00,
  0x00,
  0x81,
  0x00,
  0x00,
  0x42,
  0x00,
  0x00,
  0x42,
  0x00,
  0x00,
  0x42,
  0x00,
  0x00,
  0x24,
  0x00,
  0x00,
  0x24,
  0x00,
  0x00,
  0x18,
  0x00,
};

#define cross_fill_width 24
#define cross_fill_height 24
static const unsigned char cross_fill_bits[] U8X8_PROGMEM = {
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x18,
  0x00,
  0x18,
  0x64,
  0x00,
  0x26,
  0x84,
  0x00,
  0x21,
  0x08,
  0x81,
  0x10,
  0x08,
  0x42,
  0x10,
  0x10,
  0x3C,
  0x08,
  0x20,
  0x00,
  0x04,
  0x40,
  0x00,
  0x02,
  0x80,
  0x00,
  0x01,
  0x80,
  0x18,
  0x01,
  0x80,
  0x18,
  0x01,
  0x80,
  0x00,
  0x01,
  0x40,
  0x00,
  0x02,
  0x20,
  0x00,
  0x04,
  0x10,
  0x3C,
  0x08,
  0x08,
  0x42,
  0x10,
  0x08,
  0x81,
  0x10,
  0x84,
  0x00,
  0x21,
  0x64,
  0x00,
  0x26,
  0x18,
  0x00,
  0x18,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
};

#define cross_block_width 14
#define cross_block_height 14
static const unsigned char cross_block_bits[] U8X8_PROGMEM = {
  0xFF,
  0x3F,
  0x01,
  0x20,
  0x01,
  0x20,
  0x01,
  0x20,
  0x01,
  0x20,
  0x01,
  0x20,
  0xC1,
  0x20,
  0xC1,
  0x20,
  0x01,
  0x20,
  0x01,
  0x20,
  0x01,
  0x20,
  0x01,
  0x20,
  0x01,
  0x20,
  0xFF,
  0x3F,
};

void u8g2_bitmap_overlay(uint8_t a) {
  uint8_t frame_size = 28;

  u8g2.drawStr(0, 0, "Bitmap overlay");

  u8g2.drawStr(0, frame_size + 12, "Solid / transparent");
  u8g2.setBitmapMode(false /* solid */);
  u8g2.drawFrame(0, 10, frame_size, frame_size);
  u8g2.drawXBMP(2, 12, cross_width, cross_height, cross_bits);
  if (a & 4)
    u8g2.drawXBMP(7, 17, cross_block_width, cross_block_height, cross_block_bits);

  u8g2.setBitmapMode(true /* transparent*/);
  u8g2.drawFrame(frame_size + 5, 10, frame_size, frame_size);
  u8g2.drawXBMP(frame_size + 7, 12, cross_width, cross_height, cross_bits);
  if (a & 4)
    u8g2.drawXBMP(frame_size + 12, 17, cross_block_width, cross_block_height, cross_block_bits);
}

void u8g2_bitmap_modes(uint8_t transparent) {
  const uint8_t frame_size = 24;

  u8g2.drawBox(0, frame_size * 0.5, frame_size * 5, frame_size);
  u8g2.drawStr(frame_size * 0.5, 50, "Black");
  u8g2.drawStr(frame_size * 2, 50, "White");
  u8g2.drawStr(frame_size * 3.5, 50, "XOR");

  if (!transparent) {
    u8g2.setBitmapMode(false /* solid */);
    u8g2.drawStr(0, 0, "Solid bitmap");
  } else {
    u8g2.setBitmapMode(true /* transparent*/);
    u8g2.drawStr(0, 0, "Transparent bitmap");
  }
  u8g2.setDrawColor(0);  // Black
  u8g2.drawXBMP(frame_size * 0.5, 24, cross_width, cross_height, cross_bits);
  u8g2.setDrawColor(1);  // White
  u8g2.drawXBMP(frame_size * 2, 24, cross_width, cross_height, cross_bits);
  u8g2.setDrawColor(2);  // XOR
  u8g2.drawXBMP(frame_size * 3.5, 24, cross_width, cross_height, cross_bits);
}



void draw(void) {
  u8g2_prepare();
  switch (draw_state >> 3) {
    case 0: u8g2_box_frame(draw_state & 7); break;
    case 1: u8g2_disc_circle(draw_state & 7); break;
    case 2: u8g2_r_frame(draw_state & 7); break;
    case 3: u8g2_string(draw_state & 7); break;
    case 4: u8g2_line(draw_state & 7); break;
    case 5: u8g2_triangle(draw_state & 7); break;
    case 6: u8g2_ascii_1(); break;
    case 7: u8g2_ascii_2(); break;
    case 8: u8g2_extra_page(draw_state & 7); break;
    case 9: u8g2_bitmap_modes(0); break;
    case 10: u8g2_bitmap_modes(1); break;
    case 11: u8g2_bitmap_overlay(draw_state & 7); break;
  }
}
void loop1(void) {

    u8g2.clearBuffer();
  draw();
  u8g2.sendBuffer();
    draw_state++;
  if ( draw_state >= 12 * 8 )
    draw_state = 0;
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------------------------------*/

byte red = 31;
byte green = 0;
byte blue = 0;
byte state = 0;
unsigned int colour = red << 11;  // Colour order is RGB 5+6+5 bits each

void draw_title(String text, int text_color, int bg_color, byte spriteN) {
  switch (spriteN) {
    case 0:
      spr[0].fillSmoothRoundRect(0, 0, 320, 30, 10, bg_color);
      spr[0].fillRect(1, 21, 319, 10, bg_color);
      spr[0].setFreeFont(ORBITRON24);
      spr[0].setTextColor(text_color);
      spr[0].setTextDatum(TL_DATUM);
      spr[0].drawString(text, 10, 2);
      break;
    case 1:
      spr[1].fillSmoothRoundRect(0, 0, 320, 30, 10, bg_color);
      spr[1].fillRect(1, 21, 319, 10, bg_color);
      spr[1].setFreeFont(ORBITRON24);
      spr[1].setTextColor(text_color);
      spr[1].setTextDatum(TL_DATUM);
      spr[1].drawString(text, 10, 2);
      break;
    case 2:
      spr2.fillSmoothRoundRect(0, 0, 320, 30, 10, bg_color);
      spr2.fillRect(1, 21, 319, 10, bg_color);
      spr2.setFreeFont(ORBITRON24);
      spr2.setTextColor(text_color);
      spr2.setTextDatum(TL_DATUM);
      spr2.drawString(text, 10, 2);
      break;
  }
}

void spr0_baseline() {
  spr[0].drawRoundRect(0, -10, 320, 220, 10, 0xf9e2);
  //spr[0].drawRoundRect(0, -20, 320, 100, 10, 0xf9e2);
  spr[0].setTextColor(TFT_RED, TFT_BLACK);
}



void rainbow_fill() {
  // The colours and state are not initialised so the start colour changes each time the funtion is called

  for (int i = IHEIGHT; i >= 0; i--) {
    // Draw a vertical line 1 pixel wide in the selected colour
    spr[sprSel].drawFastHLine(0, i, IWIDTH, colour);  // in this example tft.width() returns the pixel width of the display
    // This is a "state machine" that ramps up/down the colour brightnesses in sequence
    switch (state) {
      case 0:
        green++;
        if (green == 64) {
          green = 63;
          state = 1;
        }
        break;
      case 1:
        red--;
        if (red == 255) {
          red = 0;
          state = 2;
        }
        break;
      case 2:
        blue++;
        if (blue == 32) {
          blue = 31;
          state = 3;
        }
        break;
      case 3:
        green--;
        if (green == 255) {
          green = 0;
          state = 4;
        }
        break;
      case 4:
        red++;
        if (red == 32) {
          red = 31;
          state = 5;
        }
        break;
      case 5:
        blue--;
        if (blue == 255) {
          blue = 0;
          state = 0;
        }
        break;
    }
    colour = red << 11 | green << 5 | blue;
  }
}
