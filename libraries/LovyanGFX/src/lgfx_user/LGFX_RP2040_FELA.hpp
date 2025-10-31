#pragma once

#define LGFX_USE_V1

#include <LovyanGFX.hpp>

// LGFX for Waveshare RP2040-LCD-0.96
// https://www.waveshare.com/wiki/RP2040-LCD-0.96

class LGFX : public lgfx::LGFX_Device
{
  lgfx::Panel_ILI9488     _panel_instance;
  lgfx::Bus_SPI       _bus_instance;

  public:
  LGFX(void)
  {
    {
      auto cfg = _bus_instance.config();
      cfg.spi_host   = 0;
      cfg.spi_mode   = 0;
      cfg.freq_write = 80000000;
      cfg.pin_sclk   = 2;
      cfg.pin_miso   = 0;
      cfg.pin_mosi   = 3;
      cfg.pin_dc     = 18;
      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);
    }

    {
      auto cfg = _panel_instance.config();
      cfg.pin_cs       = 22;
      cfg.pin_rst      = 19;
      cfg.panel_width  = 320;
      cfg.panel_height = 480;
      cfg.offset_x     = 0;
      cfg.offset_y     = 0;
      cfg.invert       = false;
      cfg.rgb_order    = false;
      cfg.offset_rotation = 0;
      _panel_instance.config(cfg);
    }


    setPanel(&_panel_instance);
  }
};
