#include "ILI9341LCDDisplay.h"

#ifdef ESP32
  #include <esp_heap_caps.h>
#endif

#ifndef DISPLAY_ROTATION
  #define DISPLAY_ROTATION 3
#endif

#ifndef DISPLAY_SCALE_X
  #define DISPLAY_SCALE_X 2.5f // 320 / 128
#endif

#ifndef DISPLAY_SCALE_Y
  #define DISPLAY_SCALE_Y 3.75f // 240 / 64
#endif

// ---------------- ILI9341Canvas16 ----------------

bool ILI9341Canvas16::alloc() {
  if (_buf) return true;
  size_t sz = (size_t)WIDTH * HEIGHT * sizeof(uint16_t);
#ifdef ESP32
  _buf = (uint16_t *) heap_caps_malloc(sz, MALLOC_CAP_SPIRAM);
  if (!_buf) _buf = (uint16_t *) malloc(sz);   // no/full PSRAM: try internal heap
#else
  _buf = (uint16_t *) malloc(sz);
#endif
  return _buf != NULL;
}

void ILI9341Canvas16::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if (!_buf || x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT) return;
  _buf[(int32_t)y * WIDTH + x] = color;
}

void ILI9341Canvas16::fillScreen(uint16_t color) {
  if (!_buf) return;
  int32_t n = (int32_t)WIDTH * HEIGHT;
  for (int32_t i = 0; i < n; i++) _buf[i] = color;
}

void ILI9341Canvas16::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  if (!_buf) return;
  if (x < 0) { w += x; x = 0; }
  if (y < 0) { h += y; y = 0; }
  if (x + w > WIDTH)  w = WIDTH - x;
  if (y + h > HEIGHT) h = HEIGHT - y;
  for (int16_t j = 0; j < h; j++) {
    uint16_t* row = &_buf[(int32_t)(y + j) * WIDTH + x];
    for (int16_t i = 0; i < w; i++) row[i] = color;
  }
}

// ---------------- ILI9341LCDDisplay ----------------

bool ILI9341LCDDisplay::i2c_probe(TwoWire& wire, uint8_t addr) {
  return true;
}

bool ILI9341LCDDisplay::begin() {
  if (!_isOn) {
    if (_peripher_power) _peripher_power->claim();

    if (PIN_TFT_LEDA_CTL != -1) {
      pinMode(PIN_TFT_LEDA_CTL, OUTPUT);
      digitalWrite(PIN_TFT_LEDA_CTL, HIGH);
    }

    display.begin();
    display.setRotation(DISPLAY_ROTATION);

    display.setSPISpeed(40e6);

    canvas.alloc();   // falls back to direct panel drawing when out of memory

    display.fillScreen(ILI9341_BLACK);
    gfx().setTextColor(ILI9341_WHITE);
    gfx().setTextSize(2 * DISPLAY_SCALE_X);
    gfx().cp437(true); // Use full 256 char 'Code Page 437' font

    _isOn = true;
  }

  return true;
}

void ILI9341LCDDisplay::turnOn() {
  ILI9341LCDDisplay::begin();
}

void ILI9341LCDDisplay::turnOff() {
  if (_isOn) {
    if (PIN_TFT_LEDA_CTL != -1) {
      digitalWrite(PIN_TFT_LEDA_CTL, HIGH);
    }
    if (PIN_TFT_RST != -1) {
      digitalWrite(PIN_TFT_RST, LOW);
    }
    if (PIN_TFT_LEDA_CTL != -1) {
      digitalWrite(PIN_TFT_LEDA_CTL, LOW);
    }
    _isOn = false;

    if (_peripher_power) _peripher_power->release();
  }
}

void ILI9341LCDDisplay::clear() {
  gfx().fillScreen(ILI9341_BLACK);
  if (canvas.getBuffer()) endFrame();
}

void ILI9341LCDDisplay::startFrame(Color bkg) {
  gfx().fillScreen(ILI9341_BLACK);
  gfx().setTextColor(ILI9341_WHITE);
  gfx().setTextSize(1 * DISPLAY_SCALE_X); // This one affects size of Please wait... message
  gfx().cp437(true); // Use full 256 char 'Code Page 437' font
}

void ILI9341LCDDisplay::setTextSize(int sz) {
  gfx().setTextSize(sz * DISPLAY_SCALE_X);
}

void ILI9341LCDDisplay::setColor(Color c) {
  switch (c) {
    case DisplayDriver::DARK :
      _color = ILI9341_BLACK;
      break;
    case DisplayDriver::LIGHT :
      _color = ILI9341_WHITE;
      break;
    case DisplayDriver::RED :
      _color = ILI9341_RED;
      break;
    case DisplayDriver::GREEN :
      _color = ILI9341_GREEN;
      break;
    case DisplayDriver::BLUE :
      _color = ILI9341_BLUE;
      break;
    case DisplayDriver::YELLOW :
      _color = ILI9341_YELLOW;
      break;
    case DisplayDriver::ORANGE :
      _color = ILI9341_ORANGE;
      break;
    default:
      _color = ILI9341_WHITE;
      break;
  }
  gfx().setTextColor(_color);
}

void ILI9341LCDDisplay::setCursor(int x, int y) {
  gfx().setCursor(x * DISPLAY_SCALE_X, y * DISPLAY_SCALE_Y);
}

void ILI9341LCDDisplay::print(const char* str) {
  gfx().print(str);
}

void ILI9341LCDDisplay::fillRect(int x, int y, int w, int h) {
  gfx().fillRect(x * DISPLAY_SCALE_X, y * DISPLAY_SCALE_Y, w * DISPLAY_SCALE_X, h * DISPLAY_SCALE_Y, _color);
}

void ILI9341LCDDisplay::drawRect(int x, int y, int w, int h) {
  gfx().drawRect(x * DISPLAY_SCALE_X, y * DISPLAY_SCALE_Y, w * DISPLAY_SCALE_X, h * DISPLAY_SCALE_Y, _color);
}

void ILI9341LCDDisplay::drawXbm(int x, int y, const uint8_t* bits, int w, int h) {
  uint8_t byteWidth = (w + 7) / 8;

  for (int j = 0; j < h; j++) {
    for (int i = 0; i < w; i++) {
      uint8_t byte = bits[j * byteWidth + i / 8];
      bool pixelOn = byte & (0x80 >> (i & 7));

      if (pixelOn) {
        for (int dy = 0; dy < DISPLAY_SCALE_X; dy++) {
          for (int dx = 0; dx < DISPLAY_SCALE_X; dx++) {
            gfx().drawPixel(x * DISPLAY_SCALE_X + i * DISPLAY_SCALE_X + dx, y * DISPLAY_SCALE_Y + j * DISPLAY_SCALE_X + dy, _color);
          }
        }
      }
    }
  }
}

uint16_t ILI9341LCDDisplay::getTextWidth(const char* str) {
  int16_t x1, y1;
  uint16_t w, h;
  gfx().getTextBounds(str, 0, 0, &x1, &y1, &w, &h);

  return w / DISPLAY_SCALE_X;
}

void ILI9341LCDDisplay::endFrame() {
  if (canvas.getBuffer()) {
    display.drawRGBBitmap(0, 0, canvas.getBuffer(), canvas.width(), canvas.height());
  }
}
