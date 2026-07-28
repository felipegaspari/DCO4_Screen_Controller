# RP2040_SCREEN_CONTROLLER_LVGL_LOVEYANGFX File Index

Purpose of **every file**, and for each source function: **what it does**, **who calls it**, and **when**.

Scope: **sketch folder root** sources + `docs/`. Vendored `fela_U8g2/` and `src/felanew_U8g2/` are summarized only (unused/legacy).

- Deep narrative: [`REFERENCE_AI.md`](REFERENCE_AI.md)
- UI / serial protocol: [`UI_AND_SERIAL.md`](UI_AND_SERIAL.md)
- Display / pin map: [`HARDWARE.md`](HARDWARE.md)
- Four-board topology: [`SYSTEM_OVERVIEW.md`](SYSTEM_OVERVIEW.md) (stub → DCO4_DCO canonical)
- Repo entry / doc index: [`../README.md`](../README.md)

Headers with no bodies are marked **no function definitions**.  
**Dead** = no live callers. **Unreachable** = call site exists but cannot run as currently gated. **`#ifdef` gated** = compiled only when the flag is set. **commented-out** = body fully commented (not compiled).

MCU: **RP2040** (dual Arduino cores). Screen brain: LVGL + LovyanGFX UI; Serial1 ← Input, Serial2 ← Mainboard; `ScreenMode` 1–8 from `serialSignal`.

---

## Call-flow overview

```mermaid
flowchart TD
  fw0["Arduino Core0"] --> setup0["setup()"]
  fw0 --> loop0["loop()"]
  fw1["Arduino Core1"] --> setup1["setup1()"]
  fw1 --> loop1["loop1()"]

  setup0 --> uartUsb["Serial.begin 1M"]
  setup0 --> uart1["Serial1 RX13/TX12 @ 2.5M"]
  setup0 --> uart2["Serial2 RX21/TX20 @ 2.5M"]

  setup1 --> lvInit["lv_init"]
  setup1 --> tftBegin["tft.begin + setRotation"]
  setup1 --> disp["lv_display_create + my_disp_flush"]
  setup1 --> indev["lv_indev_create + my_touchpad_read"]
  setup1 --> tick["lv_tick_set_cb my_tick_get_cb"]
  setup1 --> uiInit["ui_init + PresetNewName anim"]

  loop0 --> rs1["serial_read_n() Serial1 Input"]
  loop0 --> rs2["serial_read_n2() Serial2 Mainboard"]

  loop1 --> millis1["millisTimer()"]
  loop1 --> mode["getScreenMode()"]
  loop1 --> hMode["handleScreenModeChange"]
  loop1 --> bottom["updateBottomMessageAndPresetUI"]
  loop1 --> levels["updateLevelBars"]
  loop1 --> adsrBars["updateADSRBars"]
  loop1 --> calUI["updateCalibrationUI"]
  loop1 --> lvh["lv_timer_handler"]

  rs1 --> handlers1["screenSerial1_handle_*"]
  rs2 --> handlers2["screenSerial2_handle_*"]
  handlers1 --> setDisp["setDisplayParam / updateParameters / flags"]
  handlers2 --> setDisp
  setDisp --> router["param_router_apply screenParamTable"]
  hMode --> drawP["draw_preset_scroll_1 / screens"]
  bottom --> drawParam["draw_param_1 / draw_preset_scroll_1"]
  calUI --> drawMan["drawManualCalibration"]
```

| Context tag | Meaning |
|-------------|---------|
| Framework | Arduino invokes `setup` / `loop` / `setup1` / `loop1` |
| Boot Core0 | Inside `setup()` once — UARTs only |
| Boot Core1 | Inside `setup1()` once — LVGL + LovyanGFX + SquareLine `ui_init` |
| Every `loop` | Core0 forever: Serial1 + Serial2 parsers |
| Every `loop1` | Core1 forever: mode/UI flags + `lv_timer_handler` |
| Soft timer | `millisTimer()` runs on Core1; **flags have no live readers** today (only commented `timer200msFlag` in `loop`) |
| Serial1 (Input) | Parser on Input→Screen (`serial_read_n`) — ADSR `'a'`/`'b'`, params, `'y'` nav, scroll/signal/char |
| Serial2 (Mainboard) | Parser on Mainboard→Screen (`serial_read_n2`) — params, scroll, signal, char |
| Param table | `screenParamTable[]` via `setDisplayParam` → `applyParamToModelAndSignals` (see catalogs under `displayParams.ino`) |
| `'y'` nav | `updateParameters()` only (stage/offset); not the full display-name switch |
| ScreenMode | `serialSignal` 1–8 → `enum class ScreenMode` |
| LVGL callback | Flush / tick / empty touch read registered in `setup1` |
| `#ifdef` | `LV_USE_LOG` (default **0** → `my_print` omitted) |

---

## 1. Entry / build / globals

### `RP2040_SCREEN_CONTROLLER_LVGL_LOVEYANGFX.ino`

Main sketch: dual-core split — Core0 UART RX, Core1 LVGL UI. Defines `ScreenMode` (1–8), display flush, and `loop1` UI orchestration.

**Functions**
- `getScreenMode()` — Cast `serialSignal` → `ScreenMode`.
  - **Called from:** `loop1()`.
  - **When:** Every `loop1`.
- `setScreenMode(ScreenMode)` — Write `serialSignal` from enum.
  - **Called from:** `handleScreenModeChange` (LoadSaveExit → PresetScroll; SaveCompleted → PresetScroll).
  - **When:** Mode-change handling on Core1.
- `my_print(const char*)` — USB Serial LVGL log sink.
  - **Called from:** **none live** — only compiled when `LV_USE_LOG != 0` (default **0** → **`#ifdef` gated off**).
- `my_disp_flush(...)` — Push LVGL dirty rect to LovyanGFX (`setAddrWindow` + `pushPixels`); `lv_disp_flush_ready`.
  - **Called from:** LVGL via `lv_display_set_flush_cb` (registered in `setup1`).
  - **When:** LVGL callback / render.
- `my_touchpad_read(...)` — Empty stub (no touch hardware wired).
  - **Called from:** LVGL via `lv_indev_set_read_cb` (registered in `setup1`).
  - **When:** LVGL input poll (no-op body).
- `my_tick_get_cb()` — Return `millis()` for LVGL tick.
  - **Called from:** LVGL via `lv_tick_set_cb` (registered in `setup1`).
  - **When:** LVGL internal timing.
- `setup()` — USB Serial @ 1 000 000; Serial1 RX13/TX12; Serial2 RX21/TX20; both UART peers @ 2 500 000, polling, FIFO 512.
  - **Called from:** Arduino framework (Core 0).
  - **When:** Boot Core0 once.
- `setup1()` — `lv_init`; LovyanGFX `tft.begin` / rotation 3; create partial buffer display + flush cb; pointer indev + touch cb; tick cb; `ui_init`; speed up `ui_PresetNewName` anim times.
  - **Called from:** Arduino framework (Core 1).
  - **When:** Boot Core1 once.
- `handleScreenModeChange(ScreenMode)` — On `signalFlag`, switch UI for modes 1–8 (load screens, show/hide panels, seed labels, clear flag).
  - **Called from:** `loop1()`.
  - **When:** Every `loop1` (early-out if `!signalFlag`).
- `updateBottomMessageAndPresetUI(ScreenMode)` — For raw `serialSignal <= 5`: hide bottom message after timeout; cursor on new-name; redraw scroll/param on flags.
  - **Called from:** `loop1()`.
  - **When:** Every `loop1`.
- `updateLevelBars(ScreenMode)` — Push OSC1/OSC2/SUB bar values when `levelBarFlag` set (all three in Silent).
  - **Called from:** `loop1()`.
  - **When:** Every `loop1`.
- `updateADSRBars()` — Push ADSR1/2 attack/decay/sustain/release bars when update flags set (scale `0.03125f *`).
  - **Called from:** `loop1()`.
  - **When:** Every `loop1`.
- `updateCalibrationUI(ScreenMode)` — Calibration menu tab (`paramNumber` 190) or always `drawManualCalibration` in ManualCalibration.
  - **Called from:** `loop1()`.
  - **When:** Every `loop1`.
- `loop()` — `serial_read_n()` + `serial_read_n2()`. Optional `|` print on `timer200msFlag` is **commented-out**.
  - **Called from:** Arduino framework (Core 0).
  - **When:** Forever.
- `loop1()` — `millisTimer()`; mode helpers above; `lv_timer_handler()`.
  - **Called from:** Arduino framework (Core 1).
  - **When:** Forever.

**`ScreenMode` ↔ `serialSignal`**

| Value | Enum | UI role |
|------:|------|---------|
| 1 | `PresetScroll` | LOAD (preset scroll) |
| 2 | `LoadSaveExit` | LOAD/SAVE EXIT → then forced back to 1 |
| 3 | `SaveSelectPreset` | SAVE destination select |
| 4 | `SaveSetName` | SAVE set name |
| 5 | `SaveCompleted` | PRESET SAVED toast → then forced to 1 |
| 6 | `Silent` | Screen silence (level bars still update) |
| 7 | `CalibrationMenu` | Calibration tabs screen |
| 8 | `ManualCalibration` | Manual calibration panel |

### `globals.h`

`blinkTitle1` bool. **Unused** (never read/written elsewhere). **No function definitions.**

### `tusb_config.h`

TinyUSB device configuration (MIT header). Sketch does **not** include TinyUSB / MIDI — config unused by current build. **No function definitions.**

---

## 2. Serial / parameters

### `Serial.h`

Externs for shared volatile UI/serial state (`presetNumber`, `paramNumber`/`paramValue`, ADSR/signal/char/level flags, `presetNameBytes`). Declares `serial_read_n()` only (`serial_read_n2` relies on Arduino auto-prototype from `Serial.ino`). Includes parser/protocol headers. Stale SIGNAL LIST comment in footer (superseded by `ScreenMode`). **No function definitions.**

### `Serial.ino`

State definitions + dual UART parsers (Serial2 = Mainboard→Screen, Serial1 = Input→Screen).

**Functions**
- `screenSerial2_handle_param16` — Decode `'p'` → `setDisplayParam` + `paramChangeFlag`.
  - **Called from:** Serial2 command table via `serial_parser_process_byte`.
  - **When:** Mainboard `'p'`.
- `screenSerial2_handle_param8` — Decode `'w'` → `setDisplayParam` + flag.
  - **Called from:** Serial2 parser.
  - **When:** Mainboard `'w'`.
- `screenSerial2_handle_param32` — Decode `'x'` → `setDisplayParam` + flag.
  - **Called from:** Serial2 parser.
  - **When:** Mainboard `'x'`.
- `screenSerial2_handle_preset_scroll` — `'q'`: preset # + 16 name chars → `presetScrollFlag`.
  - **Called from:** Serial2 parser.
  - **When:** Mainboard `'q'` (17-byte payload, no finish).
- `screenSerial2_handle_signal` — `'s'`: set `serialSignal` + `signalFlag`.
  - **Called from:** Serial2 parser.
  - **When:** Mainboard `'s'`.
- `screenSerial2_handle_char_select` — `'c'`: `presetChar` + `presetCharFlag`.
  - **Called from:** Serial2 parser.
  - **When:** Mainboard `'c'`.
- `serial_read_n2()` — Timeout + drain Serial2 into parser.
  - **Called from:** `loop()` every iteration.
  - **When:** Every `loop` (Core0).
- `screenSerial1_handle_adsr1` — `'a'`: load ADSR1 A/D/S/R words → `updateADSR1Flag`.
  - **Called from:** Serial1 parser.
  - **When:** Input `'a'`.
- `screenSerial1_handle_adsr2` — `'b'`: load ADSR2 → `updateADSR2Flag`.
  - **Called from:** Serial1 parser.
  - **When:** Input `'b'`.
- `screenSerial1_apply_param_from_frame` — Shared: `setDisplayParam`; set `paramChangeFlag` unless `serialSignal == 6` (Silent).
  - **Called from:** Serial1 `'p'`/`'w'`/`'x'` handlers.
  - **When:** Input param frames.
- `screenSerial1_handle_param16` — Decode `'p'` → apply helper.
  - **Called from:** Serial1 parser.
  - **When:** Input `'p'`.
- `screenSerial1_handle_param8` — Decode `'w'`; **ignore** manual cal stage/offset IDs (those use `'y'`); reinterpret value as unsigned 0..255 → apply.
  - **Called from:** Serial1 parser.
  - **When:** Input `'w'`.
- `screenSerial1_handle_param32` — Decode `'x'` → apply.
  - **Called from:** Serial1 parser.
  - **When:** Input `'x'`.
- `screenSerial1_handle_param_nav_byte` — `'y'`: id + int8 → `updateParameters`; set `paramChangeFlag` for ids 150..155.
  - **Called from:** Serial1 parser.
  - **When:** Input `'y'`.
- `screenSerial1_handle_preset_scroll` — `'q'`: preset # + 16 chars + finish → `presetScrollFlag`.
  - **Called from:** Serial1 parser.
  - **When:** Input `'q'` (18-byte payload).
- `screenSerial1_handle_signal` — `'s'`: `serialSignal` + `signalFlag`.
  - **Called from:** Serial1 parser.
  - **When:** Input `'s'`.
- `screenSerial1_handle_char_select` — `'c'`: char index flags.
  - **Called from:** Serial1 parser.
  - **When:** Input `'c'`.
- `serial_read_n()` — Timeout + drain Serial1 into parser.
  - **Called from:** `loop()` every iteration.
  - **When:** Every `loop` (Core0).

Command / length inventory for both links is tabulated below (built from `screenSerial*Commands[]` + `SCREEN_SERIAL_*` constants).

**Note:** `presetNameBytesOLD[17]` is defined/externed but **never read or written** → **Dead** storage.

#### Payload length constants

| Constant | Value | Layout |
|----------|------:|--------|
| `SCREEN_SERIAL_LEN_PARAM_16` | 4 | `[id, hi, lo, finish]` |
| `SCREEN_SERIAL_LEN_PARAM_8` | 3 | `[id, int8, finish]` |
| `SCREEN_SERIAL_LEN_PARAM_32` | 6 | `[id, b0..b3, finish]` |
| `SCREEN_SERIAL2_LEN_PRESET_SCROLL` | 17 | `[preset#, 16 chars]` (no finish) |
| `SCREEN_SERIAL1_LEN_PRESET_SCROLL` | 18 | `[preset#, 16 chars, finish]` |
| `SCREEN_SERIAL_LEN_SIGNAL` | 1 | `[signal]` |
| `SCREEN_SERIAL_LEN_CHAR_SELECT` | 1 | `[char index]` |
| `SCREEN_SERIAL_LEN_ADSR_BLOCK` | 8 | ADSR A/D/S/R words |
| `SCREEN_SERIAL_LEN_PARAM_BYTE_TO_NAV` | 3 | `'y'`: `[paramId, value, finish]` |

#### Serial2 command table (`screenSerial2Commands[]` — Mainboard → Screen)

| Cmd | Handler | Length const | Effect |
|-----|---------|--------------|--------|
| `'p'` | `screenSerial2_handle_param16` | `SCREEN_SERIAL_LEN_PARAM_16` | Decode → `setDisplayParam` + `paramChangeFlag` |
| `'w'` | `screenSerial2_handle_param8` | `SCREEN_SERIAL_LEN_PARAM_8` | Same |
| `'x'` | `screenSerial2_handle_param32` | `SCREEN_SERIAL_LEN_PARAM_32` | Same |
| `'q'` | `screenSerial2_handle_preset_scroll` | `SCREEN_SERIAL2_LEN_PRESET_SCROLL` | Preset # + 16-char name → `presetScrollFlag` |
| `'s'` | `screenSerial2_handle_signal` | `SCREEN_SERIAL_LEN_SIGNAL` | `serialSignal` + `signalFlag` |
| `'c'` | `screenSerial2_handle_char_select` | `SCREEN_SERIAL_LEN_CHAR_SELECT` | `presetChar` + `presetCharFlag` |

#### Serial1 command table (`screenSerial1Commands[]` — Input → Screen)

| Cmd | Handler | Length const | Effect |
|-----|---------|--------------|--------|
| `'a'` | `screenSerial1_handle_adsr1` | `SCREEN_SERIAL_LEN_ADSR_BLOCK` | ADSR1 words → `updateADSR1Flag` |
| `'b'` | `screenSerial1_handle_adsr2` | `SCREEN_SERIAL_LEN_ADSR_BLOCK` | ADSR2 words → `updateADSR2Flag` |
| `'p'` | `screenSerial1_handle_param16` | `SCREEN_SERIAL_LEN_PARAM_16` | → `screenSerial1_apply_param_from_frame` |
| `'w'` | `screenSerial1_handle_param8` | `SCREEN_SERIAL_LEN_PARAM_8` | Ignore ids 152/153; else reinterpret u8 → apply |
| `'x'` | `screenSerial1_handle_param32` | `SCREEN_SERIAL_LEN_PARAM_32` | → apply helper |
| `'y'` | `screenSerial1_handle_param_nav_byte` | `SCREEN_SERIAL_LEN_PARAM_BYTE_TO_NAV` | → `updateParameters`; flag for ids 150..155 |
| `'q'` | `screenSerial1_handle_preset_scroll` | `SCREEN_SERIAL1_LEN_PRESET_SCROLL` | Preset # + 16 chars + finish → `presetScrollFlag` |
| `'s'` | `screenSerial1_handle_signal` | `SCREEN_SERIAL_LEN_SIGNAL` | `serialSignal` + `signalFlag` |
| `'c'` | `screenSerial1_handle_char_select` | `SCREEN_SERIAL_LEN_CHAR_SELECT` | `presetChar` + `presetCharFlag` |

### `serial_parser.h`

Generic non-blocking frame parser (`SERIAL_MAX_PAYLOAD` 18 for screen `'q'` frames).

**Functions**
- `serial_parser_reset()` — Clear context to wait-for-cmd.
  - **Called from:** `serial_parser_check_timeout`; `serial_parser_process_byte`.
  - **When:** Timeout or frame complete.
- `serial_parser_find_cmd()` — Lookup command def.
  - **Called from:** `serial_parser_process_byte`.
  - **When:** First byte of frame.
- `serial_parser_check_timeout()` — Drop stale partial frames (5 ms).
  - **Called from:** `serial_read_n`; `serial_read_n2`.
  - **When:** Before draining UART if mid-payload.
- `serial_parser_process_byte()` — State machine; invoke `on_frame` when complete.
  - **Called from:** `serial_read_n`; `serial_read_n2`.
  - **When:** Each RX byte.

### `serial_param_protocol.h`

**Functions**
- `decode_i16_be()` — Big-endian int16.
  - **Called from:** `decode_param_p`.
- `decode_i32_le()` — Little-endian int32.
  - **Called from:** `decode_param_x`.
- `decode_param_p()` / `decode_param_w()` / `decode_param_x()` — Fill `ParamFrame`.
  - **Called from:** Serial handlers in `Serial.ino`.
  - **When:** Param frames `'p'`/`'w'`/`'x'`.

### `param_router.h`

**Functions**
- `param_router_apply<ValueT>()` — Linear search descriptor table; invoke matching `apply`.
  - **Called from:** `applyParamToModelAndSignals()`.
  - **When:** Any `setDisplayParam()` path.

### `params_def.h`

Canonical shared `enum ParamId` (numeric IDs must stay stable across MCUs). **No function definitions.**

### `parameters.ino`

Lightweight `'y'` nav handler (Input → Screen), separate from display-name mapping.

**Functions**
- `updateParameters(byte, int32_t)` — Apply `PARAM_MANUAL_CALIBRATION_STAGE` / `OFFSET` to local cal model.
  - **Called from:** `screenSerial1_handle_param_nav_byte`.
  - **When:** Input `'y'` frames.
  - Note: other ParamIds fall through `default` (no-op).

### `displayParams.h`

UI model globals: hide timeout, mixer levels, ADSR1/2 words, manual-cal stage/offset/gap. **No function definitions** (variables live in header).

### `displayParams.ino`

Param → model router + LVGL label/bar draw helpers + human-readable `paramName` switch.

**Functions**
- `apply_param_sqr1_level` / `sqr2_level` / `sub_level` — Set OSC/SUB level + `levelBarFlag` 1/2/3.
  - **Called from:** **param table only**.
  - **When:** Matching ParamId via `setDisplayParam`.
- `apply_param_calibration_flag` — `v==0` → signal 2; `v==1` → signal 7; set `signalFlag`.
  - **Called from:** param table.
  - **When:** `PARAM_CALIBRATION_FLAG`.
- `apply_param_manual_calibration_flag` — `v==1` → signal 8; `v==0` → signal 7; set `signalFlag`.
  - **Called from:** param table.
  - **When:** `PARAM_MANUAL_CALIBRATION_FLAG`.
- `apply_param_manual_calibration_stage` — Stage + derived `manualCalibrationOSCN`.
  - **Called from:** param table.
  - **When:** `PARAM_MANUAL_CALIBRATION_STAGE` (also mirrored by `'y'` → `updateParameters`).
- `apply_param_manual_calibration_offset` — Set `offset`.
  - **Called from:** param table.
  - **When:** `PARAM_MANUAL_CALIBRATION_OFFSET`.
- `apply_param_gap_from_dco` — Set `calibrationGap`.
  - **Called from:** param table.
  - **When:** `PARAM_GAP_FROM_DCO`.
- `apply_param_ui_calibration_dismiss` — If signal 7 → signal 2 + `signalFlag`.
  - **Called from:** param table.
  - **When:** `PARAM_UI_CALIBRATION_DISMISS`.
- `apply_param_ui_calibration_menu_mode` — Force signal 7 + `signalFlag`.
  - **Called from:** param table.
  - **When:** `PARAM_UI_CALIBRATION_MENU_MODE`.
- `applyParamToModelAndSignals()` — `param_router_apply` on `screenParamTable[]`.
  - **Called from:** `setDisplayParam()`.
  - **When:** Every display-param update.
- `draw_param_1()` — Show bottom message panel with `paramName` + value; start hide timer.
  - **Called from:** `updateBottomMessageAndPresetUI` when `paramChangeFlag`.
  - **When:** Core1 UI path (`serialSignal <= 5`).
- `draw_preset_scroll_1()` — Update preset number/name labels (or textarea char) from `serialSignal` cases 1–4.
  - **Called from:** `handleScreenModeChange`; `updateBottomMessageAndPresetUI` when `presetScrollFlag`.
  - **When:** Mode change / scroll flag.
- `drawManualCalibration()` — Labels for offset, oscillator N, gap, waveform SAW/TRI/SQR.
  - **Called from:** `updateCalibrationUI` in `ManualCalibration`.
  - **When:** Every `loop1` while in mode 8.
- `setDisplayParam()` — Router apply, then ParamId → toast label switch (full catalog below).
  - **Called from:** Serial2 `'p'`/`'w'`/`'x'` handlers; Serial1 `screenSerial1_apply_param_from_frame`.
  - **When:** Param frames (not `'y'`).

#### `screenParamTable[]` (model / signal side effects)

| ParamId | Apply | Side effect |
|---------|-------|-------------|
| `PARAM_SQR1_LEVEL` (22) | `apply_param_sqr1_level` | `OSC1Level` + `levelBarFlag=1` |
| `PARAM_SQR2_LEVEL` (23) | `apply_param_sqr2_level` | `OSC2Level` + `levelBarFlag=2` |
| `PARAM_SUB_LEVEL` (24) | `apply_param_sub_level` | `SUBLevel` + `levelBarFlag=3` |
| `PARAM_CALIBRATION_FLAG` (150) | `apply_param_calibration_flag` | `v==0` → signal 2; `v==1` → signal 7; `signalFlag` |
| `PARAM_MANUAL_CALIBRATION_FLAG` (151) | `apply_param_manual_calibration_flag` | `v==1` → signal 8; `v==0` → signal 7; `signalFlag` |
| `PARAM_MANUAL_CALIBRATION_STAGE` (152) | `apply_param_manual_calibration_stage` | `manualCalibrationStage` + `manualCalibrationOSCN` |
| `PARAM_MANUAL_CALIBRATION_OFFSET` (153) | `apply_param_manual_calibration_offset` | `offset` |
| `PARAM_GAP_FROM_DCO` (154) | `apply_param_gap_from_dco` | `calibrationGap` |
| `PARAM_UI_CALIBRATION_DISMISS` (199) | `apply_param_ui_calibration_dismiss` | If signal 7 → signal 2 + `signalFlag` |
| `PARAM_UI_CALIBRATION_MENU_MODE` (200) | `apply_param_ui_calibration_menu_mode` | Force signal 7 + `signalFlag` |

#### `setDisplayParam()` ParamId catalog (toast labels)

| ID | ParamId | Display `paramName` / variants | Value remap | Table side-effect |
|---:|---------|--------------------------------|-------------|-------------------|
| 1 | `PARAM_SAW_STATUS` | `OSC1 SAW` + OFF/ON | — | — |
| 2 | `PARAM_SAW2_STATUS` | `OSC2 SAW` + OFF/ON | — | — |
| 3 | `PARAM_TRI_STATUS` | `OSC1 TRI` + OFF/ON | — | — |
| 4 | `PARAM_SINE_STATUS` | `OSC1 SIN` + OFF/ON | — | — |
| 5 | `PARAM_SQR1_STATUS` | `OSC1 SQR` + OFF/ON | — | — |
| 6 | `PARAM_SQR2_STATUS` | `OSC2 SQR` + OFF/ON | — | — |
| 7 | `PARAM_RESONANCE_COMPENSATION` | `ResoAmpComp` | — | — |
| 8 | `PARAM_VCA_ADSR_RESTART` | `ADSR1 Restart` | — | — |
| 9 | `PARAM_VCF_ADSR_RESTART` | `ADSR2 Restart` | — | — |
| 10 | `PARAM_ADSR3_TO_OSC_SELECT` | ADSR3 TO OSC1 / OSC2 / BOTH | — | — |
| 11 | `PARAM_LFO1_WAVEFORM` | `LFO1 Shape` | — | — |
| 12 | `PARAM_LFO2_WAVEFORM` | `LFO2 Shape` | — | — |
| 13 | `PARAM_OSC1_INTERVAL` | `Octave` | `(v-36)/12` | — |
| 14 | `PARAM_OSC2_INTERVAL` | `OSC2 Interval` | `v-=36` | — |
| 15 | `PARAM_OSC2_DETUNE_VAL` | `OSC2 Detune` | `v-=256` | — |
| 16 | `PARAM_LFO2_TO_DETUNE2` | `LFO2->OSC2 Pitch` | — | — |
| 17 | `PARAM_OSC_SYNC_MODE` | `OscPhaseSync` | — | — |
| 18 | `PARAM_PORTAMENTO_TIME` | `Portamento` | — | — |
| 19 | `PARAM_VCF_KEYTRACK` | `VCF Keytrack` | — | — |
| 20 | `PARAM_VELOCITY_TO_VCF` | `Velocity -> VCF` | — | — |
| 21 | `PARAM_VELOCITY_TO_VCA` | `Velocity -> VCA` | — | — |
| 22 | `PARAM_SQR1_LEVEL` | `OSC1 Level` | — | yes |
| 23 | `PARAM_SQR2_LEVEL` | `OSC2 Level` | — | yes |
| 24 | `PARAM_SUB_LEVEL` | `SUB Level` | — | yes |
| 25 | `PARAM_CALIBRATION_VALUE` | `CALIBRATION VAL` | — | — |
| 26 | `PARAM_VOICE_MODE` | MONO / POLY / UNISON | — | — |
| 27 | `PARAM_UNISON_DETUNE` | `Analog Detune` | — | — |
| 28 | `PARAM_ANALOG_DRIFT_AMOUNT` | `Analog Drift` | — | — |
| 29 | `PARAM_ANALOG_DRIFT_SPEED` | `Analog Drift Speed` | — | — |
| 30 | `PARAM_ANALOG_DRIFT_SPREAD` | `Analog Drift Spread` | — | — |
| 31 | `PARAM_SYNC_MODE` | `Sync Mode` | — | — |
| 40 | `PARAM_LFO1_TO_DCO` | `LFO1 -> Pitch` | — | — |
| 41 | `PARAM_LFO1_SPEED` | `LFO1 Speed` | — | — |
| 42 | `PARAM_LFO2_SPEED` | `LFO2 Speed` | — | — |
| 43 | `PARAM_VCA_LEVEL` | `VCA -> LEVEL` | — | — |
| 44 | `PARAM_LFO1_TO_VCA` | `LFO1 -> VCA` | — | — |
| 45 | `PARAM_LFO2_TO_PW` | `LFO2 -> PWM` | — | — |
| 46 | `PARAM_ADSR3_TO_PWM` | `ADSR3 -> PWM` | `v-=512` | — |
| 47 | `PARAM_ADSR3_TO_DETUNE1` | `ADSR3 -> Pitch` | — | — |
| 48 | `PARAM_ADSR1_ATTACK_CURVE` | curve names / `ADSR1 Curves` @100 | — | — |
| 49 | `PARAM_ADSR1_DECAY_CURVE` | curve names / `ADSR1 Decay` @100 | — | — |
| 50 | `PARAM_ADSR2_ATTACK_CURVE` | curve names / `ADSR2 Curves` @100 | — | — |
| 51 | `PARAM_ADSR2_DECAY_CURVE` | curve names / `ADSR2 Decay` @100 | — | — |
| 120 | `PARAM_FADERS_CONTROL_MANUAL` | `MAN FADERS` | — | — |
| 121 | `PARAM_FADER_ROW1_CONTROL_MANUAL` | `MAN FADERS 1` | — | — |
| 122 | `PARAM_FADER_ROW2_CONTROL_MANUAL` | `MAN FADERS 2` | — | — |
| 123 | `PARAM_VCF_POTS_CONTROL_MANUAL` | `MANUAL VCF` | — | — |
| 124 | `PARAM_PWM_POTS_CONTROL_MANUAL` | `MANUAL PWM` | — | — |
| 125 | `PARAM_ALL_CONTROLS_MANUAL` | `ALL CONTROLS MANUAL` | — | — |
| 126 | `PARAM_ADSR3_ENABLED` | `ADSR3 ENABLED` | — | — |
| 127 | `PARAM_FUNCTION_KEY` | `FUNCTION KEY` | — | — |
| 128 | `PARAM_VCA_POTS_CONTROL_MANUAL` | `MANUAL VCA` | — | — |
| 129 | `PARAM_POTS_CONTROL_MANUAL` | `MANUAL POTS` | — | — |
| 150 | `PARAM_CALIBRATION_FLAG` | `AUTO CALIBRATION` | — | yes |
| 151 | `PARAM_MANUAL_CALIBRATION_FLAG` | `MANUAL CALIBRATION` | — | yes |
| 152 | `PARAM_MANUAL_CALIBRATION_STAGE` | `OSCILLATOR N` | — | yes |
| 153 | `PARAM_MANUAL_CALIBRATION_OFFSET` | `OFFSET` | — | yes |
| 154 | `PARAM_GAP_FROM_DCO` | `GAP` | — | yes |
| 190 | `PARAM_UI_MENU_POSITION` | *(no toast label; empty case)* | — | — |
| 199 | `PARAM_UI_CALIBRATION_DISMISS` | *(no toast; side-effect only)* | — | yes |
| 200 | `PARAM_UI_CALIBRATION_MENU_MODE` | *(no toast; side-effect only)* | — | yes |
| 210 | `PARAM_PW_VALUE` | `PW` | — | — |
| 211 | `PARAM_LFO3_SPEED` | `LFO3 Speed` | — | — |
| 212 | `PARAM_LFO3_WAVEFORM` | `LFO3 Shape` | — | — |
| 214 | `PARAM_ADSR3_RESTART` | `ADSR3 Restart` | — | — |
| 215 | `PARAM_VCA_LEVEL_ALT` | `VCA -> LEVEL` | — | — |

**In `params_def.h` but no `setDisplayParam` case (no display label):**

| ID | ParamId | Notes |
|---:|---------|-------|
| 32 | `PARAM_PORTAMENTO_MODE` | Enum only; falls through `default` |
| 101 | `PARAM_CALIBRATION_MODE` | Enum only; falls through `default` |
| 155 | `PARAM_MANUAL_CALIBRATION_OFFSET_FROM_DCO` | No toast case; `'y'` may set `paramChangeFlag` (ids 150..155) but no label |

#### SquareLine `ui_*` widgets referenced from project `.ino`

| Widget | Used by |
|--------|---------|
| `ui_init` | `setup1` |
| `ui_Main` | `handleScreenModeChange` (LoadSaveExit) |
| `ui_MANUALCALIBRATION` | CalibrationMenu / ManualCalibration |
| `ui_PresetSavePanel` | mode change hide/show |
| `ui_PresetNewName` | setup1 anim; save name / cursor / scroll case 4 |
| `ui_PresetSavedMesage` | SaveCompleted; toast hide |
| `ui_PresetNOLD` / `ui_PresetNOLDShadow` | SaveSelectPreset |
| `ui_PresetNameOLD` / `ui_PresetNameOLDShadow` | SaveSelectPreset |
| `ui_BottomMessagePanel` | toast hide; `draw_param_1` show |
| `ui_CommandMessage` / `ui_CommandMessageShadow` | `draw_param_1` |
| `ui_CommandValue` / `ui_CommandValueShadow` | `draw_param_1` |
| `ui_PresetN` / `ui_PresetNShadow` | `draw_preset_scroll_1` cases 1–2 |
| `ui_PresetName` / `ui_PresetNameShadow` | `draw_preset_scroll_1` cases 1–2 |
| `ui_PresetNNew` / `ui_PresetNNewShadow` | `draw_preset_scroll_1` case 3 |
| `ui_PresetNameNew` / `ui_PresetNameNewShadow` | `draw_preset_scroll_1` case 3 |
| `ui_OSC1Level` / `ui_OSC2Level` / `ui_SUBLevel` | `updateLevelBars` |
| `ui_ADSR1AttackBar` / `ui_ADSR1DecayBar` / `ui_ADSR1SustainBar` / `ui_ADSR1ReleaseBar` | `updateADSRBars` |
| `ui_ADSR2AttackBar` / `ui_ADSR2DecayBar` / `ui_ADSR2SustainBar` / `ui_ADSR2ReleaseBar` | `updateADSRBars` |
| `ui_calibrationTabs` | `updateCalibrationUI` (param 190) |
| `ui_manualCalibrationPanel` | mode 7/8 show/hide |
| `ui_calibrationOffset` / `ui_calibrationOffsetShadow` | `drawManualCalibration` |
| `ui_oscillatorN` / `ui_oscillatorNShadow` | `drawManualCalibration` |
| `ui_calibrationGap` / `ui_calibrationGapShadow` | `drawManualCalibration` |
| `ui_waveform` / `ui_waveformShadow` | `drawManualCalibration` (SAW/TRI/SQR) |

---

## 3. Timing / utilities

### `timers_millis.h`

Soft-timer timestamps + flags (99 µs … 500 ms) and blink toggles. **No function definitions.**

### `timers_millis.ino`

**Functions**
- `millisTimer()` — Clear then set soft flags for 5/11/23/31/67/100/150/200 ms (µs timers and 500 ms are **commented-out**).
  - **Called from:** `loop1()` every iteration.
  - **When:** Every `loop1`.
  - Note: **no live readers** of these flags in project sources (only commented `timer200msFlag` in `loop`) → flags are effectively **Dead** consumers today.

### `auxiliary.h`

`blinkTimer*` / `blinker*Flag` globals. **No function definitions.**

### `auxiliary.ino`

**Functions**
- `blinker1()` — Toggle `blinker1Flag` every 70 ms.
  - **Called from:** **none (dead)**.
- `blinker2()` — Intended blinker; uses `blinkTimer1` threshold wrongly and writes `blinker2Flag = !blinker1Flag`.
  - **Called from:** **none (dead)**.

---

## 4. Dead / unused / legacy (sketch root)

### `ui.ino`

**Entirely commented-out** former TFT_eSPI + LVGL demo sketch (`setup`/`loop`, flush/touch/tick). No active definitions. Live equivalents live in `RP2040_SCREEN_CONTROLLER_LVGL_LOVEYANGFX.ino`.

### `tft_setup.h`

Legacy **TFT_eSPI** `User_Setup`-style pin/driver config (ILI9341, etc.). **Not `#include`d** by the live LovyanGFX sketch → unused by current build. **No function definitions.**

### `fela_U8g2/` — **unused/legacy vendored**

| Path | Status | Summary |
|------|--------|---------|
| `fela_U8g2/` | Unused / legacy | Vendored U8g2 tree (~106 files). Not included by the LVGL+LovyanGFX sketch. Do not inventory per-file. |

### `src/felanew_U8g2/` — **unused/legacy vendored**

| Path | Status | Summary |
|------|--------|---------|
| `src/felanew_U8g2/` | Unused / legacy | Second vendored U8g2 tree (~122 files). Not included by the live sketch. Do not inventory per-file. |

---

## 5. Documentation

All detailed docs live under `docs/` (this file included). Sketch/repo entry `README.md` is linked as `../README.md`.

| File | Status | Purpose |
|------|--------|---------|
| `README.md` (via `../README.md`) | Current | Overview / build / doc index. |
| `docs/SYSTEM_OVERVIEW.md` | Current | Stub pointing to DCO4_DCO canonical overview (+ local UART roles). |
| `docs/UI_AND_SERIAL.md` | Current | ScreenMode, UART frames, LVGL update path. |
| `docs/HARDWARE.md` | Current | Display / UART / pin map (LovyanGFX board config). |
| `docs/REFERENCE_AI.md` | Current | Deep semantic map. |
| `docs/FILE_INDEX.md` | Current | This file — files, functions, call sites. |

---

## 6. External libraries (not inventoriable here)

| Library / path | Used by |
|----------------|---------|
| `lvgl` | `setup1` / `loop1` / draw helpers |
| `LovyanGFX` | `LGFX tft` instance |
| `lgfx_user/LGFX_RP2040_FELA.hpp` | Board panel/bus pin config (under LovyanGFX) |
| `ui.h` (SquareLine `libraries/ui`) | Screens/widgets (`ui_Main`, bars, labels, …) |
| Arduino `Serial` / `Serial1` / `Serial2` | Core0 parsers + USB debug |

Unused **inside this sketch folder:** `fela_U8g2/`, `src/felanew_U8g2/`, `ui.ino`, `tft_setup.h` (see § dead/legacy above).

---

## Quick “where do I change X?”

| Goal | Start here |
|------|------------|
| Boot / dual-core split | `RP2040_SCREEN_CONTROLLER_LVGL_LOVEYANGFX.ino` (`setup` / `setup1` / `loop` / `loop1`) |
| UART pins / baud | `setup()` Serial1 RX13/TX12, Serial2 RX21/TX20 @ 2.5 M |
| ScreenMode / signal 1–8 | `ScreenMode` enum + `handleScreenModeChange`; RX `'s'` handlers in `Serial.ino` |
| Serial2 (Mainboard) RX commands | `screenSerial2Commands[]` table in this file + handlers in `Serial.ino` |
| Serial1 (Input) RX commands | `screenSerial1Commands[]` table in this file + handlers in `Serial.ino` |
| Param display names / remaps | `setDisplayParam()` catalog in this file; switch in `displayParams.ino` |
| Param → levels / cal / mode signals | `screenParamTable[]` map in this file; `apply_param_*` in `displayParams.ino` |
| `'y'` nav (stage/offset only) | `parameters.ino` `updateParameters` |
| New ParamId | `params_def.h` → display switch and/or `screenParamTable` apply |
| Preset scroll / name UI | `draw_preset_scroll_1` + `'q'`/`'c'` handlers |
| Mixer / ADSR bars | `updateLevelBars` / `updateADSRBars`; level applies + `'a'`/`'b'` |
| Manual calibration UI | `drawManualCalibration` + mode 7/8 in `handleScreenModeChange` |
| LVGL screens / widgets | `ui_*` inventory in this file; SquareLine `ui.h`; `ui_init` in `setup1` |
| Panel / SPI / resolution | `LGFX_RP2040_FELA.hpp` + `screenWidth`/`screenHeight` in main `.ino` |
| Soft timer rates | `timers_millis.ino` (flags unused until something reads them) |
| Legacy TFT_eSPI / U8g2 | `tft_setup.h`, `ui.ino`, `fela_U8g2/`, `src/felanew_U8g2/` — leave alone |
