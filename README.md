# Brankas Anti Maling - IoT Security System

## Deskripsi Proyek

Brankas Anti Maling adalah sistem keamanan cerdas berbasis Internet of Things (IoT) yang mengintegrasikan teknologi terbaru untuk melindungi aset berharga di rumah maupun tempat usaha. Sistem ini tidak hanya mengandalkan kekuatan fisik tradisional, tetapi juga dilengkapi dengan berbagai lapisan keamanan digital yang dapat diakses dan dikontrol secara real-time.

## Fitur Utama

### Sistem Deteksi dan Monitoring
- **Sensor Getar**: Mendeteksi getaran dan percobaan pembongkaran
- **Sensor Gerak (PIR)**: Mengidentifikasi pergerakan mencurigakan di sekitar brankas
- **Autentikasi Multi-Faktor**: Sidik jari dan password untuk akses terautorisasi
- **Pelacakan GPS**: Real-time location tracking dengan modul GPS NEO-6M
- **Notifikasi Real-time**: Alert langsung ke WhatsApp dan aplikasi Android

### Respons Keamanan Otomatis
- Alarm sirene otomatis saat terdeteksi percobaan pembobolan
- Aktivasi kamera pengintai untuk mengambil foto pelaku
- Pencatatan log ke database cloud untuk audit keamanan
- Notifikasi push ke perangkat mobile

### Monitoring dan Kontrol
- Aplikasi Android dengan antarmuka pengguna intuitif
- Dashboard real-time untuk memantau status keamanan
- Kontrol remote untuk membuka/mengunci brankas
- Histori akses dan aktivitas mencurigakan

## Spesifikasi Teknis

### Hardware Components
- **Mikrokontroler**: ESP32 WROOM 32
- **Sensor Biometrik**: Fingerprint Scanner R305
- **Modul GPS**: NEO-6M dengan antena eksternal
- **Modul GSM**: SIM800L untuk komunikasi seluler
- **Sensor Gerak**: HC-SR501 PIR Motion Sensor
- **Sensor Getar**: SW-420 Vibration Sensor
- **Output**: Buzzer aktif, Relay lock mechanism
- **Kamera**: ESP32-CAM module

### Software & Platform
- **Framework IoT**: Arduino Core for ESP32
- **Cloud Database**: Firebase Realtime Database
- **Mobile Platform**: Android Native (Kotlin)
- **Communication**: MQTT Protocol, HTTP REST API
- **Authentication**: Firebase Auth

## Architecture System

```
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│   Hardware      │    │   Cloud          │    │   Mobile        │
│   Components    │───▶│   Platform       │───▶│   Application   │
│                 │    │                  │    │                 │
│ - ESP32 MCU     │    │ - Firebase       │    │ - Android App   │
│ - Sensors       │    │ - Realtime DB    │    │ - Real-time     │
│ - GPS & GSM     │    │ - Authentication │    │   Monitoring    │
│ - Camera        │    │ - Storage        │    │ - Remote Control│
└─────────────────┘    └──────────────────┘    └─────────────────┘
         │                       │                       │
         └─────────────────────────────────────────────────┘
                         Real-time Data Flow
```

## Instalasi dan Setup

### Prerequisites
- Arduino IDE 2.0 atau lebih baru
- Board ESP32 dalam Arduino Board Manager
- Library Firebase ESP32 Client
- Library TinyGPS++
- Library Adafruit Fingerprint Sensor
- Android Studio (untuk development aplikasi mobile)

### Hardware Installation

1. **Wiring Diagram**:
```
ESP32 GPIO34  → Sensor Getar (Analog)
ESP32 GPIO35  → Sensor PIR (Digital)
ESP32 GPIO32  → Buzzer Aktif
ESP32 GPIO33  → Relay Lock Mechanism
ESP32 GPIO25  → Camera Trigger
ESP32 GPIO16  → GPS TX
ESP32 GPIO17  → GPS RX
ESP32 GPIO26  → Fingerprint TX
ESP32 GPIO27  → Fingerprint RX
ESP32 3.3V    → Power semua sensor
ESP32 GND     → Ground common
```

2. **Assembly Steps**:
   - Mount semua komponen dalam enclosure yang kokoh
   - Pastikan koneksi GPS dan GSM memiliki akses ke sinyal
   - Install sensor getar pada titik strategis brankas
   - Tempatkan sensor PIR untuk coverage area optimal

### Software Configuration

1. **Konfigurasi ESP32**:
   - Clone repository dan buka di Arduino IDE
   - Install required libraries melalui Library Manager
   - Update konfigurasi WiFi dan Firebase di file config
   - Upload sketch ke board ESP32

2. **Setup Firebase**:
   - Buat project baru di Firebase Console
   - Aktifkan Realtime Database
   - Generate authentication tokens
   - Update security rules sesuai kebutuhan

3. **Aplikasi Android**:
   - Import project Android ke Android Studio
   - Konfigurasi Firebase SDK
   - Build dan install ke perangkat Android

## Konfigurasi dan Kalibrasi

### Kalibrasi Sensor
1. **Sensor Getar**: Atur threshold sensitivity berdasarkan lingkungan
2. **Sensor PIR**: Adjust delay dan sensitivity untuk mengurangi false positive
3. **Fingerprint**: Enroll fingerprint pengguna terdaftar melalui serial interface

### Konfigurasi Cloud
```json
{
  "firebase_config": {
    "host": "your-project.firebaseio.com",
    "auth": "your-secret-token"
  },
  "alert_settings": {
    "cooldown_period": 30000,
    "max_alerts_per_hour": 10
  }
}
```

## Penggunaan

### Operasi Normal
1. **Arming System**: Sistem aktif secara otomatis setelah boot
2. **Akses Terotorisasi**: Gunakan fingerprint atau aplikasi mobile untuk membuka
3. **Monitoring**: Pantau status melalui dashboard real-time

### Emergency Procedures
1. **Alert Otomatis**: Sistem akan trigger alarm dan notifikasi secara otomatis
2. **Manual Override**: Akses emergency melalui kode master di aplikasi
3. **System Reset**: Prosedur reset melalui kombinasi button presses

## API Documentation

### Endpoints Firebase
- `/security/status` - Status sistem keamanan
- `/security/alerts` - Log alert keamanan
- `/control/unlock_command` - Perintah buka kunci
- `/access_logs` - Histori akses pengguna

### Mobile App Commands
- `setUnlockCommand(true)` - Membuka brankas remote
- `getSecurityStatus()` - Mendapat status keamanan
- `getLocation()` - Mendapat koordinat GPS terkini

## Troubleshooting

### Common Issues
1. **Koneksi WiFi Terputus**: Sistem akan switch ke backup GSM
2. **Sensor False Positive**: Adjust sensitivity threshold
3. **GPS No Signal**: Pastikan antena terpasang dengan baik
4. **Battery Low**: Notifikasi akan dikirim saat battery <20%

### Maintenance Routine
- Weekly sensor calibration check
- Monthly system health diagnostic
- Quarterly firmware update check
- Biannual physical inspection

## Security Considerations

### Data Protection
- Enkripsi data dalam transit menggunakan TLS
- Authentication token rotation
- Secure storage untuk sensitive data

### Physical Security
- Tamper-proof enclosure design
- Anti-prying mechanism
- Backup power supply

## Development Roadmap

### Versi 1.1 (Q2 2024)
- Integration dengan smart home systems
- Voice command support
- Enhanced machine learning untuk pattern recognition

### Versi 1.2 (Q3 2024)
- Facial recognition capability
- Blockchain-based audit trail
- Multi-language support

## Kontribusi

Untuk berkontribusi dalam pengembangan sistem ini:

1. Fork repository
2. Buat feature branch
3. Commit changes dengan deskripsi jelas
4. Submit pull request untuk review

## Support dan Kontak

Untuk pertanyaan teknis dan dukungan:

- Documentation: [Link ke dokumentasi lengkap]
- Issues: [GitHub Issues page]
- Email: tech-support@brankas-antimaling.com

## Lisensi

Proyek ini dilisensikan di bawah MIT License - lihat file LICENSE untuk detail lengkap.

## Disclaimer

Sistem ini dirancang sebagai lapisan keamanan tambahan dan tidak menggantikan sistem keamanan konvensional. Pengguna disarankan untuk tetap mengimplementasikan praktik keamanan fisik standar.
