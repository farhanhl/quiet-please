#include <Arduino.h>
#include "config.h"
#include "jammer.h"
#include "serial_control.h"
#include "web_control.h"
#include "display.h"

static Jammer jammer;
static SerialControl ctrl(jammer);
static WebControl web(jammer);
static Display oled(jammer);

static TaskHandle_t jammer_task_handle = nullptr;

static void jammer_task(void* param) {
    while (true) {
        if (jammer.is_active()) {
            jammer.tick();
        } else {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
}

void setup() {
    ctrl.begin(SERIAL_BAUD);
    if (!jammer.begin()) {
        Serial.println("WARNING: no nRF24 module responded on either SPI bus");
        Serial.println("check wiring + 10uF/0.1uF caps on each module");
    }
    oled.begin();
    web.begin();

    xTaskCreatePinnedToCore(
        jammer_task,
        "jammer",
        JAMMER_TASK_STACK,
        nullptr,
        JAMMER_TASK_PRIO,
        &jammer_task_handle,
        JAMMER_TASK_CORE);
}

void loop() {
    ctrl.loop();
    web.loop();
    oled.update();
}
