#include <Arduino.h>
#include <string.h>
#include <strings.h>
#include "serial_control.h"

static const char* mode_name(JammerMode m) {
    switch (m) {
        case JammerMode::IDLE:       return "IDLE";
        case JammerMode::BLE_ADV:    return "BLE_ADV";
        case JammerMode::BLE_FULL:   return "BLE_FULL";
        case JammerMode::BT_CLASSIC: return "BT_CLASSIC";
        case JammerMode::CW_SINGLE:  return "CW_SINGLE";
        case JammerMode::SCAN:       return "SCAN";
    }
    return "?";
}

SerialControl::SerialControl(Jammer& j) : jammer_(j), pos_(0) {
    buf_[0] = '\0';
}

void SerialControl::begin(uint32_t baud) {
    Serial.begin(baud);
    while (!Serial && millis() < 1500) { delay(10); }
    Serial.println();
    Serial.println("quiet-please // nRF24 dual-module BT jammer");
    Serial.println("type `help` for commands");
    print_status();
}

void SerialControl::loop() {
    while (Serial.available()) {
        int c = Serial.read();
        if (c == '\r') continue;
        if (c == '\n') {
            buf_[pos_] = '\0';
            if (pos_ > 0) handle(buf_);
            pos_ = 0;
        } else if (c == 0x08 || c == 0x7F) {
            if (pos_ > 0) pos_--;
        } else if (pos_ < sizeof(buf_) - 1) {
            buf_[pos_++] = (char)c;
            Serial.write((char)c);
        }
    }
}

void SerialControl::handle(const char* line) {
    Serial.println();

    static char copy[64];
    strncpy(copy, line, sizeof(copy) - 1);
    copy[sizeof(copy) - 1] = '\0';

    char* tok = strtok(copy, " \t");
    if (!tok) return;
    const char* cmd = tok;
    tok = strtok(nullptr, " \t");
    const char* arg = tok;

    if (strcasecmp(cmd, "help") == 0 || strcasecmp(cmd, "?") == 0) {
        print_help();
    } else if (strcasecmp(cmd, "status") == 0) {
        print_status();
    } else if (strcasecmp(cmd, "start") == 0) {
        jammer_.start();
        Serial.println(jammer_.is_active() ? "started" : "refused: no mode or module");
    } else if (strcasecmp(cmd, "stop") == 0) {
        jammer_.stop();
        Serial.println("stopped");
    } else if (strcasecmp(cmd, "mode") == 0) {
        if (!arg) { Serial.println("usage: mode <0|1|2|3|4|5>"); return; }
        JammerMode m = JammerMode::IDLE;
        if      (strcasecmp(arg, "0") == 0 || strcasecmp(arg, "IDLE") == 0)       m = JammerMode::IDLE;
        else if (strcasecmp(arg, "1") == 0 || strcasecmp(arg, "BLE_ADV") == 0)    m = JammerMode::BLE_ADV;
        else if (strcasecmp(arg, "2") == 0 || strcasecmp(arg, "BLE_FULL") == 0)   m = JammerMode::BLE_FULL;
        else if (strcasecmp(arg, "3") == 0 || strcasecmp(arg, "BT_CLASSIC") == 0
                                || strcasecmp(arg, "BT") == 0)                    m = JammerMode::BT_CLASSIC;
        else if (strcasecmp(arg, "4") == 0 || strcasecmp(arg, "CW") == 0
                                || strcasecmp(arg, "CW_SINGLE") == 0)             m = JammerMode::CW_SINGLE;
        else if (strcasecmp(arg, "5") == 0 || strcasecmp(arg, "SCAN") == 0)       m = JammerMode::SCAN;
        else { Serial.println("invalid mode"); return; }
        jammer_.set_mode(m);
        Serial.printf("mode = %s\n", mode_name(m));
    } else if (strcasecmp(cmd, "channel") == 0) {
        if (!arg) { Serial.println("usage: channel <0-125>"); return; }
        int v = atoi(arg);
        if (v < 0 || v > 125) { Serial.println("invalid channel (0-125)"); return; }
        if (jammer_.set_channel((uint8_t)v))
            Serial.printf("cw_channel = %d (%d MHz)\n", v, 2400 + v);
    } else if (strcasecmp(cmd, "power") == 0) {
        if (!arg) { Serial.println("usage: power <0|1|2|3>"); return; }
        int v = atoi(arg);
        if (v < 0 || v > 3) { Serial.println("invalid power (0-3)"); return; }
        jammer_.set_power((uint8_t)v);
        Serial.printf("pa_level = %d\n", v);
    } else if (strcasecmp(cmd, "timer") == 0) {
        if (!arg) { Serial.println("usage: timer <seconds>  (0=off)"); return; }
        int v = atoi(arg);
        if (v < 0) { Serial.println("invalid"); return; }
        jammer_.set_auto_stop((uint32_t)v * 1000);
        Serial.printf("auto_stop = %d sec\n", v);
    } else if (strcasecmp(cmd, "scan") == 0) {
        Serial.println("scanning...");
        jammer_.set_mode(JammerMode::SCAN);
        jammer_.run_scan();
        print_scan();
    } else if (strcasecmp(cmd, "save") == 0) {
        jammer_.save_settings();
        Serial.println("settings saved to NVS");
    } else if (strcasecmp(cmd, "reset") == 0) {
        Serial.println("resetting...");
        Serial.flush();
        delay(50);
        ESP.restart();
    } else {
        Serial.print("unknown: ");
        Serial.println(cmd);
    }
}

void SerialControl::print_help() {
    Serial.println(F("commands:"));
    Serial.println(F("  help                    this list"));
    Serial.println(F("  status                  current state"));
    Serial.println(F("  mode <0-5>              IDLE/BLE_ADV/BLE_FULL/BT_CLASSIC/CW/SCAN"));
    Serial.println(F("  channel <0-125>         target channel for CW (freq=2400+ch MHz)"));
    Serial.println(F("  power <0-3>             TX power: 0=-18dBm 1=-12 2=-6 3=0dBm"));
    Serial.println(F("  timer <seconds>         auto-stop timer (0=off)"));
    Serial.println(F("  start                   start jamming"));
    Serial.println(F("  stop                    stop jamming"));
    Serial.println(F("  scan                    scan channels for RF activity"));
    Serial.println(F("  save                    save settings to NVS"));
    Serial.println(F("  reset                   soft-reboot ESP32"));
    Serial.println(F("channel map: BT=2..80, BLE adv={2,26,80}, BLE full=even 0..80"));
}

void SerialControl::print_status() {
    JammerStatus s = jammer_.status();
    Serial.println(F("---- status ----"));
    Serial.printf("mode       : %s\n", mode_name(s.mode));
    Serial.printf("active     : %s\n", s.active ? "YES" : "no");
    Serial.printf("cw_channel : %u (%u MHz)\n", s.cw_channel, 2400u + s.cw_channel);
    Serial.printf("pa_level   : %u\n", s.pa_level);
    Serial.printf("auto_stop  : %lu sec\n", (unsigned long)(s.auto_stop_ms / 1000));
    if (s.active && s.remaining_ms > 0)
        Serial.printf("remaining  : %lu sec\n", (unsigned long)(s.remaining_ms / 1000));
    Serial.printf("module M1  : %s\n", s.m1_ok ? "OK (HSPI)" : "MISSING");
    Serial.printf("module M2  : %s\n", s.m2_ok ? "OK (VSPI)" : "MISSING");
    Serial.printf("sweeps     : %lu\n", (unsigned long)s.sweeps);
    Serial.println(F("----------------"));
}

void SerialControl::print_scan() {
    if (!jammer_.scan_available()) {
        Serial.println("no scan data — run `scan` first");
        return;
    }
    const ScanResult* r = jammer_.scan_results();
    uint8_t n = jammer_.scan_count();
    Serial.println(F("---- scan results ----"));
    Serial.println(F("ch  freq  M1 M2"));
    for (uint8_t i = 0; i < n; i++) {
        Serial.printf("%3u %4u  %s  %s\n",
            r[i].channel, 2400 + r[i].channel,
            r[i].m1_detected ? "DET" : "---",
            r[i].m2_detected ? "DET" : "---");
    }
    Serial.println(F("----------------------"));
}
