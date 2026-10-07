#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"
#include "display.h"

static Adafruit_SSD1306 oled(OLED_W, OLED_H, &Wire, -1);

static const char* mode_short(JammerMode m) {
    switch (m) {
        case JammerMode::IDLE:       return "IDLE";
        case JammerMode::BLE_ADV:    return "BADV";
        case JammerMode::BLE_FULL:   return "BFUL";
        case JammerMode::BT_CLASSIC: return "BCLA";
        case JammerMode::CW_SINGLE:  return "CW";
        case JammerMode::SCAN:       return "SCAN";
    }
    return "?";
}

Display::Display(Jammer& j) : jammer_(j), ok_(false), last_update_(0) {}

bool Display::begin() {
    Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);
    ok_ = oled.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
    if (ok_) {
        oled.clearDisplay();
        oled.setTextSize(1);
        oled.setTextColor(SSD1306_WHITE);
        oled.setCursor(0, 0);
        oled.println("quiet-please");
        oled.println("booting...");
        oled.display();
    }
    return ok_;
}

void Display::update() {
    if (!ok_) return;
    if (millis() - last_update_ < 250) return;
    last_update_ = millis();

    JammerStatus s = jammer_.status();

    oled.clearDisplay();

    oled.setCursor(0, 0);
    oled.setTextSize(1);
    oled.setTextColor(SSD1306_WHITE);

    oled.print(s.active ? "*" : " ");
    oled.print(" ");
    oled.println(mode_short(s.mode));

    oled.print("PWR:");
    oled.print(s.pa_level);
    oled.print(" CH:");
    oled.print(s.cw_channel);
    oled.print("(");
    oled.print(2400 + s.cw_channel);
    oled.println(")");

    oled.print("M1:");
    oled.print(s.m1_ok ? "OK" : "--");
    oled.print(" M2:");
    oled.println(s.m2_ok ? "OK" : "--");

    oled.print("SWP:");
    oled.print(s.sweeps);

    if (s.active && s.remaining_ms > 0) {
        uint32_t sec = s.remaining_ms / 1000;
        uint32_t m = sec / 60;
        uint32_t ss = sec % 60;
        oled.setCursor(OLED_W - 42, 48);
        oled.printf("T:%lum%lu", m, ss);
    }

    if (s.active) {
        oled.fillRect(0, OLED_H - 2, OLED_W, 2, SSD1306_WHITE);
    }

    oled.display();
}
