// Definitions for serial-related shared state.
// These were previously defined in Serial.h; they now live here with
// extern declarations in the header to avoid multiple-definition issues.
volatile byte    presetNumber       = 0;
String           presetNameString   = "Vacio";

volatile bool    presetScrollFlag   = false;

volatile byte    paramNumber        = 0;
volatile int32_t paramValue         = 0;
String           paramName;

volatile bool    paramChangeFlag    = false;

volatile bool    updateADSR1Flag    = false;
volatile bool    updateADSR2Flag    = false;

volatile bool    signalFlag         = false;
volatile byte    serialSignal       = 1;

volatile bool    presetCharFlag     = false;
volatile byte    presetChar         = 0;

volatile byte    levelBarFlag       = true;

// +1 for null terminator so LVGL/string APIs see a clean C-string.
volatile char    presetNameBytes[17];
volatile char    presetNameBytesOLD[17];

// Forward declaration from parameters.ino.
void updateParameters(byte paramNumberNavigation, int32_t paramValueNavigation);

// ---------------------------------------------------------------------------
// Screen controller serial parsers (Serial2: mainboard -> screen,
//                                   Serial1: input -> screen)
// ---------------------------------------------------------------------------

// Payload length constants for this board's links.
static const uint8_t SCREEN_SERIAL_LEN_PARAM_16          = 4;   // [id, hi, lo, finish]
static const uint8_t SCREEN_SERIAL_LEN_PARAM_8           = 3;   // [id, int8, finish]
static const uint8_t SCREEN_SERIAL_LEN_PARAM_32          = 6;   // [id, b0..b3, finish]
static const uint8_t SCREEN_SERIAL2_LEN_PRESET_SCROLL    = 17;  // [preset#, 16 chars]
static const uint8_t SCREEN_SERIAL1_LEN_PRESET_SCROLL    = 18;  // [preset#, 16 chars, finish]
static const uint8_t SCREEN_SERIAL_LEN_SIGNAL            = 1;   // [signal]
static const uint8_t SCREEN_SERIAL_LEN_CHAR_SELECT       = 1;   // [char index]
static const uint8_t SCREEN_SERIAL_LEN_ADSR_BLOCK        = 8;   // [A_hi, A_lo, D_hi, D_lo, S_hi, S_lo, R_hi, R_lo]
static const uint8_t SCREEN_SERIAL_LEN_PARAM_BYTE_TO_NAV = 3;   // 'y': [paramId, value, finish]

// ---------------------------
// Serial2 (mainboard -> screen)
// ---------------------------

// 'p' : PARAM 16-bit from mainboard.
static void screenSerial2_handle_param16(char, const uint8_t* payload, uint8_t len) {
  if (len != SCREEN_SERIAL_LEN_PARAM_16) {
    return;
  }
  ParamFrame frame;
  decode_param_p(payload, frame);
  paramNumber = frame.id;
  paramValue  = frame.value;

  setDisplayParam();
  paramChangeFlag = true;
}

// 'w' : PARAM 8-bit from mainboard.
static void screenSerial2_handle_param8(char, const uint8_t* payload, uint8_t len) {
  if (len != SCREEN_SERIAL_LEN_PARAM_8) {
    return;
  }
  ParamFrame frame;
  decode_param_w(payload, frame);
  paramNumber = frame.id;
  paramValue  = frame.value;

  setDisplayParam();
  paramChangeFlag = true;
}

// 'x' : PARAM 32-bit from mainboard.
static void screenSerial2_handle_param32(char, const uint8_t* payload, uint8_t len) {
  if (len != SCREEN_SERIAL_LEN_PARAM_32) {
    return;
  }
  ParamFrame frame;
  decode_param_x(payload, frame);
  paramNumber = frame.id;
  paramValue  = frame.value;

  setDisplayParam();
  paramChangeFlag = true;
}

// 'q' : preset scroll from mainboard.
// payload: [presetNumber, 16 chars]  (no explicit finish byte in this link)
static void screenSerial2_handle_preset_scroll(char, const uint8_t* payload, uint8_t len) {
  if (len != SCREEN_SERIAL2_LEN_PRESET_SCROLL) {
    return;
  }

  presetNumber = payload[0];
  for (int i = 0; i < 16; ++i) {
    uint8_t c = payload[i + 1];
    if (c < 32) {
      c = 32;
    }
    presetNameBytes[i] = (char)c;
  }
  presetNameBytes[16] = '\0';

  presetNameString = String((char*)presetNameBytes);
  presetScrollFlag = true;
}

// 's' : screen mode / signal from mainboard.
static void screenSerial2_handle_signal(char, const uint8_t* payload, uint8_t len) {
  if (len != SCREEN_SERIAL_LEN_SIGNAL) {
    return;
  }
  serialSignal = payload[0];
  signalFlag   = true;
  // Debug: log incoming signals on Serial2
  // Serial.print("Screen Serial2 signal: ");
  // Serial.println(serialSignal);
}

// 'c' : preset char index from mainboard.
static void screenSerial2_handle_char_select(char, const uint8_t* payload, uint8_t len) {
  if (len != SCREEN_SERIAL_LEN_CHAR_SELECT) {
    return;
  }
  presetChar     = payload[0];
  presetCharFlag = true;
}

// Command table and parser context for Serial2.
static const SerialCommandDef screenSerial2Commands[] = {
  { 'p', SCREEN_SERIAL_LEN_PARAM_16,       screenSerial2_handle_param16       },
  { 'w', SCREEN_SERIAL_LEN_PARAM_8,        screenSerial2_handle_param8        },
  { 'x', SCREEN_SERIAL_LEN_PARAM_32,       screenSerial2_handle_param32       },
  { 'q', SCREEN_SERIAL2_LEN_PRESET_SCROLL, screenSerial2_handle_preset_scroll },
  { 's', SCREEN_SERIAL_LEN_SIGNAL,         screenSerial2_handle_signal        },
  { 'c', SCREEN_SERIAL_LEN_CHAR_SELECT,    screenSerial2_handle_char_select   },
};

static SerialParserContext screenSerial2Parser = {
  SERIAL_WAIT_FOR_CMD,
  0,
  nullptr,
  {0},
  0,
  0,
  0
};

// Core0: non-blocking Mainboard Serial2 parser pump.
void serial_read_n2() {
  // Expire any stale partial frame.
  if (screenSerial2Parser.state == SERIAL_READ_PAYLOAD) {
    uint32_t now = micros();
    serial_parser_check_timeout(screenSerial2Parser, now);
  }

  if (Serial2.available() > 0) {
    uint32_t now = micros();  // one timestamp per batch is enough
    while (Serial2.available() > 0) {
      uint8_t b = Serial2.read();
      serial_parser_process_byte(
        screenSerial2Parser,
        screenSerial2Commands,
        sizeof(screenSerial2Commands) / sizeof(screenSerial2Commands[0]),
        b,
        now
      );
    }
  }
}

// ---------------------------
// Serial1 (input -> screen)
// ---------------------------

// 'a' : ADSR1 block (attack/decay/sustain/release) from input controller.
static void screenSerial1_handle_adsr1(char, const uint8_t* payload, uint8_t len) {
  if (len != SCREEN_SERIAL_LEN_ADSR_BLOCK) {
    return;
  }

  ADSR1Attack  = word(payload[0], payload[1]);
  ADSR1Decay   = word(payload[2], payload[3]);
  ADSR1Sustain = word(payload[4], payload[5]);
  ADSR1Release = word(payload[6], payload[7]);

  updateADSR1Flag = true;
}

// 'b' : ADSR2 block from input controller.
static void screenSerial1_handle_adsr2(char, const uint8_t* payload, uint8_t len) {
  if (len != SCREEN_SERIAL_LEN_ADSR_BLOCK) {
    return;
  }

  ADSR2Attack  = word(payload[0], payload[1]);
  ADSR2Decay   = word(payload[2], payload[3]);
  ADSR2Sustain = word(payload[4], payload[5]);
  ADSR2Release = word(payload[6], payload[7]);

  updateADSR2Flag = true;
}

// Shared helper for 'p'/'w'/'x' coming from input controller.
static inline void screenSerial1_apply_param_from_frame(const ParamFrame& frame) {
  paramNumber = frame.id;
  paramValue  = frame.value;

  setDisplayParam();

  if (serialSignal != 6) {
    paramChangeFlag = true;
  }
}

// 'p' : PARAM 16-bit from input controller.
static void screenSerial1_handle_param16(char, const uint8_t* payload, uint8_t len) {
  if (len != SCREEN_SERIAL_LEN_PARAM_16) {
    return;
  }
  ParamFrame frame;
  decode_param_p(payload, frame);
  screenSerial1_apply_param_from_frame(frame);
}

// 'w' : PARAM 8-bit from input controller.
static void screenSerial1_handle_param8(char, const uint8_t* payload, uint8_t len) {
  if (len != SCREEN_SERIAL_LEN_PARAM_8) {
    return;
  }
  ParamFrame frame;
  decode_param_w(payload, frame);
  // Ignore manual calibration stage/offset on the generic 'w' path.
  // The screen's manual calibration UI is driven solely by 'y' nav frames
  // from the input controller so stage and offset updates are tightly
  // coupled and under input's control.
  if (frame.id == static_cast<uint8_t>(ParamId::PARAM_MANUAL_CALIBRATION_STAGE) ||
      frame.id == static_cast<uint8_t>(ParamId::PARAM_MANUAL_CALIBRATION_OFFSET)) {
    return;
  }

  // Default case for other 8-bit params:
  // We want 0..255 display semantics for most 8‑bit parameters (levels,
  // menu flags, etc.). decode_param_w() interprets the payload as int8_t,
  // which would make values >127 appear negative. Reinterpret the underlying
  // 8-bit pattern as unsigned here and promote to int32_t so the UI sees 0..255.
  uint8_t raw = static_cast<uint8_t>(frame.value);
  frame.value = static_cast<int32_t>(raw);
  screenSerial1_apply_param_from_frame(frame);
}

// 'x' : PARAM 32-bit from input controller (reserved / rarely used).
static void screenSerial1_handle_param32(char, const uint8_t* payload, uint8_t len) {
  if (len != SCREEN_SERIAL_LEN_PARAM_32) {
    return;
  }
  ParamFrame frame;
  decode_param_x(payload, frame);
  screenSerial1_apply_param_from_frame(frame);
}

// 'y' : small navigation / calibration param from input controller.
// payload: [paramId, value (int8), finish]
static void screenSerial1_handle_param_nav_byte(char, const uint8_t* payload, uint8_t len) {
  if (len != SCREEN_SERIAL_LEN_PARAM_BYTE_TO_NAV) {
    return;
  }

  byte id    = payload[0];
  int8_t val = (int8_t)payload[1];
  // payload[2] is finishByte; we ignore its value here.

  paramNumber = id;
  paramValue  = (int16_t)val;

  updateParameters(paramNumber, (int32_t)paramValue);

  // For calibration-related 'y' updates coming from the input controller
  // (manual stage/offset, etc.), trigger a redraw of the calibration UI so
  // the on-screen values update immediately when changing oscillators or
  // tweaking the offset.
  if (paramNumber >= 150 && paramNumber <= 155) {
    paramChangeFlag = true;
  }
}

// 'q' : preset scroll from input controller.
// payload: [presetNumber, 16 chars, finishByte]
static void screenSerial1_handle_preset_scroll(char, const uint8_t* payload, uint8_t len) {
  if (len != SCREEN_SERIAL1_LEN_PRESET_SCROLL) {
    return;
  }

  presetNumber = payload[0];
  for (int i = 0; i < 16; ++i) {
    uint8_t c = payload[i + 1];
    if (c < 32) {
      c = 32;
    }
    presetNameBytes[i] = (char)c;
  }
  presetNameBytes[16] = '\0';

  presetNameString = String((char*)presetNameBytes);
  presetScrollFlag = true;
}

// 's' : screen mode / signal from input controller.
static void screenSerial1_handle_signal(char, const uint8_t* payload, uint8_t len) {
  if (len != SCREEN_SERIAL_LEN_SIGNAL) {
    return;
  }
  serialSignal = payload[0];
  signalFlag   = true;
  // Debug: log incoming signals on Serial1
  // Serial.print("Screen Serial1 signal: ");
  // Serial.println(serialSignal);
}

// 'c' : preset char index from input controller.
static void screenSerial1_handle_char_select(char, const uint8_t* payload, uint8_t len) {
  if (len != SCREEN_SERIAL_LEN_CHAR_SELECT) {
    return;
  }
  presetChar     = payload[0];
  presetCharFlag = true;
}

// Command table and parser context for Serial1.
static const SerialCommandDef screenSerial1Commands[] = {
  { 'a', SCREEN_SERIAL_LEN_ADSR_BLOCK,        screenSerial1_handle_adsr1          },
  { 'b', SCREEN_SERIAL_LEN_ADSR_BLOCK,        screenSerial1_handle_adsr2          },
  { 'p', SCREEN_SERIAL_LEN_PARAM_16,          screenSerial1_handle_param16        },
  { 'w', SCREEN_SERIAL_LEN_PARAM_8,           screenSerial1_handle_param8         },
  { 'x', SCREEN_SERIAL_LEN_PARAM_32,          screenSerial1_handle_param32        },
  { 'y', SCREEN_SERIAL_LEN_PARAM_BYTE_TO_NAV, screenSerial1_handle_param_nav_byte },
  { 'q', SCREEN_SERIAL1_LEN_PRESET_SCROLL,    screenSerial1_handle_preset_scroll  },
  { 's', SCREEN_SERIAL_LEN_SIGNAL,            screenSerial1_handle_signal         },
  { 'c', SCREEN_SERIAL_LEN_CHAR_SELECT,       screenSerial1_handle_char_select    },
};

static SerialParserContext screenSerial1Parser = {
  SERIAL_WAIT_FOR_CMD,
  0,
  nullptr,
  {0},
  0,
  0,
  0
};

// Core0: non-blocking Input Serial1 parser pump.
void serial_read_n() {
  // Expire any stale partial frame.
  if (screenSerial1Parser.state == SERIAL_READ_PAYLOAD) {
    uint32_t now = micros();
    serial_parser_check_timeout(screenSerial1Parser, now);
  }

  if (Serial1.available() > 0) {
    uint32_t now = micros();  // one timestamp per batch is enough
    while (Serial1.available() > 0) {
      uint8_t b = Serial1.read();
      serial_parser_process_byte(
        screenSerial1Parser,
        screenSerial1Commands,
        sizeof(screenSerial1Commands) / sizeof(screenSerial1Commands[0]),
        b,
        now
      );
    }
  }
}
