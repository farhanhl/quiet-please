# Tutorial: Rakit quiet-please dari Awal sampai Bisa Digunakan

Tutorial ini untuk pemula. Ikuti urutan langkah dari atas ke bawah. Jangan skip.

---

## Langkah 1: Beli Komponen

### Wajib (tanpa ini tidak bisa jalan)

| Komponen | Jumlah | Beli Dimana |
|---|---|---|
| ESP32 DevKit v4 (38-Pin, CP2102) | 1 | [Shopee (Pilih 38-pin CP2102)](https://shopee.co.id/search?keyword=esp32%20devkit%20v4%2038%20pin%20cp2102) |
| EBYTE E01-ML01DP5 (20 dBm) | 2 | [Shopee](https://shopee.co.id/NRF24L01-Ultra-Long-Range-2100M-E01-ML01DP5-Ebyte-20dBm-2-4Ghz-PA-LNA-i.2178321.8027887735) |
| Kapasitor elektrolit 100µF | 2 | [Shopee](https://shopee.co.id/search?keyword=kapasitor%20elektrolit%20100uf) |
| Kapasitor keramik 100nF | 2 | [Shopee](https://shopee.co.id/search?keyword=kapasitor%20keramik%20100nf) |
| Jumper wire female-female | 1 set | [Shopee](https://shopee.co.id/search?keyword=jumper%20wire%20female%20female) |
| Kabel USB (data, bukan cuma charging) | 1 | [Shopee](https://shopee.co.id/search?keyword=kabel%20micro%20usb%20data) |
| Antena 2.4 GHz SMA | 2 | [Shopee](https://shopee.co.id/search?keyword=antena%202.4ghz%20sma%20omni) |

### Opsional (bagus punya tapi tidak wajib)

| Komponen | Jumlah | Untuk Apa |
|---|---|---|
| OLED SSD1306 0.96" I2C | 1 | Lihat status tanpa buka HP |
| Breadboard 830 titik | 1 | Rakit tanpa solder |
| Regulator AMS1117-3.3 | 1 | Kalau power tidak stabil |

### Alat rakit

| Alat | Untuk Apa | Beli Dimana |
|---|---|---|
| Solder iron 30-40W + timah | Solder kapasitor ke modul (8 titik, 10 menit) | [Shopee](https://shopee.co.id/search?keyword=solder%20iron%2030w) |
| Multimeter | Cek voltage 3.3V, continuity (opsional tapi disarankan) | [Shopee](https://shopee.co.id/search?keyword=multimeter%20digital%20murah) |

> **Penting soal kabel USB:** Banyak kabel USB murah cuma untuk charging, tidak bisa transfer data. Test: colok ke HP/komputer, kalau tidak muncul sebagai device → kabelnya cuma charging. Beli yang tulisan "data sync" atau "data cable".

---

## Langkah 2: Siapkan Alat

Alat yang dibutuhkan untuk merakit:

1. **Solder iron** + timah — untuk solder kapasitor ke modul EBYTE
2. **Multimeter** — untuk cek voltage (opsional tapi sangat disarankan)
3. **Komputer/Laptop** — Windows, Mac, atau Linux
4. **Koneksi internet** — untuk download software

---

## Soal Solder: Kenapa Perlu, Berapa Banyak, dan Alternatifnya

### Kenapa perlu solder?

Solder cuma dipakai untuk **satu hal**: memasang kapasitor 100µF + 100nF ke pin VCC/GND modul EBYTE. Itu saja.

**Alasan teknisnya:**

EBYTE menarik ~130 mA per modul saat transmit (2 modul = ~260 mA). Lonjakan arus ini butuh kapasitor yang terpasang **sesingkat mungkin** ke pin modul — idealnya < 5 mm.

Kalau kapasitor dipasang di breadboard dengan jumper wire di tengahnya:
- Terlalu banyak resistance + inductance di jalur
- Voltage drop saat TX burst → modul brownout → reset
- Hasilnya: jamming berhenti acak, modul kadang OK kadang MISSING

Solder langsung ke pin modul = jarak < 5 mm = stabil.

### Berapa banyak titik solder?

Sangat sedikit:

| Pekerjaan | Titik solder | Waktu |
|---|---|---|
| Kapasitor 100µF → VCC + GND (modul 1) | 2 titik | ~2 menit |
| Kapasitor 100nF → VCC + GND (modul 1) | 2 titik | ~1 menit |
| Kapasitor 100µF → VCC + GND (modul 2) | 2 titik | ~2 menit |
| Kapasitor 100nF → VCC + GND (modul 2) | 2 titik | ~1 menit |
| **Total** | **8 titik** | **~10 menit** |

Wiring ESP32 ↔ modul ↔ OLED **tidak perlu solder sama sekali** — semua pakai jumper wire female-female yang dicolok ke breadboard.

### Yang TIDAK perlu solder:

- ❌ Wiring ESP32 ↔ modul EBYTE → jumper wire female-female
- ❌ Wiring ESP32 ↔ OLED → jumper wire
- ❌ Antena → screw/SMA connector, tinggal pasang
- ❌ ESP32 ke breadboard → tinggal ditancapkan

### Beli solder iron (kalau belum punya)

Cukup solder iron murah untuk pemula:

| Item | Harga | Catatan |
|---|---|---|
| Solder iron 30-40W | Rp 15rb – 40rb | Model pensil biasa cukup |
| Timah solder 0.8-1mm | Rp 5rb – 15rb | Beli yang sudah ada flux core |
| Solder stand/holder | Rp 5rb – 15rb | Opsional tapi aman |

Link Shopee:
- Solder iron → https://shopee.co.id/search?keyword=solder%20iron%2030w%20pistol
- Timah solder → https://shopee.co.id/search?keyword=timah%20solder%200.8mm%20flux

> Total biaya tambahan ~Rp 25rb – 70rb. Ini investasi yang worth it — hasil rakitan jauh lebih reliable.

### Alternatif tanpa solder (untuk testing saja)

Kalau tidak mau beli solder iron, ada 2 opsi — tapi pahami konsekuensinya:

**Opsi 1: Kapasitor di breadboard dekat modul**

1. Masukkan pin VCC/GND modul ke breadboard
2. Pasang kapasitor di breadboard tepat sebelah pin VCC/GND
3. Pakai jumper wire **sangat pendek** (< 2 cm)

Hasil: **mungkin** jalan untuk init dan testing ringan, tapi kemungkinan besar brownout saat jamming aktif (TX burst ~130 mA). Tidak predictable.

**Opsi 2: Beli modul yang sudah pre-soldered dengan kapasitor**

Beberapa toko di Shopee menjual modul nRF24L01+ PA+LNA yang sudah diberi kapasitor. Cari keyword "nrf24l01 pa lna kapasitor" atau "nrf24l01 ready". Jarang ada untuk EBYTE E01-ML01DP5 — biasanya hanya modul clone generik.

Hasil: lebih baik dari opsi 1, tapi tetap tidak sebaik solder langsung.

> **Rekomendasi:** Beli solder iron murah. 8 titik solder, 10 menit kerja, hasil reliable. Alternatif tanpa solder = hasil tidak predictable — modul bisa working saat init tapi reset saat jamming aktif.

### Kalau tetap mau tanpa solder

Kita masih bisa jalan — buat modul dengan breadboard, lakukan test `status` dan `scan` dulu. Kalau keduanya OK dan tidak ada brownout, ada kemungkinan setup kamu cukup stabil. Tapi kalau muncul gejala berikut saat jamming → kapasitor harus solder:

- Modul kadang OK kadang MISSING
- Jamming berhenti sendiri setelah beberapa detik/menit
- Sweeps count berhenti bertambah padahal status masih active
- ESP32 reboot sendiri (brownout message di serial)

---

## Langkah 3: Solder Kapasitor ke Modul EBYTE

Ini langkah **paling penting**. Tanpa kapasitor, modul akan reset saat transmit.

### Untuk SETIAP modul EBYTE (lakukan 2x):

1. Ambil **1 kapasitor elektrolit 100µF**
   - Kaki **panjang** = positif (+) = ke **VCC**
   - Kaki **pendek** = negatif (-) = ke **GND**
   - **HATI-HATI:** salah pasang polaritas = kapasitor bocor/pecah/meledak

2. Ambil **1 kapasitor keramik 100nF**
   - Tidak ada polaritas (boleh dipasang bolak-balik)

3. Solder **kedua kapasitor** langsung ke pin VCC dan GND modul EBYTE:
   - Kaki positif kapasitor elektrolit → pin **VCC** (solder ke header pin bawah modul)
   - Kaki negatif kapasitor elektrolit → pin **GND** (solder ke header pin bawah modul)
   - Kapasitor keramik 100nF: satu kaki ke **VCC**, kaki lain ke **GND** (pasang sedekat mungkin ke pin)

4. Ulangi untuk modul kedua

> **Kenapa harus solder langsung ke modul?** Current TX EBYTE ~130mA per modul. Jarak kapasitor ke chip harus sesingkat mungkin. Kalau lewat breadboard/jumper wire, terlalu banyak resistance/inductance → voltage drop → brownout.

### Cek hasil solder dengan multimeter:
- Set multimeter ke mode continuity (beep)
- Cek: VCC pin modul ↔ kaki + kapasitor → harus beep
- Cek: GND pin modul ↔ kaki - kapasitor → harus beep
- Cek: VCC ↔ GND → **TIDAK boleh beep** (kalau beep = short circuit, perbaiki solder)

---

## Langkah 4: Rakit Wiring

### Pilihan A: Pakai Breadboard (cepat, untuk testing)

Taro ESP32 di breadboard. Pakai jumper wire female-female untuk connect ESP32 ke modul EBYTE.

### Pilihan B: Solder langsung (permanent, lebih reliable)

Solder jumper wire dari ESP32 ke modul EBYTE langsung.

### Wiring — Modul 1 (HSPI)

Hubungkan pin ESP32 ke pin modul EBYTE pertama:

```
ESP32 Pin          →    EBYTE Modul 1 Pin
──────────                 ──────────────
3.3V               →    VCC
GND                →    GND
GPIO 16            →    CE
GPIO 15            →    CSN
GPIO 14            →    SCK
GPIO 13            →    MOSI
GPIO 12            →    MISO
(tidak dipakai)     →    IRQ
```

Gunakan **7 kabel jumper** female-female.

### Wiring — Modul 2 (VSPI)

Hubungkan pin ESP32 ke pin modul EBYTE kedua:

```
ESP32 Pin          →    EBYTE Modul 2 Pin
──────────                 ──────────────
3.3V               →    VCC
GND                →    GND
GPIO 22            →    CE
GPIO 5             →    CSN
GPIO 18            →    SCK
GPIO 23            →    MOSI
GPIO 19            →    MISO
(tidak dipakai)     →    IRQ
```

Gunakan **7 kabel jumper** female-female lain.

### Wiring — OLED (kalau ada)

```
ESP32 Pin          →    OLED Pin
──────────                 ─────────
3.3V               →    VCC
GND                →    GND
GPIO 4             →    SDA
GPIO 17            →    SCL
```

### Pasang Antena

Screw antena 2.4 GHz SMA ke **kedua modul EBYTE**.

> **Wajib pasang antena sebelum nyalakan!** Modul EBYTE output 20 dBm — tanpa antena, RF power memantul balik ke PA dan merusak modul dalam hitungan detik.

### Cek wiring sekali lagi

Sebelum colok USB, cek ulang:
- ✅ VCC modul = ESP32 3.3V (BUKAN 5V — modul rusak kalau 5V)
- ✅ GND modul = ESP32 GND
- ✅ CE, CSN, SCK, MOSI, MISO sesuai pin di tabel di atas
- ✅ Kapasitor 100µF + 100nF terpasang di setiap modul
- ✅ Antena terpasang di kedua modul

---

## Langkah 5: Install Software di Komputer

### 5A. Install Driver USB ESP32

ESP32 DevKit v4 pakai chip CP2102 untuk USB-serial.

1. Download driver dari [Silicon Labs CP210x](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)
2. Pilih versi Windows (atau Mac/Linux)
3. Install seperti biasa (next, next, finish)
4. Colok ESP32 ke komputer pakai kabel USB
5. Buka **Device Manager** (Windows: klik kanan Start → Device Manager)
6. Cari di **Ports (COM & LPT)** → harus muncul "Silicon Labs CP210x USB to UART Bridge (COMx)"
7. Catat nomor COM (misal: COM5, COM7, dll) — ini dipakai nanti

> Kalau tidak muncul di Device Manager:
> - Coba kabel USB lain (pastikan kabel data, bukan charge-only)
> - Coba port USB lain
> - Coba install driver ulang

### 5B. Install PlatformIO

PlatformIO adalah tool untuk build dan flash firmware ke ESP32.

**Cara 1: Pakai VS Code (paling mudah)**

1. Download dan install [VS Code](https://code.visualstudio.com/)
2. Buka VS Code
3. Klik icon **Extensions** di sidebar kiri (atau tekan Ctrl+Shift+X)
4. Cari "PlatformIO IDE"
5. Klik **Install**
6. Tunggu sampai install selesai (bisa beberapa menit)
7. Restart VS Code

**Cara 2: Pakai command line (untuk yang sudah familiar)**

```bash
pip install -U platformio
```

---

## Langkah 6: Download Project

### Cara 1: Download ZIP

1. Download folder project `quiet-please` ke komputer
2. Extract ZIP ke folder, misal: `C:\Users\NamaAnda\Desktop\project\quiet-please`

### Cara 2: Buka langsung di VS Code

1. Buka VS Code
2. File → Open Folder
3. Pilih folder `quiet-please`
4. VS Code akan otomatis detect PlatformIO project

---

## Langkah 7: Build Firmware

### Cara VS Code:

1. Buka folder `quiet-please` di VS Code
2. Tunggu sampai PlatformIO selesai load (lihat status bar bawah — tulisan "PlatformIO Ready")
3. Klik icon ✔ (checkmark) di status bar bawah → ini akan **Build** firmware
4. Tunggu beberapa menit (pertama kali download library, lama)
5. Kalau sukses, muncul tulisan hijau: `SUCCESS`

### Cara Command Line:

```bash
cd quiet-please
pio run
```

> Kalau ada error "platform not installed", jalankan: `pio platform install espressif32`
> Kalau ada error "RF24 not found", pastikan internet aktif — PlatformIO auto-download library.

---

## Langkah 8: Flash Firmware ke ESP32

### Cara VS Code:

1. Pastikan ESP32 sudah colok USB ke komputer
2. Klik icon → (panah kanan) di status bar bawah → ini akan **Upload/Flash**
3. Tunggu sampai selesai (30-60 detik)
4. Kalau sukses, muncul tulisan hijau: `SUCCESS`

### Cara Command Line:

```bash
pio run -t upload --upload-port COM5
```

Ganti `COM5` dengan nomor COM port yang kamu catat di Langkah 5A.

### Kalau upload gagal:

1. Tekan tombol **BOOT** di ESP32 board (tahan)
2. Tekan tombol **EN/RST** sekali (lepas)
3. Lepas tombol **BOOT** setelah muncul "Connecting..." di layar
4. Upload ulang

Ini biasanya terjadi kalau ESP32 belum masuk mode download otomatis.

---

## Langkah 9: Test Pertama via Serial Monitor

Setelah flash sukses, ESP32 akan reboot otomatis dan firmware mulai jalan.

### Buka Serial Monitor:

**Cara VS Code:**
1. Klik icon 🔌 (plug) di status bar bawah → Serial Monitor
2. Atau tekan Ctrl+Shift+P → ketik "Serial Monitor" → Enter

**Cara Command Line:**
```bash
pio device monitor -b 115200
```

### Apa yang harus muncul:

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
```

### Cek hasil:

- **`module M1 : OK`** → Modul 1 (HSPI) terdeteksi. Wiring benar.
- **`module M2 : OK`** → Modul 2 (VSPI) terdeteksi. Wiring benar.
- **`module M1 : MISSING`** → Masalah! Lihat troubleshooting di bawah.

> Kalau kedua modul **OK**, lanjut ke Langkah 10.
> Kalau ada yang **MISSING**, lihat **Troubleshooting** di bagian bawah.

### Ketik command test:

Di serial monitor, ketik `help` lalu Enter. Harus muncul daftar command.

Ketik `status` lalu Enter. Harus muncul status lengkap.

---

## Langkah 10: Test via Web UI (WiFi)

### 10A. Connect WiFi

1. Ambil HP atau laptop
2. Buka setting WiFi
3. Cari SSID **`quiet-please`**
4. Connect dengan password **`quiet123`**

> Saat connect ke WiFi `quiet-please`, HP/laptop tidak akan punya internet (karena ESP32 cuma bikin AP lokal, bukan router internet). Ini normal. Kalau mau tetap online, pakai mobile data.

### 10B. Buka Web UI

1. Buka browser (Chrome, Safari, dll)
2. Ketik: **`http://192.168.4.1`**
3. Muncul popup login:
   - Username: **`admin`**
   - Password: **`quiet123`**
4. Klik OK / Sign In

### 10C. Halaman Web UI

Halaman web akan menampilkan:

- **Titik merah** di atas = status idle (belum jamming)
- **4 tombol mode**: BLE ADV, BLE FULL, BT CLASSIC, CW SINGLE
- **Slider channel**: untuk pilih frekuensi CW
- **4 tombol power**: -18, -12, -6, 0 dBm
- **Input timer**: auto-stop (default 300 detik)
- **Tombol START** (hijau) dan **STOP** (merah)
- **Tombol SCAN CHANNELS** (biru) — scan aktivitas RF

### 10D. Test Scan (cek modul berfungsi)

Sebelum mulai jamming, test apakah modul bisa deteksi sinyal:

1. Nyalakan Bluetooth di HP (pastikan ada perangkat BT aktif di sekitar)
2. Klik tombol **SCAN CHANNELS** di web UI
3. Tunggu ~2 detik (scan 79 channel)
4. Hasil muncul sebagai bar chart:
   - **Bar hijau** = sinyal terdeteksi di channel tersebut
   - **Bar abu-abu** = tidak ada sinyal

Kalau ada bar hijau → modul berfungsi dengan baik.

Kalau semua bar abu-abu → kemungkinan tidak ada BT aktif di sekitar, atau modul tidak berfungsi (cek wiring ulang).

### 10E. Test Jamming

1. Klik tombol **BT CLASSIC** (mode 3)
2. Klik **START**
3. Titik di atas harus berubah **hijau** = jamming active
4. Status panel menampilkan:
   - Mode: BT_CLASSIC
   - Active: YES
   - Sweeps: (angka naik cepat)
   - Timer countdown (auto-stop dalam 300 detik)
5. Coba connect BT headphone/speaker ke HP → harus susah/gagal pairing
6. Klik **STOP** untuk berhenti

> **Auto-stop timer:** Default 300 detik (5 menit). Jamming otomatis berhenti setelah waktu habis. Ini fail-safe agar tidak lupa dimatikan. Set ke `0` untuk disable timer.

---

## Langkah 11: Test via Serial (Alternatif)

Kalau tidak punya HP atau lebih suka pakai komputer:

```
> mode 1              (pilih mode BLE ADV)
> start               (mulai jamming)
> status              (cek status)
> stop                (berhenti)
> scan                (scan channel)
> save                (simpan settings)
> timer 120           (set auto-stop 120 detik)
```

---

## Langkah 12: Test OLED (kalau ada)

Kalau OLED SSD1306 sudah terpasang (I2C GPIO4/GPIO17), display akan menampilkan:

```
┌─────────────────────┐
│ * BADV              │  ← mode + status dot (* = active)
│ PWR:3 CH:40(2440)   │  ← power + channel + freq
│ M1:OK M2:OK         │  ← module status
│ SWP:8421  T:4:32    │  ← sweep count + timer
└─────────────────────┘
```

Bottom bar terang = jamming active.

OLED update otomatis setiap 250ms. Tidak perlu command apa pun.

Kalau OLED blank/hitam:
- Cek wiring: SDA=GPIO4, SCL=GPIO17
- Cek I2C address: default 0x3C, beberapa modul pakai 0x3D
- Cek VCC/GND

---

## Langkah 13: Simpan Settings (NVS)

Settings (mode, power, channel, timer) bisa disimpan permanen ke flash ESP32. Settings survive reboot.

### Via Web UI:
Klik link atau akses `http://192.168.4.1/api/save` di browser.

### Via Serial:
```
> save
settings saved to NVS
```

Settings akan auto-load saat ESP32 boot.

---

## Langkah 14: Operasi Sehari-hari

Setelah semua test OK, cara pakai sehari-hari:

### Cara cepat:

1. Colok power ke ESP32 (USB power bank / adaptor HP)
2. Tunggu 3-5 detik (boot + WiFi AP start)
3. Connect HP ke WiFi `quiet-please` (password: `quiet123`)
4. Buka browser → `http://192.168.4.1`
5. Login: `admin` / `quiet123`
6. Pilih mode → START
7. Selesai → STOP
8. Kalau lupa STOP → auto-stop dalam 5 menit

### Mode yang recommended:

| Skenario | Mode | Kenapa |
|---|---|---|
| Ganggu pairing earbud/smartwatch baru | **BLE ADV** | Hanya jam 3 advertising channel, efektif untuk prevent pairing |
| Putus koneksi BT yang sudah aktif | **BT CLASSIC** | Sweep 79 channel, ganggu semua BT Classic |
| Target perangkat spesifik di freq tertentu | **CW SINGLE** | Carrier kontinu di 1 channel — paling kuat per channel |
| Cek channel mana yang aktif | **SCAN** | RX mode, deteksi sinyal tanpa transmit |

---

## Troubleshooting

### Modul MISSING (tidak terdeteksi)

**Gejala:** `module M1 : MISSING` atau `module M2 : MISSING` di status

**Solusi step-by-step:**

1. **Cek wiring** — pastikan setiap kabel jumper terhubung:
   - CE → GPIO 16 (M1) atau GPIO 22 (M2)
   - CSN → GPIO 15 (M1) atau GPIO 5 (M2)
   - SCK → GPIO 14 (M1) atau GPIO 18 (M2)
   - MOSI → GPIO 13 (M1) atau GPIO 23 (M2)
   - MISO → GPIO 12 (M1) atau GPIO 19 (M2)
   - VCC → 3.3V (BUKAN 5V!)
   - GND → GND

2. **Cek kapasitor** — pastikan 100µF terpasang dengan polaritas benar:
   - Kaki panjang (+) → VCC
   - Kaki pendek (-) → GND

3. **Cek voltage** dengan multimeter:
   - Set multimeter ke DC voltage
   - Probe merah ke VCC pin modul, probe hitam ke GND
   - Harus baca **3.3V** (toleransi 3.0-3.5V)
   - Kalau baca 0V → wiring putus
   - Kalau baca 5V → **SALAH!** Ganti ke pin 3.3V ESP32

4. **Cek jumper wire** — kadang kabel jumper murah rusak/longgar. Coba ganti kabel satu per satu.

5. **Tukar modul** — kalau M1 MISSING tapi M2 OK, tukar posisi modul. Kalau yang sebelumnya M2 jadi MISSING → modul tersebut rusak. Kalau M1 tetap MISSING → wiring M1 yang bermasalah.

### ESP32 tidak muncul di Device Manager

1. Coba kabel USB lain — pastikan kabel **data**, bukan charge-only
2. Coba port USB lain
3. Install driver CP2102 ulang
4. Tekan tombol EN/RST di ESP32

### Upload gagal / "Connecting..."

1. Tahan tombol **BOOT**
2. Tekan tombol **EN** sekali (lepas)
3. Setelah "Connecting..." muncul di layar, lepas **BOOT**
4. Upload ulang

### Build gagal / error library

1. Pastikan internet aktif (PlatformIO download library otomatis)
2. Jalankan: `pio pkg install`
3. Kalau masih error, hapus folder `.pio` dan build ulang:
   ```bash
   rm -rf .pio
   pio run
   ```

### Jamming tidak efektif

1. **Cek modul** — pastikan `module M1 : OK` dan `module M2 : OK`
2. **Scan dulu** — klik SCAN CHANNELS, pastikan ada bar hijau (sinyal terdeteksi)
3. **Coba mode CW SINGLE** — set channel yang terdeteksi di scan, START. Ini paling kuat per channel.
4. **Cek jarak** — dekatkan ESP32 ke target BT (< 5m untuk test awal)
5. **Cek antena** — pastikan antena SMA terpasang kencang di modul
6. **Cek power** — multimeter di VCC pin saat TX aktif. Kalau drop < 3.0V → tambah cap atau pakai regulator eksternal

### Web UI tidak bisa diakses

1. Pastikan HP/laptop connect ke WiFi `quiet-please` (bukan WiFi rumah)
2. Coba akses `http://192.168.4.1` (bukan `https://`)
3. Coba browser lain (Chrome, Firefox, Safari)
4. Login: user `admin`, password `quiet123`
5. Kalau lupa password → cek di `include/config.h` baris `WEB_PASS`

### OLED blank / hitam

1. Cek wiring: SDA=GPIO4, SCL=GPIO17, VCC=3.3V, GND=GND
2. Beberapa OLED pakai I2C address 0x3D bukan 0x3C. Edit di `include/config.h`:
   ```c
   #define OLED_ADDR 0x3D
   ```
3. Rebuild dan flash ulang

### Jamming auto-stop padahal belum mau berhenti

1. Auto-stop timer aktif (default 300 detik / 5 menit)
2. Set timer ke 0 untuk disable: ketik `timer 0` di serial, atau set ke 0 di web UI
3. Atau set lebih lama: `timer 3600` = 1 jam

### ESP32 reboot terus / brownout

1. Current TX 2 modul EBYTE = ~260mA — ESP32 regulator mungkin tidak kuat
2. **Solusi 1:** Tambah kapasitor 100µF di setiap modul (kalau belum)
3. **Solusi 2:** Pakai regulator eksternal AMS1117-3.3:
   - Input: 5V dari USB
   - Output: 3.3V ke VCC modul EBYTE (bukan dari pin ESP32)
   - GND: common dengan ESP32 GND
4. **Solusi 3:** Power ESP32 dari power bank 5V 2A (bukan USB komputer yang mungkin current terbatas)

---

## Checklist Akhir

Sebelum nyatakan "sudah jadi", pastikan semua ini ✅:

- [ ] ESP32 terdeteksi di Device Manager (COM port muncul)
- [ ] Firmware build sukses (`pio run` → SUCCESS)
- [ ] Flash sukses (`pio run -t upload` → SUCCESS)
- [ ] Serial monitor menampilkan boot message
- [ ] `module M1 : OK` dan `module M2 : OK` di status
- [ ] WiFi AP `quiet-please` muncul di HP
- [ ] Web UI bisa diakses di `http://192.168.4.1`
- [ ] Login web: `admin` / `quiet123` berhasil
- [ ] SCAN CHANNELS menampilkan hasil (ada bar hijau)
- [ ] START → status berubah active, titik hijau
- [ ] STOP → status kembali idle, titik merah
- [ ] (Kalau ada OLED) Display menampilkan status
- [ ] Antena terpasang di kedua modul
- [ ] Kapasitor 100µF + 100nF terpasang di kedua modul

Kalau semua ✅ → **project siap digunakan.**

---

## Tips Penting

1. **Selalu pasang antena sebelum nyalakan.** E01-ML01DP5 output 20 dBm — tanpa antena, PA rusak.

2. **VCC 3.3V, bukan 5V.** Modul EBYTE dirancang untuk 3.3V. 5V = modul rusak permanen.

3. **Mulai dari mode SCAN** untuk verifikasi modul berfungsi sebelum jamming.

4. **Auto-stop timer** default 300 detik. Kalau lupa stop, firmware auto-stop. Set ke 0 untuk disable.

5. **Kalau hasil tidak efektif**, coba:
   - Dekatkan ke target (< 5m untuk test awal)
   - Pakai mode CW SINGLE di channel yang terdeteksi dari scan
   - Cek voltage VCC saat TX aktif (harus tetap 3.3V)
   - Upgrade antena ke Yagi/panel directional

6. **Save settings** setelah konfigurasi (`save` di serial atau `/api/save` di web). Settings survive reboot.

7. **Web UI dan Serial bisa dipakai bersamaan.** Tidak konflik. Keduanya akses engine yang sama.
