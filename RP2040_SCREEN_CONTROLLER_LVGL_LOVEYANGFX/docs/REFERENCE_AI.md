# Screen Controller — Reference (AI / developers)

Semantic map for **`RP2040_SCREEN_CONTROLLER_LVGL_LOVEYANGFX`** only.

- System: [`SYSTEM_OVERVIEW.md`](SYSTEM_OVERVIEW.md) → DCO4_DCO canonical  
- UI/serial: [`UI_AND_SERIAL.md`](UI_AND_SERIAL.md)  
- Pins/deps: [`HARDWARE.md`](HARDWARE.md)  
- Call sites: [`FILE_INDEX.md`](FILE_INDEX.md)  
- Entry: [`../README.md`](../README.md)

---

## What this sketch owns

| Owns | Does not own |
|------|----------------|
| LVGL presentation of params, presets, cal UI | Voice engine / CV generation |
| UART RX from Input + Mainboard | Preset file storage (Input LittleFS) |
| ScreenMode UI state machine | Panel scanning |

Display-only: touch callback is empty.

---

## Runtime model

**Core 0:** open UARTs; forever `serial_read_n` (Serial1) + `serial_read_n2` (Serial2); set volatiles/flags.

**Core 1:** LVGL + LovyanGFX; consume flags; update widgets; `lv_timer_handler()`.

Do not call LVGL from Core0 or UART parsers from Core1.

---

## Modules

| File | Role |
|------|------|
| Main `.ino` | Dual-core entry, flush, ScreenMode helpers, UI update orchestration |
| `Serial.ino` | Two parsers + handlers |
| `displayParams.ino` | Param→label/model, draw helpers |
| `parameters.ino` | `'y'` nav apply |
| `serial_*.h`, `params_def.h`, `param_router.h` | Shared-style protocol |
| `timers_millis.*` | Soft timers (used lightly) |
| `auxiliary.*` | Blink helpers (**unused** in live loop) |

---

## External deps (edit carefully)

- **SquareLine `ui.h`** — regenerating overwrites widget names; keep draw helpers in sync.
- **`LGFX_RP2040_FELA.hpp`** — panel pins; outside this folder.
- **`params_def.h`** — keep ParamIds aligned with Mainboard; header guard text may still say “mainboard”.

---

## Dead / misleading

- `fela_U8g2/`, `src/felanew_U8g2/` — unused (~70 MB)  
- `ui.ino`, `tft_setup.h` — leftovers  
- Stale ParamId/signal comments at bottom of `Serial.h` / old lists — trust `ScreenMode` + `params_def.h`
