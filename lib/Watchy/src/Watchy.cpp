#include "Watchy.h"

Watchy32KRTC Watchy::RTC;
#define ACTIVE_LOW 0
GxEPD2_BW <WatchyDisplay, WatchyDisplay::HEIGHT> Watchy::display(
        WatchyDisplay{});

RTC_DATA_ATTR bool USB_PLUGGED_IN = false;
RTC_DATA_ATTR int counter = 1;

void Watchy::init(String datetime) {
    esp_sleep_wakeup_cause_t wakeup_reason;
    wakeup_reason = esp_sleep_get_wakeup_cause(); // get wake up reason
#ifdef ARDUINO_ESP32S3_DEV
    Wire.begin(WATCHY_V3_SDA, WATCHY_V3_SCL);     // init i2c
#else
    Wire.begin(SDA, SCL);                         // init i2c
#endif
    RTC.init();
    // Init the display since is almost sure we will use it
    display.epd2.initWatchy();

    switch (wakeup_reason) {
        case ESP_SLEEP_WAKEUP_TIMER: // RTC Alarm
            RTC.read(currentTime);
            showWatchFace(true);

            break;
        case ESP_SLEEP_WAKEUP_EXT1:
            // Do nothing for now
            break;
        default: // reset
            RTC.config(datetime);

        #ifdef ARDUINO_ESP32S3_DEV
                    pinMode(USB_DET_PIN, INPUT);
            USB_PLUGGED_IN = (digitalRead(USB_DET_PIN) == 1);
        #endif

            RTC.read(currentTime);
            showWatchFace(false); // full update on reset

            // For some reason, seems to be enabled on first boot
            esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
            break;
    }
    deepSleep();
}

void Watchy::deepSleep() {
    display.hibernate();
    RTC.clearAlarm();        // resets the alarm flag in the RTC
#ifdef ARDUINO_ESP32S3_DEV
    esp_sleep_enable_ext0_wakeup((gpio_num_t)USB_DET_PIN, USB_PLUGGED_IN ? LOW : HIGH); //// enable deep sleep wake on USB plug in/out
  rtc_gpio_set_direction((gpio_num_t)USB_DET_PIN, RTC_GPIO_MODE_INPUT_ONLY);
  rtc_gpio_pullup_en((gpio_num_t)USB_DET_PIN);

  esp_sleep_enable_ext1_wakeup(
      BTN_PIN_MASK,
      ESP_EXT1_WAKEUP_ANY_LOW); // enable deep sleep wake on button press
  rtc_gpio_set_direction((gpio_num_t)UP_BTN_PIN, RTC_GPIO_MODE_INPUT_ONLY);
  rtc_gpio_pullup_en((gpio_num_t)UP_BTN_PIN);

  rtc_clk_32k_enable(true);
  //rtc_clk_slow_freq_set(RTC_SLOW_FREQ_32K_XTAL);
  struct tm timeinfo;
  getLocalTime(&timeinfo);
  int secToNextMin = 60 - timeinfo.tm_sec;
  esp_sleep_enable_timer_wakeup(secToNextMin * uS_TO_S_FACTOR);
#else
    // Set GPIOs 0-39 to input to avoid power leaking out
    const uint64_t ignore = 0b11110001000000110000100111000010; // Ignore some GPIOs due to resets
    for (int i = 0; i < GPIO_NUM_MAX; i++) {
        if ((ignore >> i) & 0b1)
            continue;
        pinMode(i, INPUT);
    }
    esp_sleep_enable_ext0_wakeup((gpio_num_t)RTC_INT_PIN,
                                 0); // enable deep sleep wake on RTC interrupt
    esp_sleep_enable_ext1_wakeup(
            BTN_PIN_MASK,
            ESP_EXT1_WAKEUP_ANY_HIGH); // enable deep sleep wake on button press
#endif
    esp_deep_sleep_start();
}

void Watchy::showWatchFace(bool partialRefresh) {
    display.setFullWindow();
    // At this point it is sure we are going to update
    display.epd2.asyncPowerOn();
    drawWatchFace();
    display.display(partialRefresh); // partial refresh
}

void Watchy::drawWatchFace() {
    display.fillScreen(GxEPD_WHITE);
    /*
    display.setFont(&DSEG7_Classic_Bold_53);
    display.setTextColor(GxEPD_BLACK);

    display.setCursor(5, 53 + 60);
    if (currentTime.Hour < 10) {
        display.print("0");
    }
    display.print(currentTime.Hour);
    display.print(":");
    if (currentTime.Minute < 10) {
        display.print("0");
    }
    display.println(currentTime.Minute);
    */

    if (counter > 1440){
        counter = 0;
    }

    display.drawBitmap(0, 0, fmap[counter], 200, 200, GxEPD_BLACK);

    counter += 1;
}
