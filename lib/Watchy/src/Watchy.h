#ifndef WATCHY_H
#define WATCHY_H

#include <Arduino.h>
#include <WiFiManager.h>
#include <GxEPD2_BW.h>
#include <Wire.h>
#include "DSEG7_Classic_Bold_53.h"
#include "Display.h"
#include "BLE.h"
#include "Watchy32KRTC.h"
#include "soc/rtc.h"
#include "soc/rtc_io_reg.h"
#include "soc/sens_reg.h"
#include "esp_sleep.h"
#include "rom/rtc.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "time.h"
#include "esp_sntp.h"
#include "hal/rtc_io_types.h"
#include "driver/rtc_io.h"
#define uS_TO_S_FACTOR 1000000ULL  //Conversion factor for micro seconds to seconds
#define ADC_VOLTAGE_DIVIDER ((360.0f+100.0f)/360.0f) //Voltage divider at battery ADC

class Watchy {
public:
    static Watchy32KRTC RTC;
    static GxEPD2_BW <WatchyDisplay, WatchyDisplay::HEIGHT> display;
    tmElements_t currentTime;

public:
    void init(String datetime = "");

    void deepSleep();

    void showWatchFace(bool partialRefresh);

    virtual void drawWatchFace();
};

#endif
