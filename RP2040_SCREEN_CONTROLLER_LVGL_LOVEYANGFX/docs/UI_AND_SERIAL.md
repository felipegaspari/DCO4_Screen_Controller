# Screen — UI modes and serial pipeline

How UART traffic becomes LVGL updates on this sketch.

---

## Dual-core split

```mermaid
flowchart TD
  subgraph core0 ["Core 0"]
    S0["setup: USB + Serial1/2"]
    L0["loop: serial_read_n + serial_read_n2"]
    L0 --> Flags["volatiles / flags"]
  end

  subgraph core1 ["Core 1"]
    S1["setup1: LVGL + LovyanGFX + ui_init"]
    L1["loop1"]
    L1 --> Mode["handleScreenModeChange"]
    L1 --> UI["preset / param / bars / cal"]
    L1 --> LV["lv_timer_handler"]
  end

  Flags --> L1
```

Core0 never touches LVGL. Core1 never reads UART.

---

## ScreenMode (`serialSignal` values)

| Value | Enum | UI behaviour |
|-------|------|----------------|
| 1 | `PresetScroll` | Preset number/name draw |
| 2 | `LoadSaveExit` | Load main screen; hide save panels; back to scroll |
| 3 | `SaveSelectPreset` | Show save panel; destination preset |
| 4 | `SaveSetName` | Show name textarea + cursor |
| 5 | `SaveCompleted` | “Saved” message; return to scroll |
| 6 | `Silent` | Suppress normal bottom/param UI; still update level bars |
| 7 | `CalibrationMenu` | Load calibration screen; tabs |
| 8 | `ManualCalibration` | Show manual cal panel |

Driven by `'s'` frames from Input (and Mainboard path on Serial2). Stale comment block in `Serial.h` is **outdated** — trust `ScreenMode` in the main `.ino`.

---

## Serial1 (Input → Screen)

| Cmd | Role |
|-----|------|
| `'a'` / `'b'` | ADSR1/2 raw blocks → bar model + flags |
| `'p'` / `'w'` / `'x'` | ParamId → `setDisplayParam` / levels / cal |
| `'y'` | Nav byte → `updateParameters` (cal stage/offset) |
| `'q'` | Preset scroll: number + **16** chars + finish (18-byte payload) |
| `'s'` | Mode signal → `serialSignal` + `signalFlag` |
| `'c'` | Char index for name edit |

## Serial2 (Mainboard → Screen)

| Cmd | Role |
|-----|------|
| `'p'` / `'w'` / `'x'` | Params / gap / cal-related |
| `'q'` | Preset scroll **17** bytes (no finish byte — different from Serial1) |
| `'s'` / `'c'` | Signal / char select |

Exact lengths: constants at top of `Serial.ino`.

---

## UI update helpers (`loop1`)

| Helper | When |
|--------|------|
| `handleScreenModeChange` | `signalFlag` — load screens / show-hide panels |
| `updateBottomMessageAndPresetUI` | Modes ≤5 — preset scroll, param toast, cursor |
| `updateLevelBars` | `levelBarFlag` — OSC1/OSC2/SUB bars |
| `updateADSRBars` | ADSR1/2 flags — envelope bars |
| `updateCalibrationUI` | Modes 7–8 — tabs / `drawManualCalibration` |

Widgets come from external SquareLine `ui.h` (`ui_Main`, `ui_MANUALCALIBRATION`, bars, labels, textarea, …).

---

## Param display path

1. Serial decode → `paramNumber` / `paramValue` / flags  
2. `setDisplayParam()` maps ParamId → label string + `applyParamToModelAndSignals()` (levels, cal flags)  
3. Core1 `draw_param_1()` / bars / cal redraw  

`parameters.ino` `updateParameters` handles Input `'y'` navigation (manual cal stage/offset) separately from the main `'p'/'w'/'x'` path.
