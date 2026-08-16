#include <Arduino.h>
#include "target.h"

ESP32Board board;

// RFM95 LoRa backpack = SX1276. Only DIO0 is wired on the Nugget's expansion
// header (DIO1 passed as RADIOLIB_NC). The radio has its OWN SPI bus (pins
// 6/7/8/9); the OLED is on I2C (35/36), so no shared-bus juggling is needed.
// CustomSX1276::std_init() calls spi.begin(P_LORA_SCLK,P_LORA_MISO,P_LORA_MOSI)
// itself, so target.cpp does not need to.
static SPIClass spi;
RADIO_CLASS radio = new Module(P_LORA_NSS, P_LORA_DIO_0, P_LORA_RESET, P_LORA_DIO_1, spi);
WRAPPER_CLASS radio_driver(radio, board);

ESP32RTCClock fallback_clock;
AutoDiscoverRTCClock rtc_clock(fallback_clock);
SensorManager sensors;

#ifdef DISPLAY_CLASS
  DISPLAY_CLASS display;
  // Nugget buttons are active-low; enable the internal pull-up (4th arg) so a
  // floating input can't read as a phantom long-press -> spurious CLI rescue.
  MomentaryButton user_btn(PIN_USER_BTN, 1000, true, true);
#endif

bool radio_init() {
  fallback_clock.begin();
  rtc_clock.begin(Wire);

  return radio.std_init(&spi);
}

mesh::LocalIdentity radio_new_identity() {
  RadioNoiseListener rng(radio);
  return mesh::LocalIdentity(&rng);  // create new random identity
}
