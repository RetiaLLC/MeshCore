#pragma once

#include <Arduino.h>
#include <helpers/ESP32Board.h>

// Retia 2024 DEF CON badge (ESP32-S3-WROOM-1, 8MB flash / 2MB QSPI PSRAM).
// RFM95W (SX1276) shares ONE SPI bus with the ILI9341 TFT, XPT2046 touch and
// micro-SD. Only DIO0 is routed from the radio (no DIO1/BUSY).
// There is NO battery sense on this hardware — USB presence is the only
// external-power signal.

// chip-selects on the shared SPI bus that must be parked high before any
// bus traffic (TFT, touch, dedicated SD, display-module SD)
#define PIN_TFT_CS    47
#define PIN_TOUCH_CS  14
#define PIN_SD_CS     10
#define PIN_MODSD_CS  39

// defined in target.cpp: claims the shared SPI bus with the Arduino SPI
// driver BEFORE LovyanGFX initialises on top of it. Order matters: doing it
// the other way round (LovyanGFX first, SPIClass::begin later, as the stock
// companion setup() sequence would) resets the SPI2 host underneath LovyanGFX
// and every subsequent display write is silently lost.
void retia_claim_spi();

class RetiaDCBadgeBoard : public ESP32Board {
public:
  void begin() {
    // park all other CS lines on the shared SPI bus so the radio (and later
    // the display) never see a floating/active neighbour during transactions
    const uint8_t cs_pins[] = { PIN_TFT_CS, PIN_TOUCH_CS, PIN_SD_CS, PIN_MODSD_CS };
    for (uint8_t i = 0; i < sizeof(cs_pins); i++) {
      pinMode(cs_pins[i], OUTPUT);
      digitalWrite(cs_pins[i], HIGH);
    }

    retia_claim_spi();

    ESP32Board::begin();

    esp_reset_reason_t reason = esp_reset_reason();
    if (reason == ESP_RST_DEEPSLEEP) {
      long wakeup_source = esp_sleep_get_ext1_wakeup_status();
      if (wakeup_source & (1 << P_LORA_DIO_0)) {  // received a LoRa packet (while in deep sleep)
        startup_reason = BD_STARTUP_RX_PACKET;
      }

      rtc_gpio_hold_dis((gpio_num_t)P_LORA_NSS);
      rtc_gpio_deinit((gpio_num_t)P_LORA_DIO_0);
    }
  }

  uint16_t getBattMilliVolts() override {
    return 0;   // no battery sense on this hardware
  }

  bool isExternalPowered() override {
    return true;  // no battery: the badge only runs on USB / SAO 5V power
  }

  const char* getManufacturerName() const override {
    return "Retia DEF CON Badge";
  }

  uint32_t getIRQGpio() override {
    return P_LORA_DIO_0;  // SX1276: IRQ is DIO0 (DIO1/BUSY not routed)
  }
};
