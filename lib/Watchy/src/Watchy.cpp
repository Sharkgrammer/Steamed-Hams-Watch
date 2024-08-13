#include "Watchy.h"

Watchy32KRTC Watchy::RTC;
#define ACTIVE_LOW 0
GxEPD2_BW <WatchyDisplay, WatchyDisplay::HEIGHT> Watchy::display(
        WatchyDisplay{});

RTC_DATA_ATTR bool USB_PLUGGED_IN = false;

void Watchy::init(String datetime) {
    esp_sleep_wakeup_cause_t wakeup_reason;
    wakeup_reason = esp_sleep_get_wakeup_cause(); // get wake up reason
    Wire.begin(WATCHY_V3_SDA, WATCHY_V3_SCL);     // init i2c
    RTC.init();
    // Init the display since is almost sure we will use it
    display.epd2.initWatchy();

    switch (wakeup_reason) {
        case ESP_SLEEP_WAKEUP_TIMER: // RTC Alarm
            RTC.read(currentTime);
            showWatchFace(true);

            break;
        default: // reset
            RTC.config(datetime);

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
    esp_sleep_enable_ext0_wakeup((gpio_num_t) USB_DET_PIN,
                                 USB_PLUGGED_IN ? LOW : HIGH); //// enable deep sleep wake on USB plug in/out
    rtc_gpio_set_direction((gpio_num_t) USB_DET_PIN, RTC_GPIO_MODE_INPUT_ONLY);
    rtc_gpio_pullup_en((gpio_num_t) USB_DET_PIN);

    esp_sleep_enable_ext1_wakeup(
            BTN_PIN_MASK,
            ESP_EXT1_WAKEUP_ANY_LOW); // enable deep sleep wake on button press
    rtc_gpio_set_direction((gpio_num_t) UP_BTN_PIN, RTC_GPIO_MODE_INPUT_ONLY);
    rtc_gpio_pullup_en((gpio_num_t) UP_BTN_PIN);

    rtc_clk_32k_enable(true);
    //rtc_clk_slow_freq_set(RTC_SLOW_FREQ_32K_XTAL);
    struct tm timeinfo;
    getLocalTime(&timeinfo);
    int secToNextMin = 60 - timeinfo.tm_sec;
    esp_sleep_enable_timer_wakeup(secToNextMin * uS_TO_S_FACTOR);
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
    display.setFont(&DSEG7_Classic_Bold_53);
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
}