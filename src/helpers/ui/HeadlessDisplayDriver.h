#pragma once

#include <helpers/ui/NullDisplayDriver.h>

/*
 * A DisplayDriver that reports "present" (begin() -> true) but draws nothing.
 *
 * The companion firmware's whole UITask — including its message buzzer/vibration/
 * NeoPixel notify — is gated on DISPLAY_CLASS and only runs when display.begin()
 * succeeds. Screenless boards (Retia Newsheen / Pusheen Puck) set
 * DISPLAY_CLASS=HeadlessDisplayDriver so UITask (and the notify) runs with no OLED.
 * All draw calls are inherited no-ops from NullDisplayDriver.
 */
class HeadlessDisplayDriver : public NullDisplayDriver {
public:
  bool begin() { return true; }
};
