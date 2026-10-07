#ifndef SERIAL_CONTROL_H
#define SERIAL_CONTROL_H

#include <stddef.h>
#include "jammer.h"

class SerialControl {
public:
    SerialControl(Jammer& j);
    void begin(uint32_t baud);
    void loop();

private:
    void handle(const char* line);
    void print_help();
    void print_status();
    void print_scan();

    Jammer& jammer_;
    char    buf_[64];
    size_t  pos_;
};

#endif
