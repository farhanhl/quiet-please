# quiet-please

Alat jamming Bluetooth 2.4 GHz berbasis **ESP32 + 2× nRF24L01+ PA+LNA**.
Firmware berjalan di ESP32, mengendalikan dua modul nRF24 secara paralel (HSPI + VSPI), dan memancarkan **continuous carrier (CW)** yang di-sweep melalui channel Bluetooth Classic / BLE.

Kontrol lewat **Serial (UART 115200)** dan **Web UI** (WiFi AP — buka browser di HP).

---

## 1. Daftar Alat & Bahan

| No | Komponen | Qty | Fungsi | Estimasi Harga |
|----|----------|-----|--------|----------------|
| 1 | **ESP32 DevKit v4 (WROOM-32)** | 1 | Microcontroller utama | Rp 60rb – 110rb |
| 2 | **nRF24L01+ PA+LNA** (dengan SMA antenna) | 2 | Transceiver 2.4 GHz + Power Amplifier | Rp 35rb – 90rb /pcs |
| 3 | **Kapasitor elektrolit 10µF / 25V** (atau 100µF) | 2 | Bulk reservoir untuk spike TX PA | Rp 500 – 2rb /pcs |
| 4 | **Kapasitor keramik 100nF (0.1µF)** | 2 | HF decoupling di pin VCC nRF24 | Rp 200 – 500 /pcs |
| 5 | **Jumper wire female-female** | 1 set | Wiring SPI | Rp 5rb – 15rb |
| 6 | **Kabel micro-USB / USB-C** (data-capable) | 1 | Power + flashing | Rp 8rb – 25rb |
| 7 | **Breadboard 830 titik** (opsional) | 1 | Prototyping | Rp 15rb – 30rb |
| 8 | **Antena 2.4 GHz SMA omni 3dBi** (bawaan modul) | sudah termasuk | — | — |

**Opsional upgrade:**

| No | Komponen | Fungsi | Estimasi Harga |
|----|----------|--------|----------------|
| 9 | **Antena panel / Yagi 2.4 GHz 12-15 dBi** (SMA) | Range jauh lebih luas | Rp 150rb – 500rb |
| 10 | **Regulator 3.3V eksternal (AMS1117-3.3)** | Kalau brownout tetap muncul | Rp 2rb |
| 11 | **Enclosure plastik** | Proteksi fisik | Rp 20rb – 60rb |

**Total minimum:** sekitar **Rp 120rb – 260rb** (di luar antena upgrade).

### Link pembelian (Shopee.co.id)

Semua link adalah halaman pencarian keyword — filter "Terlaris" atau rating tinggi saat memilih toko.

- ESP32 DevKit v4 → https://shopee.co.id/search?keyword=esp32%20devkit%20v4%20wroom-32
- nRF24L01+ PA+LNA → https://shopee.co.id/search?keyword=nrf24l01%2B%20pa%2Blna%20antenna
- Kapasitor elektrolit 10µF 25V → https://shopee.co.id/search?keyword=kapasitor%20elektrolit%2010uf%2025v
- Kapasitor keramik 100nF → https://shopee.co.id/search?keyword=kapasitor%20keramik%20100nf%20ceramic
- Jumper female-female → https://shopee.co.id/search?keyword=jumper%20wire%20female%20female
- Breadboard 830 → https://shopee.co.id/search?keyword=breadboard%20830%20point
- AMS1117-3.3 → https://shopee.co.id/search?keyword=ams1117-3.3%20regulator
- Antena 2.4 GHz panel/Yagi SMA → https://shopee.co.id/search?keyword=antena%202.4ghz%20sma%20router

### Yang harus dicek saat membeli nRF24

- **Wajib** modul dengan **PA+LNA** (biasanya pakai chip RFX2401C, punya konektor **SMA** — bukan modul nRF24 plain yang pakai antena keramik trace PCB).
- Modul PA+LNA ditandai dengan PCB yang lebih besar, konektor antenna SMA di pojok, dan ada tulisan "PA+LNA" atau "nRF24L01+PA+LNA".
- **Jangan beli "nRF24L01 plain"** — range hanya ~10m, tidak cocok untuk project ini.
- Alternatif yang lebih kuat: **EBYTE E01-ML01DP5** atau **E01-2G4M27D** (500mW). Cari di Shopee dengan keyword `E01-ML01DP5`.

---

## 2. Wiring

**ESP32 DevKit v4** ↔ **Modul 1 (HSPI)** ↔ **Modul 2 (VSPI)**

```
                 ┌────────── Modul 1 (HSPI) ──────────┐
                 │                                    │
ESP32 3.3V ──────┼──── VCC  (10µF + 0.1µF → GND)      │
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
ESP32 3.3V ──────┼──── VCC  (10µF + 0.1µF → GND)      │
ESP32 GND  ──────┼──── GND                              │
ESP32 GPIO22 ────┼──── CE                               │
ESP32 GPIO5  ────┼──── CSN                              │
ESP32 GPIO18 ────┼──── SCK                              │
ESP32 GPIO23 ────┼──── MOSI                             │
ESP32 GPIO19 ────┼──── MISO                             │
                 │       IRQ  (tidak dipakai)           │
                 └──────────────────────────────────────┘
```

**Kapasitor wajib dipasang langsung di pin VCC/GND modul nRF24** (solder ke header module, bukan ke breadboard). Ini bukan opsional — tanpa kapasitor, modul sering gagal init atau browout saat TX.

> **Polaritas:** kaki panjang kapasitor elektrolit = **(+)** = VCC. Kaki pendek = **(–)** = GND. Terbalik = kapasitor bocor/pecah.

---

## 3. Build & Flash

### Prasyarat

- **PlatformIO** (`pip install -U platformio`) atau **PlatformIO extension di VS Code**.
- Driver CP2102/CH340 untuk USB-SERIAL ESP32 (bawaan DevKit v4 pakai CP2102).

### Perintah

```bash
# clone / masuk ke folder project
cd quiet-please

# build
pio run

# flash (ganti COMx dengan port ESP32 anda)
pio run -t upload --upload-port COM5

# monitor serial
pio device monitor -b 115200
```

Alternatif VS Code: install extension **PlatformIO IDE**, buka folder project, klik ✔ (Build) lalu → (Upload), lalu Monitor.

### Identifikasi COM port

**Windows:** Device Manager → Ports (COM & LPT) → "Silicon Labs CP210x USB to UART Bridge (COMx)"
**Linux:** `ls /dev/ttyUSB*` atau `dmesg | tail` setelah colok USB.

---

## 4. Kontrol

Ada dua cara kontrol — **Serial** dan **Web UI**. Keduanya berjalan bersamaan, mengakses engine jammer yang sama. Tidak saling konflik.

### 4A. Web UI (via WiFi AP)

Setelah ESP32 boot, ia membuat WiFi Access Point sendiri.

1. Buka WiFi di HP/laptop
2. Connect ke SSID **`quiet-please`** (open, tanpa password)
3. Buka browser → **`http://192.168.4.1`**

Halaman web menampilkan:
- **Mode buttons**: BLE ADV / BLE FULL / BT CLASSIC / CW SINGLE
- **Channel slider**: 0–125 dengan live MHz display (untuk CW mode)
- **Power buttons**: -18 / -12 / -6 / 0 dBm
- **START / STOP**: tombol besar
- **Status panel**: mode, active, channel, power, module M1/M2, sweep count (auto-refresh tiap 1 detik)
- **Status dot**: hijau = jamming active, merah = idle

UI mobile-friendly, dark theme. Tidak butuh internet — ESP32 AP self-contained.

> Saat HP connect ke AP `quiet-please`, HP tidak punya internet (kecuali mobile data aktif). Ini normal.

**REST API** (jika ingin kontrol dari script/custom UI):

| Endpoint | Method | Params | Response |
|----------|--------|--------|----------|
| `/api/status` | GET | — | JSON status |
| `/api/mode?m=1` | GET | `m` = 0-4 | JSON status |
| `/api/channel?c=40` | GET | `c` = 0-125 | JSON status |
| `/api/power?p=3` | GET | `p` = 0-3 | JSON status |
| `/api/start` | GET | — | JSON status |
| `/api/stop` | GET | — | JSON status |

Contoh JSON status:
```json
{"mode":1,"active":true,"cw_channel":40,"pa_level":3,"m1_ok":true,"m2_ok":true,"sweeps":15234}
```

**Ubah SSID/password AP** — edit di `include/config.h`:
```c
#define WIFI_AP_SSID   "quiet-please"
#define WIFI_AP_PASS   ""        // kosong = open; isi untuk password
#define WIFI_CHANNEL   6         // WiFi channel 1-13
```

### 4B. Kontrol via Serial

Buka **serial monitor 115200 baud**, ketik command lalu Enter.

| Command | Argumen | Fungsi |
|---------|---------|--------|
| `help` | — | Daftar command |
| `status` | — | Tampilkan mode, power, channel, module, sweep count |
| `mode` | `0`/`1`/`2`/`3`/`4` atau nama | Set jamming mode |
| `channel` | `0-125` | Channel target untuk mode `CW` (freq = 2400+ch MHz) |
| `power` | `0-3` | TX power: 0=−18dBm, 1=−12, 2=−6, 3=0dBm |
| `start` | — | Mulai jamming |
| `stop` | — | Stop jamming |
| `reset` | — | Soft reboot ESP32 |

**Contoh sesi serial:**

```
quiet-please // nRF24 dual-module BT jammer
type `help` for commands
AP SSID : quiet-please
AP IP   : 192.168.4.1
Web UI  : http://192.168.4.1
---- status ----
mode       : IDLE
active     : no
cw_channel : 40 (2440 MHz)
pa_level   : 3
module M1  : OK (HSPI)
module M2  : OK (VSPI)
sweeps     : 0
----------------

> mode 1
mode = BLE_ADV

> start
started

> stop
stopped
```

---

## 5. Mode Jamming

| # | Nama | Channel target | Efek |
|---|------|---------------|------|
| 1 | **BLE_ADV** | {2, 26, 80} = BLE ch 37/38/39 | Mencegah perangkat BLE discover/pair. Cukup untuk membuat earbud baru tidak bisa pairing, namun koneksi aktif mungkin tetap hidup (BLE hopping). |
| 2 | **BLE_FULL** | even ch 0..80 (40 channel = semua BLE) | Jam lengkap untuk seluruh BLE stack (advertising + data). Koneksi aktif juga terputus. |
| 3 | **BT_CLASSIC** | ch 2..80 (79 channel = semua BT Classic) | Jam speaker/headset/kb Classic BT. |
| 4 | **CW_SINGLE** | channel user (0..125) | Carrier kontinu di satu frekuensi. Paling efektif untuk target spesifik (mis. satu BT channel aktif). |

**Split dual-module** (Modul 1 / Modul 2) sudah dihardcode di `include/config.h`:

| Mode | Modul 1 (HSPI) | Modul 2 (VSPI) |
|------|----------------|----------------|
| BLE_ADV | {2, 80} | {26} |
| BLE_FULL | even 0..40 (21 ch) | even 42..80 (20 ch) |
| BT_CLASSIC | 2..41 (40 ch) | 42..80 (39 ch) |
| CW_SINGLE | user ch | user ch (kedua modul di freq sama) |

---

## 6. Prinsip Kerja

1. **CW mode** nRF24 diaktifkan dengan set bit `CONT_WAVE` (bit 7) + `PLL_LOCK` (bit 4) di register `RF_SETUP` (0x06).
2. Carrier terus-menerus dipancarkan; untuk **sweep**, cukup rewrite register `RF_CH` — PLL re-lock otomatis (~130 µs).
3. RF24 library punya helper `startConstCarrier()` / `stopConstCarrier()` yang melakukan init + dummy payload write + CE high sesuai datasheet.
4. `setChannel()` dipakai untuk pindah carrier ke freq berikutnya tanpa restart.

Timing per tick di `Jammer::tick()`: `setChannel(M1)` + `setChannel(M2)` + `delayMicroseconds(150)`. Total duty cycle per channel ≈ 150 µs.

---

## 7. Range & Antenna

| Setup | Range efektif |
|-------|---------------|
| Modul PA+LNA + antena rubber duck bawaan | 50 – 200 m (LoS) |
| + antena panel/Yagi 12 dBi | 500 m – 1 km (LoS) |
| + antena Yagi 15 dBi, TX 0 dBm | 1 – 2 km+ (LoS) |

LoS = line-of-sight. Dinding, lantai, dan air menurunkan drastis.

**Penting:** modul PA+LNA punya chip **RFX2401C** di depan antenna SMA. Kalau salah beli "nRF24L01+ plain" (PCB kecil, antenna trace keramik), range hanya ~10 m dan PA register tidak berpengaruh.

---

## 8. Troubleshooting

| Gejala | Kemungkinan | Solusi |
|--------|-------------|--------|
| `module M1 : MISSING` di status | Modul 1 tidak init | Cek wiring SPI (SCK/MISO/MOSI/CSN), cek kapasitor 10µF di VCC-GND modul, cek polaritas kapasitor elektrolit |
| Kedua-dua `MISSING` | CSN pin conflict / regulator brownout | CSN M2 = GPIO5, CSN M1 = GPIO15 (bukan GPIO 5 dua-duanya). Pastikan 3.3V cukup (cek pakai multimeter). Coba power nRF24 dari sumber 3.3V eksternal |
| Status baca `0x00` atau `0xFF` | SPI kable salah / chip rusak | Double-check MISO ke GPIO12 (M1) / GPIO19 (M2), dan SCK ke GPIO14 (M1) / GPIO18 (M2) |
| Jamming berhenti acak | Brownout saat TX burst | Naikkan kapasitor ke 100µF, tambahkan kapasitor keramik 0.1µF sedekat mungkin ke pin VCC modul |
| Serial tidak ada output | Salah baud / port | Pastikan 115200 baud, port yang benar, tekan tombol EN/RST di DevKit saat monitor dibuka |
| Upload gagal | Mode download belum masuk | Tahan tombol **BOOT** sambil tekan **EN**, lepas BOOT setelah "Connecting..." muncul |

---

## 9. Catatan Operasional

Perangkat ini **memancarkan interferensi RF aktif** di band ISM 2.4 GHz. Untuk penggunaan yang aman dan terkontrol:

- **Gunakan di dalam enclosure Faraday / ruang shielded** atau jauh dari perangkat elektronik lain.
- **Antena mengarah ke beban dummy** (50Ω dummy load) saat debugging firmware agar tidak memancar ke lingkungan.
- **Jangan dinyalakan tanpa antena / dummy load** — RF power akan memantul ke PA dan merusak modul.

Status LED onboard ESP32 (GPIO2) menyala saat jamming aktif — gunakan sebagai indikator visual.

---

## 10. Struktur Project

```
quiet-please/
├── platformio.ini          # build config
├── include/
│   └── config.h            # pin defs, channel arrays, WiFi AP, constants
├── src/
│   ├── main.cpp            # setup() + loop()
│   ├── jammer.h            # Jammer class interface
│   ├── jammer.cpp          # CW start/stop, dual-module sweep
│   ├── serial_control.h    # SerialControl class interface
│   ├── serial_control.cpp  # command parser + status printer
│   ├── web_control.h       # WebControl class interface
│   └── web_control.cpp     # WiFi AP + WebServer + HTML + REST API
└── README.md               # file ini
```

---

## 11. Kustomisasi Cepat

**Tambah channel atau mode baru** — edit `include/config.h` dan tambahkan case di `Jammer::m1_channel_now()` / `Jammer::m2_channel_now()` di `src/jammer.cpp`.

**Ubah pin** — ganti `M1_*` / `M2_*` di `include/config.h`. Pastikan pin HSPI/VSPI yang dipilih tidak conflict dengan flash SPI internal (hindari GPIO 6-11).

**Ubah settling time** — `PLL_SETTLE_US` di `include/config.h` (default 150 µs; datasheet minimal 130 µs).

**Tambah mode WiFi 2.4 GHz** — channel array WiFi center sudah ada di referensi project `huuck/FlipperZeroNRFJammer`; tinggal duplikat pola array di `config.h` dan tambahkan case.
