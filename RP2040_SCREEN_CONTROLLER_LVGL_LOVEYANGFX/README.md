## DCO4 – Screen Controller (LVGL + LovyanGFX)

Arduino sketch for the **DCO4 screen board**: RP2040 dual-core firmware that renders a **480×320 LVGL** UI via **LovyanGFX**, fed by high-speed UART from the **Input** and **Mainboard** controllers (params, presets, calibration, ADSR/level bars).

**This documentation covers only this folder:**  
`DCO4_Screen_Controller/RP2040_SCREEN_CONTROLLER_LVGL_LOVEYANGFX/`

System context: [`docs/SYSTEM_OVERVIEW.md`](docs/SYSTEM_OVERVIEW.md) → canonical overview in sibling **DCO4_DCO**.

---

## Documentation

| Doc | Status | Contents |
|-----|--------|----------|
| [`docs/SYSTEM_OVERVIEW.md`](docs/SYSTEM_OVERVIEW.md) | Current | Stub → four-board overview + local UART table |
| [`docs/UI_AND_SERIAL.md`](docs/UI_AND_SERIAL.md) | Current | ScreenMode 1–8, Serial1/2 cmds, Core0→Core1 flags |
| [`docs/HARDWARE.md`](docs/HARDWARE.md) | Current | UART pins, LovyanGFX deps, unused U8g2/TFT leftovers |
| [`docs/FILE_INDEX.md`](docs/FILE_INDEX.md) | Current | Project files + functions + call sites |
| [`docs/REFERENCE_AI.md`](docs/REFERENCE_AI.md) | Current | Semantic map for developers / AI |

---

## Features

- Dual-core: Core0 serial RX; Core1 LVGL render/update.
- Screens: main preset/param UI + manual calibration (SquareLine `ui.h`).
- Serial1 @ 2.5 Mbaud from Input (ADSR bars, params, `'y'` nav, 16-char preset names, mode signals).
- Serial2 @ 2.5 Mbaud from Mainboard (params, preset scroll variant, signals).
- Param toast labels, OSC/SUB level bars, ADSR bars, calibration stage/offset/gap.

**Not active here:** touch input; USB MIDI product identity (commented); vendored U8g2 trees; TFT_eSPI `tft_setup.h`.

---

## Architecture

| Layer | Location |
|-------|----------|
| Entry / dual-core | `RP2040_SCREEN_CONTROLLER_LVGL_LOVEYANGFX.ino` |
| Serial parsers | `Serial.ino`, `serial_*.h` |
| Param → UI model | `displayParams.*`, `parameters.ino` |
| Widgets | External `<ui.h>` (SquareLine) |
| Panel | External LovyanGFX + `LGFX_RP2040_FELA.hpp` |

Details: [`docs/UI_AND_SERIAL.md`](docs/UI_AND_SERIAL.md).

---

## Building

1. Open **this folder** as the Arduino sketch (`RP2040_SCREEN_CONTROLLER_LVGL_LOVEYANGFX.ino`).
2. Install / configure: **lvgl**, **LovyanGFX**, board user file **`lgfx_user/LGFX_RP2040_FELA.hpp`**, SquareLine export providing **`ui.h`**.
3. Board: RP2040 (Earle Philhower core or compatible).
4. Do not rely on `fela_U8g2/` or `tft_setup.h` for the live build.

### Flags (project)

| Define | Role |
|--------|------|
| `LGFX_USE_V1` | LovyanGFX v1 API |
| `LV_COLOR_16_SWAP 0` | No RGB565 swap in flush |

---

## Contributing / hacking

- Start with [`docs/REFERENCE_AI.md`](docs/REFERENCE_AI.md) and [`docs/FILE_INDEX.md`](docs/FILE_INDEX.md).
- Keep LVGL off Core0; keep UART off Core1.
- Align ParamIds with Mainboard; sync SquareLine widget names with draw helpers.
- Prefer documenting over deleting the unused U8g2 trees unless you intentionally clean the repo.
