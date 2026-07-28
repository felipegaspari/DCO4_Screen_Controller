# Screen — hardware notes

Scoped to **`RP2040_SCREEN_CONTROLLER_LVGL_LOVEYANGFX`**.

---

## UART (authoritative: main `.ino`)

| Port | RX | TX | Baud | Peer |
|------|----|----|------|------|
| USB `Serial` | — | — | 1 000 000 | Debug |
| `Serial1` | 13 | 12 | 2 500 000 | Input |
| `Serial2` | 21 | 20 | 2 500 000 | Mainboard |

Both UARTs: polling mode, FIFO 512. Brought up in Core0 `setup()`.

---

## Display

| Item | Value |
|------|--------|
| Driver stack | **LovyanGFX** (`LGFX tft`) |
| Board config | External `#include <lgfx_user/LGFX_RP2040_FELA.hpp>` |
| LVGL resolution | 480 × 320 |
| Rotation | `tft.setRotation(3)` |
| Flush | Partial buffer (~1/10 screen); `my_disp_flush` → `pushPixels` |
| Touch | `my_touchpad_read` is **empty** (display-only) |

**Do not use `tft_setup.h` for pin truth** — it is a leftover TFT_eSPI / ILI9341 config and is **not included** by the live sketch.

---

## Unused / legacy in this folder

| Path | Status |
|------|--------|
| `fela_U8g2/` | Vendored U8g2 — **not referenced** by live sources |
| `src/felanew_U8g2/` | Second U8g2 copy — **unused** |
| `ui.ino` | Fully commented SquareLine/TFT_eSPI template |
| `tft_setup.h` | TFT_eSPI leftovers |
| `tusb_config.h` | TinyUSB MIDI config present; USB MIDI product strings commented in `setup` |

---

## External libraries (required to build)

| Library | Role |
|---------|------|
| `lvgl` | UI framework |
| `LovyanGFX` | Panel/SPI draw |
| SquareLine export (`ui.h`) | Screens/widgets (`ui_Main`, `ui_MANUALCALIBRATION`, bars, labels, …) |
