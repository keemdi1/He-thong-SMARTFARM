#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <DHT.h>
#include <ld2410.h>

// ================== RADAR ==================
#define MONITOR_SERIAL Serial
#define RADAR_SERIAL   Serial1
#define RADAR_RX_PIN   16
#define RADAR_TX_PIN   17
#define RADAR_BAUDRATE 115200

ld2410 radar;
bool isPresent = false;
unsigned long lastPresenceTime = 0;
const unsigned long AUTO_TIMEOUT = 20000; // 20s sau khi không phát hiện người
unsigned long lastRadarRead = 0;

// ================== WIFI ==================
const char* ssid = "B805_1";
const char* password = "1234567890";

// ================== FIREBASE ==================
#define API_KEY "AIzaSyBaZgHe9UKU5Qe6FBgsW0VPmNHX028jzmU"
#define DATABASE_URL "https://smartfam-9256d-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "ax0TggUUF5PKeWptYidyuHR2UA51UGuXiatwtYdp"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// ================== CẢM BIẾN ==================
#define DOAM_KHONGKHI_PIN 4
#define DOAM_DAT_PIN 35
#define ANHSANG_PIN 32
DHT dht(DOAM_KHONGKHI_PIN, DHT11);

// ================== THIẾT BỊ ==================
#define FAN_PIN 13
#define DEN_PIN 12
#define BOM_PIN 25
#define VALVE_SPRAY_PIN 27
#define VALVE_WATERING_PIN 26
#define VALVE_DRAIN_PIN 14

// ================== LED TRẠNG THÁI ==================
#define AUTO_PIN 21
#define MANUAL_PIN 22
#define ERROR_PIN 23

// ================== NGƯỠNG ==================
#define DOAM_DAT_KHO 1000
#define DOAM_DAT_UOT 3000
#define ANHSANG_TOI 3500
#define ANHSANG_SANG 500

// ================== BIẾN TOÀN CỤC ==================
int isAuto = 1;
String lastControlSource = "firebase";

float prev_nhietdo = -1000;
float prev_doamkk = -1000;
int prev_doamdat = -1;
int prev_anhsang = -1;

// ================== HÀM PHỤ ==================
int readAnalogAverage(int pin, int samples = 10, int delayMs = 5) {
  long total = 0;
  for (int i = 0; i < samples; i++) {
    total += analogRead(pin);
    delay(delayMs);
  }
  return total / samples;
}

void initRadar() {
  RADAR_SERIAL.begin(RADAR_BAUDRATE, SERIAL_8N1, RADAR_RX_PIN, RADAR_TX_PIN);
  delay(500);
  MONITOR_SERIAL.println("Khởi động radar LD2410...");
  if (radar.begin(RADAR_SERIAL)) MONITOR_SERIAL.println("Radar LD2410 kết nối thành công!");
  else MONITOR_SERIAL.println("Không thể kết nối radar!");
}

void initWiFi() {
  WiFi.begin(ssid, password);
  MONITOR_SERIAL.print("Kết nối WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    MONITOR_SERIAL.print(".");
  }
  MONITOR_SERIAL.println("\nWiFi đã kết nối!");
  MONITOR_SERIAL.println(WiFi.localIP());
}

void initFirebase() {
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  if (Firebase.ready()) MONITOR_SERIAL.println("Firebase đã kết nối!");
}

int getFirebaseInt(String path) {
  if (Firebase.RTDB.getInt(&fbdo, path)) return fbdo.intData();
  else {
    MONITOR_SERIAL.print("Lỗi đọc Firebase: ");
    MONITOR_SERIAL.println(path);
    return 0;
  }
}

// ================== SETUP ==================
void setup() {
  MONITOR_SERIAL.begin(115200);
  dht.begin();
  initRadar();

  pinMode(FAN_PIN, OUTPUT);
  pinMode(DEN_PIN, OUTPUT);
  pinMode(BOM_PIN, OUTPUT);
  pinMode(VALVE_SPRAY_PIN, OUTPUT);
  pinMode(VALVE_WATERING_PIN, OUTPUT);
  pinMode(VALVE_DRAIN_PIN, OUTPUT);
  pinMode(AUTO_PIN, OUTPUT);
  pinMode(MANUAL_PIN, OUTPUT);
  pinMode(ERROR_PIN, OUTPUT);

  initWiFi();
  initFirebase();

  Firebase.RTDB.setString(&fbdo, "/last_control_source", "firebase");
  Firebase.RTDB.setInt(&fbdo, "/autoBtn", isAuto);
}

// ================== LOOP ==================
void loop() {
  unsigned long loopStart = millis();

  // --- RADAR ---
  radar.read();
  if (millis() - lastRadarRead > 100) {
    lastRadarRead = millis();
    bool radarNow = radar.presenceDetected() || radar.stationaryTargetDetected();

    if (radarNow) {
      if (!isPresent) {
        MONITOR_SERIAL.println("👤 Phát hiện có người → MANUAL");
        isPresent = true;
        if (isAuto) {
          isAuto = 0;
          Firebase.RTDB.setInt(&fbdo, "/autoBtn", 0);
          Firebase.RTDB.setString(&fbdo, "/last_control_source", "radar");
        }
      }
      lastPresenceTime = millis();
    } else {
      // Không phát hiện người
      if (isPresent && millis() - lastPresenceTime > AUTO_TIMEOUT) {
        isPresent = false;
        MONITOR_SERIAL.println("🚪 Không còn người → chuyển về AUTO");
        isAuto = 1;
        Firebase.RTDB.setInt(&fbdo, "/autoBtn", 1);
        Firebase.RTDB.setString(&fbdo, "/last_control_source", "radar");
      }
    }
  }

  // --- FIREBASE ---
  // Firebase chỉ được phép đổi chế độ khi không có người
  if (!isPresent && Firebase.RTDB.getInt(&fbdo, "/autoBtn")) {
    int remoteAuto = fbdo.intData();
    if (remoteAuto != isAuto) {
      isAuto = remoteAuto;
      lastControlSource = "firebase";
      Firebase.RTDB.setString(&fbdo, "/last_control_source", "firebase");
      MONITOR_SERIAL.printf("🌐 Firebase đổi chế độ: %s\n", isAuto ? "AUTO" : "MANUAL");
    }
  }

  // --- CẢM BIẾN ---
  float nhietdo = dht.readTemperature();
  float doamkk = dht.readHumidity();
  int doamdat_raw = readAnalogAverage(DOAM_DAT_PIN);
  int anhsang_raw = readAnalogAverage(ANHSANG_PIN);

  if (isnan(nhietdo) || isnan(doamkk)) {
    digitalWrite(ERROR_PIN, HIGH);
    MONITOR_SERIAL.println("❌ Lỗi đọc DHT11!");
    delay(1000);
    return;
  } else digitalWrite(ERROR_PIN, LOW);

  int doamdat_percent = constrain(map(doamdat_raw, DOAM_DAT_KHO, DOAM_DAT_UOT, 0, 100), 0, 100);
  int anhsang_percent = constrain(map(anhsang_raw, ANHSANG_TOI, ANHSANG_SANG, 0, 100), 0, 100);

  // --- GỬI DỮ LIỆU LÊN FIREBASE ---
  if (abs(nhietdo - prev_nhietdo) >= 0.5) {
    Firebase.RTDB.setFloat(&fbdo, "/cam_bien/nhietdo", nhietdo);
    prev_nhietdo = nhietdo;
  }
  if (abs(doamkk - prev_doamkk) >= 5) {
    Firebase.RTDB.setFloat(&fbdo, "/cam_bien/doamkk", doamkk);
    prev_doamkk = doamkk;
  }
  if (abs(doamdat_percent - prev_doamdat) >= 5) {
    Firebase.RTDB.setInt(&fbdo, "/cam_bien/doamdat", doamdat_percent);
    prev_doamdat = doamdat_percent;
  }
  if (abs(anhsang_percent - prev_anhsang) >= 5) {
    Firebase.RTDB.setInt(&fbdo, "/cam_bien/anhsang", anhsang_percent);
    prev_anhsang = anhsang_percent;
  }

  // --- LED TRẠNG THÁI ---
  digitalWrite(AUTO_PIN, isAuto ? HIGH : LOW);
  digitalWrite(MANUAL_PIN, isAuto ? LOW : HIGH);

  // --- ĐIỀU KHIỂN THIẾT BỊ ---
  if (isAuto) {
    // AUTO: điều khiển hoàn toàn theo cảm biến
    digitalWrite(FAN_PIN, nhietdo > 30);
    digitalWrite(DEN_PIN, anhsang_percent < 30);
    digitalWrite(BOM_PIN, doamdat_percent < 40);
    digitalWrite(VALVE_SPRAY_PIN, doamkk < 30);
    digitalWrite(VALVE_WATERING_PIN, doamdat_percent < 50);
    digitalWrite(VALVE_DRAIN_PIN, doamdat_percent > 95);
  } else {
    // MANUAL: lấy trạng thái từ Firebase
    digitalWrite(FAN_PIN, getFirebaseInt("/thietbi5/quat"));
    digitalWrite(DEN_PIN, getFirebaseInt("/thietbi4/den"));
    digitalWrite(BOM_PIN, getFirebaseInt("/thietbi1/bom"));
    digitalWrite(VALVE_SPRAY_PIN, getFirebaseInt("/thietbi2/phun_suong"));
    digitalWrite(VALVE_WATERING_PIN, getFirebaseInt("/thietbi6/valve_watering"));
    digitalWrite(VALVE_DRAIN_PIN, getFirebaseInt("/thietbi3/van_xa"));
  }

  // --- GỬI TRẠNG THÁI THIẾT BỊ LÊN FIREBASE ---
  Firebase.RTDB.setInt(&fbdo, "/thietbi5/quat", digitalRead(FAN_PIN));
  Firebase.RTDB.setInt(&fbdo, "/thietbi4/den", digitalRead(DEN_PIN));
  Firebase.RTDB.setInt(&fbdo, "/thietbi1/bom", digitalRead(BOM_PIN));
  Firebase.RTDB.setInt(&fbdo, "/thietbi2/phun_suong", digitalRead(VALVE_SPRAY_PIN));
  Firebase.RTDB.setInt(&fbdo, "/thietbi6/valve_watering", digitalRead(VALVE_WATERING_PIN));
  Firebase.RTDB.setInt(&fbdo, "/thietbi3/van_xa", digitalRead(VALVE_DRAIN_PIN));

  // --- LOG RA SERIAL ---
  MONITOR_SERIAL.println("==================================");
  MONITOR_SERIAL.printf("🌡️ %.1f°C | 💧KK %.1f%% | 💦Đất %d%% | ☀️Ánh sáng %d%%\n", nhietdo, doamkk, doamdat_percent, anhsang_percent);
  MONITOR_SERIAL.printf("👤 Radar: %s | ⚙️ Chế độ: %s | Nguồn: %s\n", isPresent ? "Có người" : "Không có", isAuto ? "AUTO" : "MANUAL", lastControlSource.c_str());
  MONITOR_SERIAL.printf("Thiết bị: Quạt:%d  Đèn:%d  Bơm:%d  Phun:%d  Tưới:%d  Xả:%d\n",
                        digitalRead(FAN_PIN), digitalRead(DEN_PIN), digitalRead(BOM_PIN),
                        digitalRead(VALVE_SPRAY_PIN), digitalRead(VALVE_WATERING_PIN), digitalRead(VALVE_DRAIN_PIN));
  MONITOR_SERIAL.printf("⏱️ Thời gian vòng lặp: %lu ms\n", millis() - loopStart);
  MONITOR_SERIAL.println("==================================");

  delay(1000);
}
