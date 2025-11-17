#include <WiFi.h>
#include <FirebaseESP32.h>
#include <HardwareSerial.h>
#include <TinyGPS++.h>
#include <Adafruit_Fingerprint.h>

// Konfigurasi WiFi
#define WIFI_SSID "Your_WiFi_SSID"
#define WIFI_PASSWORD "Your_WiFi_Password"

// Konfigurasi Firebase
#define FIREBASE_HOST "your-project.firebaseio.com"
#define FIREBASE_AUTH "Your_Firebase_Auth_Key"

// Pin Definitions
#define VIBRATION_SENSOR_PIN 34
#define PIR_SENSOR_PIN 35
#define BUZZER_PIN 32
#define RELAY_LOCK_PIN 33
#define CAMERA_TRIGGER_PIN 25

// Sensor Objects
HardwareSerial gpsSerial(1);
TinyGPSPlus gps;
HardwareSerial fingerprintSerial(2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerprintSerial);

// Firebase Objects
FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

// Variables
unsigned long lastAlertTime = 0;
const unsigned long ALERT_COOLDOWN = 30000; // 30 detik
bool systemArmed = true;
String authorizedUsers[] = {"user1", "user2", "admin"};

void setup() {
  Serial.begin(115200);
  
  // Initialize pins
  pinMode(VIBRATION_SENSOR_PIN, INPUT);
  pinMode(PIR_SENSOR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RELAY_LOCK_PIN, OUTPUT);
  pinMode(CAMERA_TRIGGER_PIN, OUTPUT);
  
  // Initialize modules
  initializeWiFi();
  initializeFirebase();
  initializeGPS();
  initializeFingerprint();
  
  Serial.println("Brankas Anti Maling System Ready!");
}

void loop() {
  if (systemArmed) {
    checkSecuritySensors();
  }
  
  checkFingerprintAuth();
  checkManualUnlock();
  updateLocation();
  
  delay(100);
}

void initializeWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void initializeFirebase() {
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
  Serial.println("Firebase Initialized");
}

void initializeGPS() {
  gpsSerial.begin(9600, SERIAL_8N1, 16, 17);
  Serial.println("GPS Module Initialized");
}

void initializeFingerprint() {
  fingerprintSerial.begin(57600, SERIAL_8N1, 26, 27);
  
  if (finger.verifyPassword()) {
    Serial.println("Fingerprint Sensor Found!");
  } else {
    Serial.println("Fingerprint Sensor NOT Found!");
  }
}

void checkSecuritySensors() {
  int vibration = analogRead(VIBRATION_SENSOR_PIN);
  int motion = digitalRead(PIR_SENSOR_PIN);
  
  // Deteksi getaran mencurigakan
  if (vibration > 2000) {
    triggerSecurityAlert("Vibration detected", "HIGH");
  }
  
  // Deteksi gerakan mencurigakan
  if (motion == HIGH) {
    triggerSecurityAlert("Motion detected", "MEDIUM");
  }
}

void triggerSecurityAlert(String alertType, String severity) {
  if (millis() - lastAlertTime < ALERT_COOLDOWN) {
    return; // Masih dalam cooldown
  }
  
  lastAlertTime = millis();
  
  // Aktifkan alarm
  activateAlarm();
  
  // Trigger camera
  triggerCamera();
  
  // Kirim notifikasi
  sendAlertNotification(alertType, severity);
  
  // Update Firebase
  updateSecurityStatus(alertType, severity);
  
  Serial.println("SECURITY ALERT: " + alertType);
}

void activateAlarm() {
  for (int i = 0; i < 10; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(500);
    digitalWrite(BUZZER_PIN, LOW);
    delay(500);
  }
}

void triggerCamera() {
  digitalWrite(CAMERA_TRIGGER_PIN, HIGH);
  delay(1000);
  digitalWrite(CAMERA_TRIGGER_PIN, LOW);
  
  // Simulasi pengambilan foto
  captureIntruderPhoto();
}

void captureIntruderPhoto() {
  // Kode untuk mengambil foto dengan kamera
  // dan mengirim via WhatsApp
  String photoData = "intruder_photo_" + String(millis());
  
  if (Firebase.setString(firebaseData, "/security/last_photo", photoData)) {
    Serial.println("Intruder photo captured and uploaded");
  }
}

void sendAlertNotification(String alertType, String severity) {
  // Simpan data alert ke Firebase
  String alertPath = "/security/alerts/" + String(millis());
  
  FirebaseJson alertData;
  alertData.set("type", alertType);
  alertData.set("severity", severity);
  alertData.set("timestamp", millis());
  alertData.set("location", getCurrentLocation());
  
  if (Firebase.setJSON(firebaseData, alertPath.c_str(), alertData)) {
    Serial.println("Alert notification sent to cloud");
  }
}

void checkFingerprintAuth() {
  int fingerprintID = getFingerprintID();
  
  if (fingerprintID != -1) {
    if (isAuthorizedUser(fingerprintID)) {
      unlockSafe();
      logAccess("Fingerprint", "GRANTED", String(fingerprintID));
    } else {
      triggerSecurityAlert("Unauthorized fingerprint attempt", "HIGH");
      logAccess("Fingerprint", "DENIED", String(fingerprintID));
    }
  }
}

int getFingerprintID() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return -1;
  
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1;
  
  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) return -1;
  
  return finger.fingerID;
}

bool isAuthorizedUser(int fingerprintID) {
  // Cek apakah fingerprint ID terdaftar
  for (int i = 0; i < sizeof(authorizedUsers)/sizeof(authorizedUsers[0]); i++) {
    if (fingerprintID == i + 1) { // ID fingerprint mulai dari 1
      return true;
    }
  }
  return false;
}

void unlockSafe() {
  digitalWrite(RELAY_LOCK_PIN, HIGH);
  systemArmed = false;
  Serial.println("Safe UNLOCKED");
  
  delay(5000); // Buka selama 5 detik
  
  lockSafe();
}

void lockSafe() {
  digitalWrite(RELAY_LOCK_PIN, LOW);
  systemArmed = true;
  Serial.println("Safe LOCKED");
}

void checkManualUnlock() {
  // Cek input dari keypad atau aplikasi
  if (Firebase.getString(firebaseData, "/control/unlock_command")) {
    if (firebaseData.stringData() == "true") {
      String user = Firebase.getString(firebaseData, "/control/user") ? 
                   firebaseData.stringData() : "MobileApp";
      
      unlockSafe();
      logAccess("Mobile App", "GRANTED", user);
      
      // Reset command
      Firebase.setString(firebaseData, "/control/unlock_command", "false");
    }
  }
}

void updateLocation() {
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      if (gps.location.isValid()) {
        String location = String(gps.location.lat(), 6) + "," + 
                         String(gps.location.lng(), 6);
        
        Firebase.setString(firebaseData, "/status/location", location);
        Firebase.setString(firebaseData, "/status/last_update", String(millis()));
      }
    }
  }
}

String getCurrentLocation() {
  if (gps.location.isValid()) {
    return String(gps.location.lat(), 6) + "," + String(gps.location.lng(), 6);
  }
  return "Unknown";
}

void logAccess(String method, String status, String user) {
  String logPath = "/access_logs/" + String(millis());
  
  FirebaseJson logData;
  logData.set("method", method);
  logData.set("status", status);
  logData.set("user", user);
  logData.set("timestamp", millis());
  logData.set("location", getCurrentLocation());
  
  Firebase.setJSON(firebaseData, logPath.c_str(), logData);
}

void updateSecurityStatus(String alertType, String severity) {
  FirebaseJson status;
  status.set("alert_type", alertType);
  status.set("severity", severity);
  status.set("timestamp", millis());
  status.set("system_armed", systemArmed);
  status.set("battery_level", analogRead(36)); // Contoh membaca battery
  
  Firebase.setJSON(firebaseData, "/security/status", status);
}
