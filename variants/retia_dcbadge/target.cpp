#include <Arduino.h>
#include "target.h"

RetiaDCBadgeBoard board;

// SX1276 (RFM95W): only DIO0 is routed on this badge, DIO1 is passed as
// RADIOLIB_NC. Radio, TFT and (bit-banged) touch share ONE physical SPI bus,
// so radio and display BOTH use the global Arduino `SPI` object — same HAL
// bus struct, same lock, per-transaction reconfig. Do NOT give either its
// own SPIClass/host: split ownership loses the shared lock, and the default
// FSPI pin mapping (SCK=12/MISO=13) is swapped relative to this badge.
RADIO_CLASS radio = new Module(P_LORA_NSS, P_LORA_DIO_0, P_LORA_RESET, P_LORA_DIO_1, SPI);

// Claim the bus with the badge's pin mapping FIRST (from board.begin), before
// the display driver's begin() could claim it with the wrong default pins.
// SPIClass::begin is guarded, so the later calls become no-ops.
void retia_claim_spi() {
  SPI.begin(P_LORA_SCLK, P_LORA_MISO, P_LORA_MOSI);
}

WRAPPER_CLASS radio_driver(radio, board);

ESP32RTCClock fallback_clock;
AutoDiscoverRTCClock rtc_clock(fallback_clock);
SensorManager sensors;

#ifdef PIN_USER_BTN
  MomentaryButton user_btn(PIN_USER_BTN, 1000, true);
#endif
#ifdef DISPLAY_CLASS
  DISPLAY_CLASS display;
#endif

bool radio_init() {
  fallback_clock.begin();
  rtc_clock.begin(Wire);

  return radio.std_init(&SPI);
}

mesh::LocalIdentity radio_new_identity() {
  RadioNoiseListener rng(radio);
  return mesh::LocalIdentity(&rng);  // create new random identity
}
