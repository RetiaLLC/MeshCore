#include "GenericNeoPixel.h"

#ifdef PIN_NEOPIXEL

#include <Adafruit_NeoPixel.h>

static Adafruit_NeoPixel _strip(NEOPIXEL_NUM, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

// Simple HSV->packed color (h: 0..255 hue), full sat/val; brightness is applied
// globally by the strip.
static uint32_t wheel(uint8_t pos) {
  pos = 255 - pos;
  if (pos < 85)  return _strip.Color(255 - pos * 3, 0, pos * 3);
  if (pos < 170) { pos -= 85; return _strip.Color(0, pos * 3, 255 - pos * 3); }
  pos -= 170;    return _strip.Color(pos * 3, 255 - pos * 3, 0);
}

void GenericNeoPixel::begin() {
  _strip.begin();
  _strip.setBrightness(NEOPIXEL_BRIGHTNESS);
  _strip.clear();
  _strip.show();
  // brief boot sweep so the operator can see the LED path works at power-up
  for (int j = 0; j < 256; j += 8) {
    for (int i = 0; i < NEOPIXEL_NUM; i++)
      _strip.setPixelColor(i, wheel((i * 256 / NEOPIXEL_NUM + j) & 0xFF));
    _strip.show();
    delay(6);
  }
  _strip.clear();
  _strip.show();
  _inited = true;
#ifdef NEOPIXEL_NOTIFY_DEBUG
  Serial.printf("[neopixel] ready: %d px on GPIO%d, boot sweep done\n", NEOPIXEL_NUM, PIN_NEOPIXEL);
#endif
}

void GenericNeoPixel::trigger() {
  if (!_inited) return;
  unsigned long now = millis();
  if (_start != 0) return;                                   // already flashing
  if (_last_end != 0 && now - _last_end < NEOPIXEL_COOLDOWN_MS) return;
  _start = now;
#ifdef NEOPIXEL_NOTIFY_DEBUG
  Serial.println("[neopixel] *** MESSAGE NOTIFY *** flashing strip");
#endif
}

void GenericNeoPixel::loop() {
  if (_start == 0) return;
  unsigned long elapsed = millis() - _start;
  if (elapsed >= NEOPIXEL_NOTIFY_MS) {                        // done
    _strip.clear();
    _strip.show();
    _start = 0;
    _last_end = millis();
    return;
  }
  // moving rainbow while active
  uint8_t phase = (uint8_t)((elapsed * 255) / 500);           // ~2 revolutions/sec
  for (int i = 0; i < NEOPIXEL_NUM; i++)
    _strip.setPixelColor(i, wheel((i * 256 / NEOPIXEL_NUM + phase) & 0xFF));
  _strip.show();
}

bool GenericNeoPixel::isActive() { return _start != 0; }

void GenericNeoPixel::off() {
  if (!_inited) return;
  _strip.clear();
  _strip.show();
  _start = 0;
}

#endif // PIN_NEOPIXEL
