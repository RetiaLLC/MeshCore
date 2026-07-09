#pragma once

#include "DisplayDriver.h"
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <helpers/RefCountedDigitalPin.h>

// Off-screen RGB565 canvas (buffer preferably in PSRAM). Frames are composed
// here and pushed to the panel in one blit from endFrame(), so the panel
// never shows a half-drawn frame (the direct-draw path visibly "pulses" on
// every refresh: full-screen clear followed by incremental redraw).
class ILI9341Canvas16 : public Adafruit_GFX {
  uint16_t* _buf;
public:
  ILI9341Canvas16(int16_t w, int16_t h) : Adafruit_GFX(w, h), _buf(NULL) {}
  bool alloc();
  uint16_t* getBuffer() const { return _buf; }
  void drawPixel(int16_t x, int16_t y, uint16_t color) override;
  void fillScreen(uint16_t color) override;
  void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) override;
};

class ILI9341LCDDisplay : public DisplayDriver {
  Adafruit_ILI9341 display;
  ILI9341Canvas16 canvas;
  bool _isOn;
  uint16_t _color;
  RefCountedDigitalPin* _peripher_power;

  bool i2c_probe(TwoWire& wire, uint8_t addr);
  Adafruit_GFX& gfx() { return canvas.getBuffer() ? (Adafruit_GFX&)canvas : (Adafruit_GFX&)display; }
public:
  // shares the global Arduino SPI bus (same host/lock as the radio's SPIClass);
  // NOTE: Adafruit_ILI9341 ctor arg order is (spi, DC, CS, RST)
  ILI9341LCDDisplay(RefCountedDigitalPin* peripher_power=NULL) : DisplayDriver(128, 64),
      display(&SPI, PIN_TFT_DC, PIN_TFT_CS, PIN_TFT_RST),
      canvas(320, 240),
      _peripher_power(peripher_power)
  {
    _isOn = false;
  }
  bool begin();

  bool isOn() override { return _isOn; }
  void turnOn() override;
  void turnOff() override;
  void clear() override;
  void startFrame(Color bkg = DARK) override;
  void setTextSize(int sz) override;
  void setColor(Color c) override;
  void setCursor(int x, int y) override;
  void print(const char* str) override;
  void fillRect(int x, int y, int w, int h) override;
  void drawRect(int x, int y, int w, int h) override;
  void drawXbm(int x, int y, const uint8_t* bits, int w, int h) override;
  uint16_t getTextWidth(const char* str) override;
  void endFrame() override;
};
