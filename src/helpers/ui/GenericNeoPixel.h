#pragma once

#ifdef PIN_NEOPIXEL

#include <Arduino.h>

/*
 * NeoPixel (WS2812) notification strip.
 *
 * Mirrors GenericVibration: a non-blocking, cooldown-gated "notify" that the
 * companion UITask fires for new-message events. On a screenless board (e.g. the
 * Retia Newsheen / Pusheen Puck) this IS the message indicator — a bright color
 * sweep across the strip for NEOPIXEL_NOTIFY_MS, then dark again.
 *
 * Build defines:
 *   PIN_NEOPIXEL        data GPIO (required to enable this class)
 *   NEOPIXEL_NUM        pixel count (default 8)
 *   NEOPIXEL_BRIGHTNESS 0..255 global brightness (default 200; stock-dim looks dead
 *                       through a silicone diffuser)
 *   NEOPIXEL_NOTIFY_MS  flash duration (default 2500)
 *   NEOPIXEL_COOLDOWN_MS min gap between flashes (default 4000)
 */

#ifndef NEOPIXEL_NUM
#define NEOPIXEL_NUM 8
#endif
#ifndef NEOPIXEL_BRIGHTNESS
#define NEOPIXEL_BRIGHTNESS 200
#endif
#ifndef NEOPIXEL_NOTIFY_MS
#define NEOPIXEL_NOTIFY_MS 2500
#endif
#ifndef NEOPIXEL_COOLDOWN_MS
#define NEOPIXEL_COOLDOWN_MS 4000
#endif

class GenericNeoPixel {
public:
  void begin();     // init strip + a short boot sweep (proves the LED path)
  void trigger();   // start a notify flash if the cooldown has passed
  void loop();      // non-blocking animation driver
  bool isActive();  // true while a flash is playing
  void off();       // clear immediately

private:
  unsigned long _start = 0;      // 0 = idle
  unsigned long _last_end = 0;   // for cooldown
  bool _inited = false;
};

#endif // PIN_NEOPIXEL
