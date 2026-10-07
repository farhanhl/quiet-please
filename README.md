# quiet-please

Alat jamming Bluetooth 2.4 GHz berbasis **ESP32 + 2× EBYTE E01-ML01DP5** (nRF24L01+ + PA, 20 dBm / 100 mW).
Firmware berjalan di ESP32, mengendalikan dua modul nRF24 secara paralel (HSPI + VSPI), dan memancarkan **continuous carrier (CW)** yang di-sweep melalui channel Bluetooth Classic / BLE.

**Kontrol:** Serial (UART 115200) + Web UI (WiFi AP) + OLED display (opsional).
**Fitur:** Dual-core FreeRTOS, auto-stop timer, channel scan/RSSI, NVS persistence, web auth.

---

## 1. Daftar Alat & Bahan

| No | Komponen | Qty | Fungsi | Estimasi Harga |
|----|----------|-----|--------|----------------|
| 1 | **ESP32 DevKit v4 (WROOM-32)** | 1 | Microcontroller utama | Rp 60rb – 110rb |
| 2 | **EBYTE E01-ML01DP5** (nRF24L01+ + PA, 20 dBm, 100 mW) | 2 | Transceiver 2.4 GHz + Power Amplifier | Rp 50rb – 120rb /pcs |
| 3 | **Kapasitor elektrolit 100µF / 16V** | 2 | Bulk reservoir untuk spike TX PA (wajib untuk EBYTE) | Rp 500 – 2rb /pcs |
| 4 | **Kapasitor keramik 100nF (0.1µF)** | 2 | HF decoupling di pin VCC nRF24 | Rp 200 – 500 /pcs |
| 5 | **Jumper wire female-female** | 1 set | Wiring SPI + I2C | Rp 5rb – 15rb |
| 6 | **Kabel micro-USB / USB-C** (data-capable) | 1 | Power + flashing | Rp 8rb – 25rb |
| 7 | **Breadboard 830 titik** (opsional) | 1 | Prototyping | Rp 15rb – 30rb |
| 8 | **Antena 2.4 GHz SMA omni 3dBi** | 2 | Antena untuk modul EBYTE | Rp 5rb – 15rb /pcs |
| 9 | **OLED SSD1306 0.96" 128×64 I2C** (opsional) | 1 | Display status real-time | Rp 15rb – 30rb |

**Opsional upgrade:**

| No | Komponen | Fungsi | Estimasi Harga |
|----|----------|--------|----------------|
| 10 | **Antena panel / Yagi 2.4 GHz 12-15 dBi** (SMA) | Range jauh lebih luas | Rp 150rb – 500rb |
| 11 | **Regulator 3.3V eksternal (AMS1117-3.3)** | Kalau brownout tetap muncul | Rp 2rb |
| 12 | **Enclosure plastik** | Proteksi fisik | Rp 20rb – 60rb |

**Total minimum:** sekitar **Rp 180rb – 380rb** (di luar OLED dan antena upgrade).

### Link pembelian (Shopee.co.id)

- ESP32 DevKit v4 → https://shopee.co.id/search?keyword=esp32%20devkit%20v4%20wroom-32
- **EBYTE E01-ML01DP5** → https://shopee.co.id/NRF24L01-Ultra-Long-Range-2100M-E01-ML01DP5-Ebyte-20dBm-2-4Ghz-PA-LNA-i.2178321.8027887735
- Kapasitor elektrolit 100µF 16V → https://shopee.co.id/search?keyword=kapasitor%20elektrolit%20100uf%2016v
- Kapasitor keramik 100nF → https://shopee.co.id/search?keyword=kapasitor%20keramik%20100nf%20ceramic
- Jumper female-female → https://shopee.co.id/search?keyword=jumper%20wire%20female%20female
- Breadboard 830 → https://shopee.co.id/search?keyword=breadboard%20830%20point
- AMS1117-3.3 → https://shopee.co.id/search?keyword=ams1117-3.3%20regulator
- Antena 2.4 GHz SMA omni → https://shopee.co.id/search?keyword=antena%202.4ghz%20sma%20omni
- Antena 2.4 GHz panel/Yagi SMA → https://shopee.co.id/search?keyword=antena%202.4ghz%20sma%20router
- OLED SSD1306 0.96" I2C → https://shopee.co.id/search?keyword=oled%20ssd1306%200.96%20i2c

### Pilihan modul nRF24

**Rekomendasi utama: EBYTE E01-ML01DP5** (link di atas)
- TX power: **20 dBm (100 mW)** — jauh lebih kuat dari clone generik
- Chip Nordic nRF24L01+ asli — CW mode (`CONT_WAVE`) reliable
- PA engineering quality tinggi — output mendekati spec
- Current TX: ~130 mA per modul
- VCC: **3.3V** (jangan 5V — yang 5V itu E01-2G4M27D versi 500 mW)
- Pinout: 8-pin header sama persis dengan nRF24L01+ PA+LNA standar
- Firmware: **tidak perlu ubah kode** — register, SPI, library RF24 semua identik

**Alternatif budget: nRF24L01+ PA+LNA clone generik**
- TX power: ~5-10 dBm (jauh di bawah spec, PA clone sering underperform)
- CW mode tidak selalu berfungsi (clone ada yang tidak implement `CONT_WAVE` dengan benar)
- Harga lebih murah (~Rp 35-90rb) tapi hasil tidak predictable
- Cari di Shopee → https://shopee.co.id/search?keyword=nrf24l01%2B%20pa%2Blna%20antenna

**Yang harus dicek saat membeli:**
- **Wajib** modul dengan **PA+LNA** dan konektor **SMA** — bukan nRF24L01 plain (antena keramik trace PCB, range ~10m)
- Modul PA+LNA: PCB lebih besar, konektor SMA di pojok
- **Jangan beli "nRF24L01 plain"** — tidak cocok untuk project ini
- Untuk EBYTE: pastikan tulisan **"E01-ML01DP5"** di PCB. Hindari yang tidak ada label — kemungkinan clone pakai casing EBYTE palsu

---

## 2. Wiring

**ESP32 DevKit v4** ↔ **Modul 1 (HSPI)** ↔ **Modul 2 (VSPI)** ↔ **OLED (I2C)**

```
                  ┌────────── Modul 1 (HSPI) ──────────┐
                  │                                    │
ESP32 3.3V ──────┼──── VCC  (100µF + 0.1µF → GND)     │
ESP32 GND  ──────┼──── GND                              │
ESP32 GPIO16 ────┼──── CE                               │
ESP32 GPIO15 ────┼──── CSN                              │
ESP32 GPIO14 ────┼──── SCK                              │
ESP32 GPIO13 ────┼──── MOSI                             │
ESP32 GPIO12 ────┼──── MISO                             │
                  │       IRQ  (tidak dipakai)           │
                  └──────────────────────────────────────┘

                  ┌────────── Modul 2 (VSPI) ──────────┐
                  │                                    │
ESP32 3.3V ──────┼──── VCC  (100µF + 0.1µF → GND)     │
ESP32 GND  ──────┼──── GND                              │
ESP32 GPIO22 ────┼──── CE                               │
ESP32 GPIO5  ────┼──── CSN                              │
ESP32 GPIO18 ────┼──── SCK                              │
ESP32 GPIO23 ────┼──── MOSI                             │
ESP32 GPIO19 ────┼──── MISO                             │
                  │       IRQ  (tidak dipakai)           │
                  └──────────────────────────────────────┘

                  ┌────────── OLED SSD1306 (I2C) ──────┐
ESP32 3.3V ──────┼──── VCC                              │
ESP32 GND  ──────┼──── GND                              │
ESP32 GPIO4  ────┼──── SDA                              │
ESP32 GPIO17 ────┼──── SCL                              │
                  └──────────────────────────────────────┘
```

**Kapasitor 100µF wajib** dipasang langsung di pin VCC/GND modul EBYTE (solder ke header module, bukan ke breadboard). E01-ML01DP5 menarik ~130 mA per modul saat TX — 2 modul = ~260 mA. Tanpa cap 100µF, modul brownout saat TX burst.

**OLED opsional** — firmware auto-detect. Jika tidak ada OLED, boot tetap normal.

> **Polaritas:** kaki panjang kapasitor elektrolit = **(+)** = VCC. Kaki pendek = **(–)** = GND.

---

## 3. Build & Flash

### Prasyarat

- **PlatformIO** (`pip install -U platformio`) atau **PlatformIO extension di VS Code**.
- Driver CP2102/CH340 untuk USB-SERIAL ESP32.

### Perintah

```bash
cd quiet-please

# build
pio run

# flash (ganti COMx dengan port ESP32)
pio run -t upload --upload-port COM5

# monitor serial
pio device monitor -b 115200
```

### Identifikasi COM port

**Windows:** Device Manager → Ports (COM & LPT) → "Silicon Labs CP210x USB to UART Bridge (COMx)"
**Linux:** `ls /dev/ttyUSB*` atau `dmesg | tail` setelah colok USB.

---

## 4. Kontrol

Tiga cara kontrol — **Web UI**, **Serial**, dan **OLED** (display only). Semua berjalan bersamaan.

### Arsitektur Dual-Core

| Core | Task | Fungsi |
|------|------|--------|
| Core 0 | Arduino `loop()` | Web server + Serial + OLED update |
| Core 1 | `jammer_task` (FreeRTOS) | CW sweep loop — `tick()` tight loop |

Jammer sweep berjalan di core terpisah → web UI tetap responsive saat jamming aktif. Mutex FreeRTOS melindungi akses ke RF24 dan state variables.

### 4A. Web UI (via WiFi AP)

Setelah ESP32 boot, ia membuat WiFi Access Point.

1. Buka WiFi di HP/laptop
2. Connect ke SSID **`quiet-please`**, password **`quiet123`**
3. Buka browser → **`http://192.168.4.1`**
4. Login: user **`admin`**, password **`quiet123`**

Halaman web menampilkan:
- **Mode buttons**: BLE ADV / BLE FULL / BT CLASSIC / CW SINGLE
- **Channel slider**: 0–125 dengan live MHz display (untuk CW mode)
- **Power buttons**: -18 / -12 / -6 / 0 dBm
- **Auto-stop timer**: input detik (0 = off, default 300 detik / 5 menit)
- **START / STOP**: tombol besar
- **SCAN CHANNELS**: scan aktivitas RF di semua channel BT
- **Status panel**: mode, active, channel, power, module M1/M2, sweep count, timer remaining (auto-refresh tiap 1 detik)
- **Status dot**: hijau = jamming active, merah = idle
- **Scan results**: bar chart per channel (hijau = sinyal terdeteksi > -64 dBm)

**REST API:**

| Endpoint | Params | Response |
|----------|--------|----------|
| `/api/status` | — | JSON status |
| `/api/mode?m=1` | `m` = 0-5 | JSON status |
| `/api/channel?c=40` | `c` = 0-125 | JSON status |
| `/api/power?p=3` | `p` = 0-3 | JSON status |
| `/api/timer?t=300000` | `t` = ms (0=off) | JSON status |
| `/api/start` | — | JSON status |
| `/api/stop` | — | JSON status |
| `/api/scan` | — | JSON status (trigger scan) |
| `/api/scanresults` | — | JSON array hasil scan |
| `/api/save` | — | "saved" |

Semua endpoint memerlukan HTTP Basic Auth (`admin` / `quiet123`).

Contoh JSON status:
```json
{"mode":1,"active":true,"cw_channel":40,"pa_level":3,"m1_ok":true,"m2_ok":true,
 "sweeps":15234,"auto_stop_ms":300000,"elapsed_ms":45000,"remaining_ms":255000}
```

**Ubah WiFi/auth settings** — edit di `include/config.h`:
```c
#define WIFI_AP_SSID   "quiet-please"
#define WIFI_AP_PASS   "quiet123"     // WiFi AP password
#define WEB_USER       "admin"        // Web Basic Auth user
#define WEB_PASS       "quiet123"     // Web Basic Auth pass
```

### 4B. Kontrol via Serial

Buka **serial monitor 115200 baud**, ketik command lalu Enter.

| Command | Argumen | Fungsi |
|---------|---------|--------|
| `help` | — | Daftar command |
| `status` | — | Mode, power, channel, module, sweep, timer |
| `mode` | `0`-`5` atau nama | IDLE / BLE_ADV / BLE_FULL / BT_CLASSIC / CW / SCAN |
| `channel` | `0-125` | Channel target untuk CW (freq = 2400+ch MHz) |
| `power` | `0-3` | TX power: 0=−18dBm, 1=−12, 2=−6, 3=0dBm |
| `timer` | `<seconds>` | Auto-stop timer (0=off) |
| `start` | — | Mulai jamming |
| `stop` | — | Stop jamming |
| `scan` | — | Scan semua channel BT untuk aktivitas RF |
| `save` | — | Simpan settings ke NVS (survive reboot) |
| `reset` | — | Soft reboot ESP32 |

**Contoh sesi serial:**

```
quiet-please // nRF24 dual-module BT jammer
type `help` for commands
AP SSID : quiet-please
AP IP   : 192.168.4.1
Web UI  : http://192.168.4.1
Auth    : admin / quiet123
---- status ----
mode       : IDLE
active     : no
cw_channel : 40 (2440 MHz)
pa_level   : 3
auto_stop  : 300 sec
module M1  : OK (HSPI)
module M2  : OK (VSPI)
sweeps     : 0
----------------

> mode 1
mode = BLE_ADV

> timer 120
auto_stop = 120 sec

> start
started

> status
---- status ----
mode       : BLE_ADV
active     : YES
remaining  : 118 sec
sweeps     : 8421
----------------

> scan
scanning...
---- scan results ----
ch  freq  M1 M2
  2 2402  DET  ---
  3 2403  ---  ---
  ...
 80 2480  DET  DET
----------------------

> save
settings saved to NVS
```

### 4C. OLED Display (opsional)

Jika SSD1306 terpasang (I2C GPIO4/GPIO17), display menampilkan:
- Mode singkat + status dot (active/idle)
- Power level + CW channel + frekuensi
- Status module M1/M2
- Sweep count
- Timer countdown (saat active)
- Bottom bar terang saat jamming active

Update otomatis setiap 250ms. Jika OLED tidak terpasang, firmware skip tanpa error.

---

## 5. Mode Jamming

| # | Nama | Channel target | Efek |
|---|------|---------------|------|
| 1 | **BLE_ADV** | {2, 26, 80} = BLE ch 37/38/39 | Mencegah BLE discover/pair |
| 2 | **BLE_FULL** | even ch 0..80 (40 channel) | Jam lengkap BLE (adv + data) |
| 3 | **BT_CLASSIC** | ch 2..80 (79 channel) | Jam speaker/headset Classic BT |
| 4 | **CW_SINGLE** | channel user (0..125) | Carrier kontinu di 1 frekuensi |
| 5 | **SCAN** | ch 2..80 (79 channel) | RX mode — deteksi aktivitas RF (RPD > -64dBm) |

**Split dual-module** di `include/config.h`:

| Mode | Modul 1 (HSPI) | Modul 2 (VSPI) |
|------|----------------|----------------|
| BLE_ADV | {2, 80} | {26} |
| BLE_FULL | even 0..40 (21 ch) | even 42..80 (20 ch) |
| BT_CLASSIC | 2..41 (40 ch) | 42..80 (39 ch) |
| CW_SINGLE | user ch | user ch |
| SCAN | sweep 2..80 | sweep 2..80 |

---

## 6. Fitur Tambahan

### Auto-Stop Timer

Fail-safe yang otomatis menghentikan jamming setelah waktu yang ditentukan.
- Default: **300 detik** (5 menit)
- Set via web UI (input seconds) atau serial (`timer <seconds>`)
- `0` = disabled (jamming tanpa batas waktu)
- Timer reset setiap kali `start` dipanggil

### Channel Scan / RSSI

Mode SCAN menggunakan **RPD (Received Power Detector)** nRF24L01+ untuk mendeteksi sinyal di atas **-64 dBm** di setiap channel.
- Men-switch radio ke RX mode (bukan TX)
- Sweep ch 2..80 (79 channel BT), ~2ms per channel
- Total scan time: ~160ms
- Hasil: per channel, M1 dan M2 masing-masing — detected atau not
- Tampil di web UI sebagai bar chart, atau di serial sebagai tabel

### NVS Persistence

Settings disimpan ke flash ESP32 (NVS — Non-Volatile Storage):
- Mode, power level, CW channel, auto-stop timer
- `save` command (serial) atau `/api/save` (web) untuk menyimpan
- Auto-load saat boot
- Settings survive reboot / power loss

### Web Authentication

- WiFi AP password: `quiet123` (configurable)
- HTTP Basic Auth di semua API endpoints: `admin` / `quiet123`
- Mencegah akses tidak sah dari orang dalam range WiFi

### Dual-Core FreeRTOS

- **Core 1:** Jammer sweep task — tight loop, prioritized
- **Core 0:** Web server + Serial + OLED — tetap responsive saat jamming
- FreeRTOS mutex melindungi RF24 SPI access dan state variables

---

## 7. Prinsip Kerja

1. **CW mode** nRF24 diaktifkan dengan set bit `CONT_WAVE` (bit 7) + `PLL_LOCK` (bit 4) di register `RF_SETUP` (0x06).
2. Carrier terus-menerus dipancarkan; untuk **sweep**, rewrite register `RF_CH` — PLL re-lock otomatis (~130 µs).
3. RF24 library `startConstCarrier()` / `stopConstCarrier()` melakukan init + dummy payload + CE high.
4. `setChannel()` pindah carrier tanpa restart.
5. **Scan mode:** `startListening()` + `testRPD()` per channel.
6. **Auto-stop:** `millis()` check di `tick()`.
7. **NVS:** `Preferences.h` (ESP32 Arduino core built-in).

---

## 8. Range & Antenna

| Setup | Range efektif |
|-------|---------------|
| E01-ML01DP5 (20 dBm) + antena rubber duck 3dBi | 100 – 300 m (LoS) |
| + antena panel/Yagi 12 dBi | 500 m – 1.5 km (LoS) |
| + antena Yagi 15 dBi | 1 – 3 km+ (LoS) |
| Clone generik (5-10 dBm) + antena rubber duck | 30 – 100 m (LoS) |

LoS = line-of-sight. Dinding, lantai, dan air menurunkan drastis.

**Penting:** E01-ML01DP5 output 20 dBm (100 mW) — **jangan nyalakan tanpa antena**. RF power tanpa beban akan merusak PA. Selalu pasang antena SMA 2.4 GHz atau dummy load 50Ω sebelum START.

---

## 9. Troubleshooting

| Gejala | Kemungkinan | Solusi |
|--------|-------------|--------|
| `module M1 : MISSING` | Modul 1 tidak init | Cek wiring SPI (SCK/MISO/MOSI/CSN), cek kapasitor 100µF di VCC-GND modul |
| Kedua-dua `MISSING` | CSN pin conflict / brownout | CSN M2=GPIO5, CSN M1=GPIO15. Pastikan 3.3V cukup (multimeter di VCC pin saat TX). Coba regulator eksternal AMS1117-3.3 |
| Status `0x00` atau `0xFF` | SPI kabel salah / chip rusak | Double-check MISO→GPIO12(M1)/GPIO19(M2), SCK→GPIO14(M1)/GPIO18(M2) |
| Jamming berhenti acak | Brownout saat TX burst | E01-ML01DP5 draw ~130mA/modul. Pastikan cap 100µF + 0.1µF di setiap modul. Kalau masih brownout → regulator eksternal AMS1117-3.3 |
| Jamming auto-stop | Auto-stop timer aktif | Default 300 detik. Set `timer 0` untuk disable, atau `timer <seconds>` untuk ubah |
| Serial tidak ada output | Salah baud / port | Pastikan 115200 baud, tekan EN/RST saat monitor dibuka |
| Upload gagal | Mode download belum masuk | Tahan **BOOT** sambil tekan **EN**, lepas BOOT setelah "Connecting..." |
| OLED blank / tidak tampil | Salah I2C addr / wiring | Cek SDA=GPIO4, SCL=GPIO17. Coba addr 0x3D (beberapa modul). Cek VCC/GND |
| Web UI tidak bisa diakses | Salah password / belum connect AP | SSID: `quiet-please`, pass: `quiet123`. Login: `admin`/`quiet123` |
| Scan hasil kosong | Tidak ada sinyal BT aktif | Pastikan ada perangkat BT aktif dalam range. RPD threshold = -64 dBm |

---

## 10. Catatan Operasional

Perangkat ini **memancarkan interferensi RF aktif** di band ISM 2.4 GHz dengan output **20 dBm (100 mW)** per modul (E01-ML01DP5).

- **Gunakan di dalam enclosure Faraday / ruang shielded** atau jauh dari perangkat elektronik lain.
- **Antena mengarah ke beban dummy** (50Ω dummy load) saat debugging firmware.
- **Jangan dinyalakan tanpa antena / dummy load** — 20 dBm tanpa beban akan merusak PA dalam hitungan detik.
- **VCC 3.3V saja** — jangan kasih 5V ke E01-ML01DP5. Yang 5V itu E01-2G4M27D (versi 500 mW, pinout berbeda).

Status LED onboard ESP32 (GPIO2) menyala saat jamming aktif. OLED display juga menampilkan status real-time.

---

## 11. Struktur Project

```
quiet-please/
├── platformio.ini          # build config (ESP32 + RF24 + Adafruit SSD1306)
├── include/
│   └── config.h            # pin defs, channel arrays, WiFi AP, OLED, timer, auth
├── src/
│   ├── main.cpp            # setup() + loop() + FreeRTOS jammer task
│   ├── jammer.h            # Jammer class interface (mode, mutex, scan, timer, NVS)
│   ├── jammer.cpp          # CW sweep, scan, auto-stop, NVS persistence, mutex
│   ├── serial_control.h    # SerialControl class interface
│   ├── serial_control.cpp  # command parser (mode/channel/power/timer/scan/save)
│   ├── web_control.h       # WebControl class interface
│   ├── web_control.cpp     # WiFi AP + WebServer + HTML + REST API + auth
│   ├── display.h           # Display class interface (OLED)
│   └── display.cpp         # SSD1306 I2C — status real-time display
└── README.md               # file ini
```

---

## 12. Kustomisasi Cepat

**Tambah mode baru** — edit `include/config.h` (channel array) + tambahkan case di `Jammer::m1_channel_now()` / `m2_channel_now()` di `src/jammer.cpp`.

**Ubah pin** — ganti `M1_*` / `M2_*` / `OLED_*` di `include/config.h`. Hindari GPIO 6-11 (flash SPI internal).

**Ubah settling time** — `PLL_SETTLE_US` di `include/config.h` (default 150 µs).

**Ubah auto-stop default** — `AUTO_STOP_DEFAULT_MS` di `include/config.h` (default 300000 = 5 menit).

**Ubah scan dwell time** — `SCAN_DWELL_MS` di `include/config.h` (default 2ms per channel).

**Ubah WiFi/AP/auth** — `WIFI_AP_SSID`, `WIFI_AP_PASS`, `WEB_USER`, `WEB_PASS` di `include/config.h`.

**Disable OLED** — hapus atau comment `#include "display.h"` di `main.cpp` dan baris `oled.begin()` / `oled.update()`. Atau cukup tidak pasang OLED — firmware auto-detect.

**Tambah mode WiFi 2.4 GHz** — channel array WiFi center sudah ada di referensi project `huuck/FlipperZeroNRFJammer`; tinggal duplikat pola array di `config.h` dan tambahkan case.

**Upgrade ke E01-2G4M27D (500 mW / 27 dBm)** — pinout berbeda (butuh VCC 5V untuk PA), current TX ~500 mA. Butuh regulator 5V eksternal + level shifter SPI (ESP32 3.3V, modul 5V tolerant di data pin tapi VCC harus 5V). Firmware tidak berubah — register map sama.
