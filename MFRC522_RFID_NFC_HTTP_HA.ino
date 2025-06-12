#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <MFRC522.h>

// === Wi-Fi credentials ===
const char* ssid = "";
const char* password = "";

// === Home Assistant Webhook endpoint ===
const char* webhook_url = "http:///api/webhook/rfid_trigger";

// === SPI Pin Assignments for Xiao ===
#define RST_PIN 3
#define SS_PIN  7

MFRC522 mfrc522(SS_PIN, RST_PIN);

// === Setup ===
void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected.");

  Serial.println("Initializing RFID...");
  SPI.begin(8, 9, 10); // SCK, MISO, MOSI
  mfrc522.PCD_Init();
  Serial.println("Ready to scan tags.");
}

// === Send UID to Home Assistant via Webhook ===
void sendWebhook(String uid) {
  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;
  http.begin(webhook_url);
  http.addHeader("Content-Type", "application/json");

  String payload = "{\"uid\": \"" + uid + "\"}";
  int code = http.POST(payload);
  Serial.print("Sent UID: "); Serial.print(uid);
  Serial.print(" | HTTP Response: "); Serial.println(code);
  http.end();
}

// === Main loop ===
void loop() {
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) return;

  String uid = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) uid += "0";
    uid += String(mfrc522.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();

  Serial.println("Scanned UID: " + uid);
  sendWebhook(uid);

  delay(2000); // debounce
  mfrc522.PICC_HaltA();
}
