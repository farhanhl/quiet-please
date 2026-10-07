#ifndef JAMMER_H
#define JAMMER_H

#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include "config.h"

enum class JammerMode : uint8_t {
    IDLE = 0,
    BLE_ADV,
    BLE_FULL,
    BT_CLASSIC,
    CW_SINGLE,
    SCAN,
};

struct JammerStatus {
    JammerMode mode;
    bool       active;
    uint8_t    pa_level;
    uint8_t    cw_channel;
    bool       m1_ok;
    bool       m2_ok;
    uint32_t   sweeps;
    uint32_t   auto_stop_ms;
    uint32_t   elapsed_ms;
    uint32_t   remaining_ms;
};

struct ScanResult {
    uint8_t  channel;
    bool     m1_detected;
    bool     m2_detected;
};

class Jammer {
public:
    Jammer();
    ~Jammer();

    bool  begin();
    void  tick();
    void  start();
    void  stop();

    void         set_mode(JammerMode m);
    JammerMode   mode() const { return mode_; }
    bool         is_active() const { return active_; }

    bool    set_channel(uint8_t ch);
    uint8_t channel() const { return cw_channel_; }

    bool         set_power(uint8_t lvl);
    uint8_t      power() const { return pa_level_; }

    bool          set_auto_stop(uint32_t ms);
    uint32_t      auto_stop_ms() const { return auto_stop_ms_; }

    JammerStatus status() const;

    void run_scan();
    bool scan_available() const { return scan_done_; }
    const ScanResult* scan_results() const { return scan_results_; }
    uint8_t scan_count() const { return scan_count_; }

    void save_settings();
    void load_settings();

private:
    uint8_t m1_channel_now() const;
    uint8_t m2_channel_now() const;
    void    apply_power_both();
    void    stop_both();
    void    start_carrier_on_both(uint8_t ch1, uint8_t ch2);
    void    lock();
    void    unlock();

    SemaphoreHandle_t mutex_;

    JammerMode mode_;
    bool       active_;
    uint8_t    pa_level_;
    uint8_t    cw_channel_;
    bool       m1_ok_;
    bool       m2_ok_;
    uint32_t   sweeps_;

    uint32_t  auto_stop_ms_;
    uint32_t  start_time_;

    uint8_t idx1_;
    uint8_t idx2_;

    uint8_t bt_m1[40];
    uint8_t bt_m1_len;
    uint8_t bt_m2[40];
    uint8_t bt_m2_len;

    ScanResult scan_results_[SCAN_NUM_CH];
    uint8_t    scan_count_;
    bool       scan_done_;
};

#endif
