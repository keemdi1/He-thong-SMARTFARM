#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <DHT.h>

// ---------- WiFi ----------
const char* ssid = "B805_1";
const char* password = "1234567890";

// ---------- Firebase ----------
#define API_KEY "AIzaSyBaZgHe9UKU5Qe6FBgsW0VPmNHX028jzmU"
#define DATABASE_URL "https://smartfam-9256d-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "ax0TggUUF5PKeWptYidyuHR2UA51UGuXiatwtYdp"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// ========== CẢM BIẾN ==========
#define DOAM_KHONGKHI_PIN 4    // DHT11
#define DOAM_DAT_PIN 35        // Soil
#define ANHSANG_PIN 32         // Ánh sáng
DHT dht(DOAM_KHONGKHI_PIN, DHT11);

// ========== THIẾT BỊ ==========
#define FAN_PIN 13             // Quạt
#define DEN_PIN 12             // Đèn
#define BOM_PIN 25             // Bơm
#define VALVE_SPRAY_PIN 27     // Van phun sương
#define VALVE_WATERING_PIN 26  // Van tưới
#define VALVE_DRAIN_PIN 14     // Van xả

// ========== LED TRẠNG THÁI ==========
#define AUTO_PIN 21
#define MANUAL_PIN 22
#define ERROR_PIN 23

int isAuto = 1;  // 1 là tự động, 0 là thủ công

// ========== NGƯỠNG & LỌC NHIỄU ==========
#define DOAM_DAT_KHO 1000
#define DOAM_DAT_UOT 3000
#define ANHSANG_TOI 3500
#define ANHSANG_SANG 500

// Ngưỡng gửi dữ liệu 
float prev_nhietdo = -1000;
float prev_doamkk = -1000;
int prev_doamdat = -1;
int prev_anhsang = -1;
// lọc trung bình 10 lần 
int readAnalogAverage(int pin, int samples = 10, int delayMs = 5) {
  long total = 0;
  for (int i = 0; i < samples; i++) {
    total += analogRead(pin);
    delay(delayMs);
  }
  return total / samples;
}

// ========== KHỞI TẠO ==========
void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(FAN_PIN, OUTPUT);
  pinMode(DEN_PIN, OUTPUT);
  pinMode(BOM_PIN, OUTPUT);
  pinMode(VALVE_SPRAY_PIN, OUTPUT);
  pinMode(VALVE_WATERING_PIN, OUTPUT);
  pinMode(VALVE_DRAIN_PIN, OUTPUT);

  pinMode(AUTO_PIN, OUTPUT);
  pinMode(MANUAL_PIN, OUTPUT);
  pinMode(ERROR_PIN, OUTPUT);

  WiFi.begin(ssid, password);
  Serial.print("Đang kết nối WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nĐã kết nối WiFi!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  if (Firebase.ready()) {
    Serial.println("Firebase đã kết nối OK!");
  } else {
    Serial.println("Firebase chưa sẵn sàng!");
  }
}

// ========== VÒNG LẶP ==========
void loop() {
  float nhietdo = dht.readTemperature();
  float doamkk = dht.readHumidity();

  int doamdat_raw = readAnalogAverage(DOAM_DAT_PIN);
  int anhsang_raw = readAnalogAverage(ANHSANG_PIN);

  int doamdat_percent = map(doamdat_raw, DOAM_DAT_KHO, DOAM_DAT_UOT, 0, 100);
  doamdat_percent = constrain(doamdat_percent, 0, 100);

  int anhsang_percent = map(anhsang_raw, ANHSANG_TOI, ANHSANG_SANG, 0, 100);
  anhsang_percent = constrain(anhsang_percent, 0, 100);

  if (isnan(nhietdo) || isnan(doamkk)) {
    Serial.println("Lỗi đọc DHT11!");
    digitalWrite(ERROR_PIN, HIGH);
    return;
  } else {
    digitalWrite(ERROR_PIN, LOW);
  }
 // Gửi dữ liệu chỉ khi thay đổi đáng kể
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
if (Firebase.RTDB.getInt(&fbdo, "/autoBtn")) {
  isAuto = (fbdo.intData() == 1);
}
  digitalWrite(AUTO_PIN, isAuto ? HIGH : LOW);
  digitalWrite(MANUAL_PIN, isAuto ? LOW : HIGH);
  
  // Serial hiển thị cảm biến
  Serial.println("===============================");
  Serial.print("Nhiệt độ: "); Serial.print(nhietdo); Serial.println(" °C");
  Serial.print("Độ ẩm không khí: "); Serial.print(doamkk); Serial.println(" %");
  Serial.print("Độ ẩm đất: "); Serial.print(doamdat_percent); Serial.println(" %");
  Serial.print("Ánh sáng: "); Serial.print(anhsang_percent); Serial.println(" %");

  if (isAuto) {
    Serial.println(">>> Chế độ: TỰ ĐỘNG");
    digitalWrite(FAN_PIN, nhietdo > 30 ? HIGH : LOW); 
    digitalWrite(DEN_PIN, anhsang_percent < 30 ? HIGH : LOW); // bat den 
    digitalWrite(BOM_PIN, doamdat_percent < 40 ? HIGH : LOW); 
    digitalWrite(VALVE_SPRAY_PIN, doamkk < 30 ? HIGH : LOW);
    digitalWrite(VALVE_WATERING_PIN, doamdat_percent < 50 ? HIGH : LOW);
    digitalWrite(VALVE_DRAIN_PIN, doamdat_percent > 85 ? HIGH : LOW);
    
    Serial.print("Quạt: "); Serial.println(digitalRead(FAN_PIN) ? "Bật" : "Tắt");
    Serial.print("Đèn: "); Serial.println(digitalRead(DEN_PIN) ? "Bật" : "Tắt");
    Serial.print("Bơm: "); Serial.println(digitalRead(BOM_PIN) ? "Bật" : "Tắt");
    Serial.print("Phun sương: "); Serial.println(digitalRead(VALVE_SPRAY_PIN) ? "Bật" : "Tắt");
    Serial.print("Tưới: "); Serial.println(digitalRead(VALVE_WATERING_PIN) ? "Bật" : "Tắt");
    Serial.print("Xả: "); Serial.println(digitalRead(VALVE_DRAIN_PIN) ? "Bật" : "Tắt");

      // Gửi trạng thái thiết bị lên Firebase khi ở chế độ TỰ ĐỘNG
Firebase.RTDB.setInt(&fbdo, "/thietbi5/quat", digitalRead(FAN_PIN));
Firebase.RTDB.setInt(&fbdo, "/thietbi4/den", digitalRead(DEN_PIN));
Firebase.RTDB.setInt(&fbdo, "/thietbi1/bom", digitalRead(BOM_PIN));
Firebase.RTDB.setInt(&fbdo, "/thietbi2/phun_suong", digitalRead(VALVE_SPRAY_PIN));
Firebase.RTDB.setInt(&fbdo, "/thietbi6/valve_watering", digitalRead(VALVE_WATERING_PIN));
Firebase.RTDB.setInt(&fbdo, "/thietbi3/van_xa", digitalRead(VALVE_DRAIN_PIN));


  } else {
    Serial.println(">>> Chế độ: THỦ CÔNG");
digitalWrite(FAN_PIN, getFirebaseInt("/thietbi5/quat"));
digitalWrite(DEN_PIN, getFirebaseInt("/thietbi4/den"));
digitalWrite(BOM_PIN, getFirebaseInt("/thietbi1/bom"));
digitalWrite(VALVE_SPRAY_PIN, getFirebaseInt("/thietbi2/phun_suong"));
digitalWrite(VALVE_WATERING_PIN, getFirebaseInt("/thietbi6/valve_watering"));
digitalWrite(VALVE_DRAIN_PIN, getFirebaseInt("/thietbi3/van_xa"));

    Serial.print("Quạt: "); Serial.println(digitalRead(FAN_PIN) ? "Bật" : "Tắt");
    Serial.print("Đèn: "); Serial.println(digitalRead(DEN_PIN) ? "Bật" : "Tắt");
    Serial.print("Bơm: "); Serial.println(digitalRead(BOM_PIN) ? "Bật" : "Tắt");
    Serial.print("Phun sương: "); Serial.println(digitalRead(VALVE_SPRAY_PIN) ? "Bật" : "Tắt");
    Serial.print("Tưới: "); Serial.println(digitalRead(VALVE_WATERING_PIN) ? "Bật" : "Tắt");
    Serial.print("Xả: "); Serial.println(digitalRead(VALVE_DRAIN_PIN) ? "Bật" : "Tắt");
  }

  delay(5000);
}

int getFirebaseInt(String path) {
  if (Firebase.RTDB.getInt(&fbdo, path)) {
    return fbdo.intData(); // Trả về 1 hoặc 0
  } else {
    Serial.print("Lỗi đọc Firebase: ");
    Serial.println(path);
    digitalWrite(ERROR_PIN, HIGH);
    return 0; // Mặc định tắt thiết bị nếu lỗi
  }
}
