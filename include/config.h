#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

// ============================================================
// ESP32 Pin Definitions
// ============================================================

// Module 1 — HSPI bus
#define M1_CE_PIN     16
#define M1_CSN_PIN    15
#define M1_SCK_PIN    14
#define M1_MOSI_PIN   13
#define M1_MISO_PIN   12

// Module 2 — VSPI bus (ESP32 default)
#define M2_CE_PIN     22
#define M2_CSN_PIN    5
#define M2_SCK_PIN    18
#define M2_MOSI_PIN   23
#define M2_MISO_PIN   19

// Status LED
#define STATUS_LED_PIN 2

// OLED SSD1306 I2C
#define OLED_SDA_PIN   4
#define OLED_SCL_PIN   17
#define OLED_ADDR      0x3C
#define OLED_W         128
#define OLED_H         64

// SPI clock for nRF24L01+ (datasheet max 8 MHz, safe ceiling 10 MHz)
#define NRF24_SPI_SPEED 8000000UL

// PLL settle time after RF_CH rewrite (datasheet ~130us, use 150 for margin)
#define PLL_SETTLE_US 150

// Scan: ms to listen per channel for RPD detection
#define SCAN_DWELL_MS  2

// ============================================================
// Serial
// ============================================================
#define SERIAL_BAUD 115200

// ============================================================
// WiFi Access Point
// ============================================================
#define WIFI_AP_SSID   "quiet-please"
#define WIFI_AP_PASS   "quiet123"
#define WIFI_CHANNEL   1        // ch1 = 2412 MHz — minimizes overlap with BT band edges
#define WEB_PORT       80

// Web Basic Auth
#define WEB_USER       "admin"
#define WEB_PASS       "quiet123"

// ============================================================
// Auto-stop timer (milliseconds). 0 = disabled.
// Default: 5 minutes fail-safe
// ============================================================
#define AUTO_STOP_DEFAULT_MS  300000

// ============================================================
// NVS
// ============================================================
#define NVS_NAMESPACE  "qprefs"

// ============================================================
// FreeRTOS
// ============================================================
#define JAMMER_TASK_CORE    1
#define JAMMER_TASK_STACK   4096
#define JAMMER_TASK_PRIO    2

// ============================================================
// nRF24L01+ Channel ↔ Frequency
//   frequency_MHz = 2400 + RF_CH   (RF_CH valid 0..125)
// ============================================================

// ============================================================
// Channel tables for each jamming mode (dual-module split)
// ============================================================

// --- BLE advertising channels (nRF24 ch = 2, 26, 80) ---
static const uint8_t BLE_ADV_M1[] = {2, 80};
static const uint8_t BLE_ADV_M2[] = {26};
#define BLE_ADV_M1_LEN (sizeof(BLE_ADV_M1)/sizeof(BLE_ADV_M1[0]))
#define BLE_ADV_M2_LEN (sizeof(BLE_ADV_M2)/sizeof(BLE_ADV_M2[0]))

// --- BLE full (37 data + 3 adv = 40 channels, even ch 0..80) ---
static const uint8_t BLE_FULL_M1[] = {
     0,  2,  4,  6,  8, 10, 12, 14, 16, 18,
    20, 22, 24, 26, 28, 30, 32, 34, 36, 38,
    40
};
static const uint8_t BLE_FULL_M2[] = {
    42, 44, 46, 48, 50, 52, 54, 56, 58, 60,
    62, 64, 66, 68, 70, 72, 74, 76, 78, 80
};
#define BLE_FULL_M1_LEN (sizeof(BLE_FULL_M1)/sizeof(BLE_FULL_M1[0]))
#define BLE_FULL_M2_LEN (sizeof(BLE_FULL_M2)/sizeof(BLE_FULL_M2[0]))

// --- BT Classic (79 channels, nRF24 ch 2..80) ---
#define BT_CLASSIC_CH_START 2
#define BT_CLASSIC_CH_END   80
#define BT_CLASSIC_M1_START 2
#define BT_CLASSIC_M1_END   41
#define BT_CLASSIC_M2_START 42
#define BT_CLASSIC_M2_END   80

// --- Scan range (BT channels 2..80 = 79 channels) ---
#define SCAN_CH_START  2
#define SCAN_CH_END    80
#define SCAN_NUM_CH    (SCAN_CH_END - SCAN_CH_START + 1)

// ============================================================
// TX Power levels
//   0 = PA_MIN (-18 dBm)   1 = PA_LOW (-12 dBm)
//   2 = PA_HIGH (-6 dBm)   3 = PA_MAX (  0 dBm)
// ============================================================
#define PA_LEVEL_MIN   0
#define PA_LEVEL_LOW   1
#define PA_LEVEL_HIGH  2
#define PA_LEVEL_MAX   3
#define PA_LEVEL_DEFAULT PA_LEVEL_MAX

#endif // CONFIG_H
