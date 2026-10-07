#ifndef DISPLAY_H
#define DISPLAY_H

#include "jammer.h"

class Display {
public:
    Display(Jammer& j);
    bool begin();
    void update();

private:
    Jammer&  jammer_;
    bool     ok_;
    uint32_t last_update_;
};

#endif
