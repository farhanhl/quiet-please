#ifndef WEB_CONTROL_H
#define WEB_CONTROL_H

#include "jammer.h"

class WebControl {
public:
    WebControl(Jammer& j);
    void begin();
    void loop();

private:
    void init_ap();
    void init_routes();
    bool check_auth();

    void handle_root();
    void handle_status();
    void handle_mode();
    void handle_channel();
    void handle_power();
    void handle_start();
    void handle_stop();
    void handle_scan();
    void handle_scan_results();
    void handle_timer();
    void handle_save();
    void handle_not_found();

    Jammer& jammer_;
    bool    ap_ok_;
};

#endif
