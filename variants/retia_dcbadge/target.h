#pragma once

#define RADIOLIB_STATIC_ONLY 1
#include <RadioLib.h>
#include <helpers/radiolib/RadioLibWrappers.h>
#include <RetiaDCBadgeBoard.h>
#include <helpers/radiolib/CustomSX1276Wrapper.h>
#include <helpers/AutoDiscoverRTCClock.h>
#include <helpers/SensorManager.h>
#include <helpers/ui/MomentaryButton.h>
#ifdef DISPLAY_CLASS
  #include <helpers/ui/ILI9341LCDDisplay.h>
#endif

extern RetiaDCBadgeBoard board;
extern WRAPPER_CLASS radio_driver;
extern AutoDiscoverRTCClock rtc_clock;
extern SensorManager sensors;

#ifdef PIN_USER_BTN
  extern MomentaryButton user_btn;
#endif
#ifdef DISPLAY_CLASS
  extern DISPLAY_CLASS display;
#endif

bool radio_init();
mesh::LocalIdentity radio_new_identity();
