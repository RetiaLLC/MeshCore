#pragma once

#include "DisplayDriver.h"
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <helpers/RefCountedDigitalPin.h>

class ILI9341LCDDisplay : public DisplayDriver {
  Adafruit_ILI9341 display;
  bool _isOn;
  uint16_t _color;
  RefCountedDigitalPin* _peripher_power;

  bool i2c_probe(TwoWire& wire, uint8_t addr);
public:
  // shares the global Arduino SPI bus (same host/lock as the radio's SPIClass);
  // NOTE: Adafruit_ILI9341 ctor arg order is (spi, DC, CS, RST)
  ILI9341LCDDisplay(RefCountedDigitalPin* peripher_power=NULL) : DisplayDriver(128, 64),
      display(&SPI, PIN_TFT_DC, PIN_TFT_CS, PIN_TFT_RST),
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
