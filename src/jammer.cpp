#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include <Preferences.h>
#include "jammer.h"

static SPIClass busM1(HSPI);
static SPIClass busM2(VSPI);

static RF24 m1(M1_CE_PIN, M1_CSN_PIN, NRF24_SPI_SPEED);
static RF24 m2(M2_CE_PIN, M2_CSN_PIN, NRF24_SPI_SPEED);

static Preferences prefs;

static rf24_pa_dbm_e pa_from_level(uint8_t lvl) {
    switch (lvl) {
        case 0:  return RF24_PA_MIN;
        case 1:  return RF24_PA_LOW;
        case 2:  return RF24_PA_HIGH;
        default: return RF24_PA_MAX;
    }
}

static void configure_radio(RF24& r) {
    r.stopListening();
    r.setAutoAck(false);
    r.disableCRC();
    r.setPayloadSize(32);
    r.setAddressWidth(5);
    r.setRetries(0, 0);
    r.setDataRate(RF24_2MBPS);
    r.setChannel(0);
    r.setPALevel(RF24_PA_MAX, true);
    uint64_t addr = 0x0LL;
    r.openWritingPipe(addr);
}

Jammer::Jammer()
    : mutex_(nullptr),
      mode_(JammerMode::IDLE),
      active_(false),
      pa_level_(PA_LEVEL_DEFAULT),
      cw_channel_(40),
      m1_ok_(false),
      m2_ok_(false),
      sweeps_(0),
      auto_stop_ms_(AUTO_STOP_DEFAULT_MS),
      start_time_(0),
      idx1_(0),
      idx2_(0),
      bt_m1_len(0),
      bt_m2_len(0),
      scan_count_(0),
      scan_done_(false) {}

Jammer::~Jammer() {
    if (mutex_) vSemaphoreDelete(mutex_);
}

void Jammer::lock()   { if (mutex_) xSemaphoreTake(mutex_, portMAX_DELAY); }
void Jammer::unlock() { if (mutex_) xSemaphoreGive(mutex_); }

bool Jammer::begin() {
    mutex_ = xSemaphoreCreateMutex();

    pinMode(STATUS_LED_PIN, OUTPUT);
    digitalWrite(STATUS_LED_PIN, LOW);

    for (uint8_t c = BT_CLASSIC_M1_START; c <= BT_CLASSIC_M1_END; c++) {
        bt_m1[bt_m1_len++] = c;
    }
    for (uint8_t c = BT_CLASSIC_M2_START; c <= BT_CLASSIC_M2_END; c++) {
        bt_m2[bt_m2_len++] = c;
    }

    busM1.begin(M1_SCK_PIN, M1_MISO_PIN, M1_MOSI_PIN, M1_CSN_PIN);
    busM2.begin(M2_SCK_PIN, M2_MISO_PIN, M2_MOSI_PIN, M2_CSN_PIN);

    m1_ok_ = m1.begin(&busM1);
    m2_ok_ = m2.begin(&busM2);

    if (m1_ok_) configure_radio(m1);
    if (m2_ok_) configure_radio(m2);

    apply_power_both();
    load_settings();
    return m1_ok_ || m2_ok_;
}

void Jammer::apply_power_both() {
    rf24_pa_dbm_e p = pa_from_level(pa_level_);
    if (m1_ok_) m1.setPALevel(p, true);
    if (m2_ok_) m2.setPALevel(p, true);
}

void Jammer::stop_both() {
    if (m1_ok_) m1.stopConstCarrier();
    if (m2_ok_) m2.stopConstCarrier();
    digitalWrite(STATUS_LED_PIN, LOW);
}

void Jammer::start_carrier_on_both(uint8_t ch1, uint8_t ch2) {
    rf24_pa_dbm_e p = pa_from_level(pa_level_);
    if (m1_ok_) m1.powerUp();
    if (m2_ok_) m2.powerUp();
    delayMicroseconds(5000);
    if (m1_ok_) m1.startConstCarrier(p, ch1);
    if (m2_ok_) m2.startConstCarrier(p, ch2);
    digitalWrite(STATUS_LED_PIN, HIGH);
}

uint8_t Jammer::m1_channel_now() const {
    switch (mode_) {
        case JammerMode::BLE_ADV:    return BLE_ADV_M1[idx1_ % BLE_ADV_M1_LEN];
        case JammerMode::BLE_FULL:   return BLE_FULL_M1[idx1_ % BLE_FULL_M1_LEN];
        case JammerMode::BT_CLASSIC: return bt_m1_len ? bt_m1[idx1_ % bt_m1_len] : 0;
        case JammerMode::CW_SINGLE:
        default:                     return cw_channel_;
    }
}

uint8_t Jammer::m2_channel_now() const {
    switch (mode_) {
        case JammerMode::BLE_ADV:    return BLE_ADV_M2[idx2_ % BLE_ADV_M2_LEN];
        case JammerMode::BLE_FULL:   return BLE_FULL_M2[idx2_ % BLE_FULL_M2_LEN];
        case JammerMode::BT_CLASSIC: return bt_m2_len ? bt_m2[idx2_ % bt_m2_len] : 0;
        case JammerMode::CW_SINGLE:
        default:                     return cw_channel_;
    }
}

void Jammer::set_mode(JammerMode m) {
    lock();
    if (m == mode_) { unlock(); return; }
    bool was_active = active_;
    if (was_active) {
        stop_both();
        active_ = false;
    }
    mode_ = m;
    idx1_ = 0;
    idx2_ = 0;
    sweeps_ = 0;
    if (was_active && m != JammerMode::IDLE && m != JammerMode::SCAN) {
        start_carrier_on_both(m1_channel_now(), m2_channel_now());
        active_ = true;
        start_time_ = millis();
    }
    unlock();
}

bool Jammer::set_channel(uint8_t ch) {
    if (ch > 125) return false;
    lock();
    cw_channel_ = ch;
    if (active_ && mode_ == JammerMode::CW_SINGLE) {
        if (m1_ok_) m1.setChannel(ch);
        if (m2_ok_) m2.setChannel(ch);
    }
    unlock();
    return true;
}

bool Jammer::set_power(uint8_t lvl) {
    if (lvl > 3) return false;
    lock();
    pa_level_ = lvl;
    if (active_) {
        uint8_t ch1 = m1_channel_now();
        uint8_t ch2 = m2_channel_now();
        stop_both();
        apply_power_both();
        start_carrier_on_both(ch1, ch2);
    } else {
        apply_power_both();
    }
    unlock();
    return true;
}

bool Jammer::set_auto_stop(uint32_t ms) {
    lock();
    auto_stop_ms_ = ms;
    if (active_) start_time_ = millis();
    unlock();
    return true;
}

void Jammer::start() {
    lock();
    if (active_) { unlock(); return; }
    if (mode_ == JammerMode::IDLE || mode_ == JammerMode::SCAN) { unlock(); return; }
    if (!m1_ok_ && !m2_ok_) { unlock(); return; }

    idx1_ = 0;
    idx2_ = 0;
    sweeps_ = 0;
    start_carrier_on_both(m1_channel_now(), m2_channel_now());
    active_ = true;
    start_time_ = millis();
    unlock();
}

void Jammer::stop() {
    lock();
    if (!active_) { unlock(); return; }
    stop_both();
    active_ = false;
    unlock();
}

void Jammer::tick() {
    if (!active_) return;
    if (mode_ == JammerMode::CW_SINGLE || mode_ == JammerMode::SCAN) return;

    lock();

    if (auto_stop_ms_ > 0 && (millis() - start_time_) >= auto_stop_ms_) {
        stop_both();
        active_ = false;
        unlock();
        return;
    }

    idx1_++;
    idx2_++;

    uint8_t ch1 = m1_channel_now();
    uint8_t ch2 = m2_channel_now();

    if (m1_ok_) m1.setChannel(ch1);
    if (m2_ok_) m2.setChannel(ch2);

    delayMicroseconds(PLL_SETTLE_US);

    uint8_t l1 = (mode_ == JammerMode::BLE_ADV)    ? BLE_ADV_M1_LEN  :
                 (mode_ == JammerMode::BLE_FULL)   ? BLE_FULL_M1_LEN :
                 (mode_ == JammerMode::BT_CLASSIC) ? bt_m1_len        : 1;
    if (l1 && (idx1_ % l1) == 0) sweeps_++;

    unlock();
}

JammerStatus Jammer::status() const {
    JammerStatus s{};
    s.mode       = mode_;
    s.active     = active_;
    s.pa_level   = pa_level_;
    s.cw_channel = cw_channel_;
    s.m1_ok      = m1_ok_;
    s.m2_ok      = m2_ok_;
    s.sweeps     = sweeps_;
    s.auto_stop_ms   = auto_stop_ms_;
    s.elapsed_ms     = active_ ? (millis() - start_time_) : 0;
    s.remaining_ms   = (active_ && auto_stop_ms_ > 0)
                        ? (auto_stop_ms_ > s.elapsed_ms ? auto_stop_ms_ - s.elapsed_ms : 0)
                        : 0;
    return s;
}

void Jammer::run_scan() {
    lock();
    if (active_) {
        stop_both();
        active_ = false;
    }

    scan_count_ = 0;
    scan_done_ = false;

    for (uint8_t ch = SCAN_CH_START; ch <= SCAN_CH_END; ch++) {
        bool m1_det = false, m2_det = false;

        if (m1_ok_) {
            m1.setChannel(ch);
            m1.startListening();
            delay(SCAN_DWELL_MS);
            m1_det = m1.testRPD();
            m1.stopListening();
        }
        if (m2_ok_) {
            m2.setChannel(ch);
            m2.startListening();
            delay(SCAN_DWELL_MS);
            m2_det = m2.testRPD();
            m2.stopListening();
        }

        scan_results_[scan_count_].channel = ch;
        scan_results_[scan_count_].m1_detected = m1_det;
        scan_results_[scan_count_].m2_detected = m2_det;
        scan_count_++;
    }

    scan_done_ = true;
    unlock();
}

void Jammer::save_settings() {
    prefs.begin(NVS_NAMESPACE, false);
    prefs.putUChar("mode",    (uint8_t)mode_);
    prefs.putUChar("power",   pa_level_);
    prefs.putUChar("channel", cw_channel_);
    prefs.putULong("timer",   auto_stop_ms_);
    prefs.end();
}

void Jammer::load_settings() {
    prefs.begin(NVS_NAMESPACE, true);
    if (prefs.isKey("power")) {
        pa_level_     = prefs.getUChar("power", PA_LEVEL_DEFAULT);
        cw_channel_   = prefs.getUChar("channel", 40);
        auto_stop_ms_ = prefs.getULong("timer", AUTO_STOP_DEFAULT_MS);
        uint8_t saved_mode = prefs.getUChar("mode", 0);
        mode_ = (saved_mode <= (uint8_t)JammerMode::SCAN) ? (JammerMode)saved_mode : JammerMode::IDLE;
        apply_power_both();
    }
    prefs.end();
}
