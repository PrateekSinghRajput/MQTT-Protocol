#include <WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <time.h>
#include "config.h"   // Include your config file

MFRC522 rfid(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);

WiFiClient espClient;
PubSubClient client(espClient);

struct User {
  String uid;
  String name;
  String roll;
  bool checkedInToday;  
  bool checkedOutToday; 
  int lastAttendanceDay; 
};

User validUsers[] = {
  {"E36DA613", "Prateek", "551", false, false, -1},
  {"66767D3D", "Rohan", "552", false, false, -1},
  {"DEADBEEF", "Sonu", "553", false, false, -1}
};

const int validUserCount = sizeof(validUsers) / sizeof(validUsers[0]);

int getUserIndexByUID(String uid) {
  for (int i = 0; i < validUserCount; i++) {
    if (uid == validUsers[i].uid) {
      return i;
    }
  }
  return -1;
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi");
  int wifi_attempts = 0;

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    lcd.setCursor(0, 1);
    lcd.print("Connecting...");
    wifi_attempts++;
    if (wifi_attempts > 40) { 
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("WiFi Failed!");
      Serial.println("\nWiFi connection failed");
      break;
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Connected");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());
    delay(2000);
  }
}

void syncTime() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  Serial.println("Waiting for time");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) { 
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println();
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}

int getCurrentDay() {
  time_t now = time(nullptr);
  struct tm timeinfo;
  localtime_r(&now, &timeinfo);
  return timeinfo.tm_mday;
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("MQTT Connecting");
    if (client.connect("ESP32Client", ACCESS_TOKEN, NULL)) {
      Serial.println("connected");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("MQTT Connected");
      digitalWrite(MQTT_LED_PIN, HIGH); 
      delay(1500);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("MQTT Failed");
      lcd.setCursor(0, 1);
      lcd.print("Retrying...");
      digitalWrite(MQTT_LED_PIN, LOW);  
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  SPI.begin();
  rfid.PCD_Init();

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Starting...");

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(MQTT_LED_PIN, OUTPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);

  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(MQTT_LED_PIN, LOW);
  digitalWrite(STATUS_LED_PIN, LOW);

  setup_wifi();
  syncTime();

  client.setServer(THINGSBOARD_SERVER, THINGSBOARD_PORT);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Place your card");
  lcd.setCursor(0, 1);
  lcd.print("near reader...");
  Serial.println("Place your RFID card near the reader...");
}

void loop() {
  if (!client.connected()) {
    digitalWrite(MQTT_LED_PIN, LOW);  
    reconnect();
  }
  client.loop();

  int today = getCurrentDay();

  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    String uidStr = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
      if (rfid.uid.uidByte[i] < 0x10) uidStr += "0";
      uidStr += String(rfid.uid.uidByte[i], HEX);
    }
    uidStr.toUpperCase();

    Serial.print("RFID UID detected: ");
    Serial.println(uidStr);

    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(MQTT_LED_PIN, digitalRead(MQTT_LED_PIN)); 
    digitalWrite(STATUS_LED_PIN, HIGH);

    int userIndex = getUserIndexByUID(uidStr);

    if (userIndex >= 0) {
      // Reset daily attendance if day changed
      if (validUsers[userIndex].lastAttendanceDay != today) {
        validUsers[userIndex].checkedInToday = false;
        validUsers[userIndex].checkedOutToday = false;
        validUsers[userIndex].lastAttendanceDay = today;
      }

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(validUsers[userIndex].name);
      lcd.setCursor(0, 1);

      String attendanceStatus;
      String inOutStatus;

      if (!validUsers[userIndex].checkedInToday) {
        // Mark IN
        validUsers[userIndex].checkedInToday = true;
        attendanceStatus = "IN";
        inOutStatus = "IN";
        lcd.print("Status:     IN");
      } else if (!validUsers[userIndex].checkedOutToday) {
        // Mark OUT
        validUsers[userIndex].checkedOutToday = true;
        attendanceStatus = "OUT";
        inOutStatus = "OUT";
        lcd.print("Status:     OUT");
      } else {
        // Already checked IN and OUT today
        attendanceStatus = "DONE";
        inOutStatus = "DONE";
        lcd.print("Already IN & OUT");
      }

      lcd.setCursor(0, 1);
      if (attendanceStatus != "DONE") {
        lcd.print("Roll: " + validUsers[userIndex].roll);
      }

      Serial.print("Attendance Status: ");
      Serial.println(attendanceStatus);

      if (attendanceStatus != "DONE") {
        String payload = "{";
        payload += "\"rfid_uid\":\"" + uidStr + "\",";
        payload += "\"name\":\"" + validUsers[userIndex].name + "\",";
        payload += "\"roll\":\"" + validUsers[userIndex].roll + "\",";
        payload += "\"status\":\"" + inOutStatus + "\"";
        payload += "}";

        if (client.publish("v1/devices/me/telemetry", payload.c_str())) {
          Serial.println("Attendance sent to ThingsBoard");
          delay(2000);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Attendance Sent");
          lcd.setCursor(0, 1);
          lcd.print("Thank You!");
        } else {
          Serial.println("Failed to send attendance data");
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Send Failed!");
          lcd.setCursor(0, 1);
          lcd.print("Try Again");
        }
      } else {
        delay(2000);
      }
    } else {
      Serial.println("Not a Valid Card");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Not a Valid");
      lcd.setCursor(0, 1);
      lcd.print("Card!");
      delay(2000);
    }

    delay(1000);

    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(STATUS_LED_PIN, LOW);

    rfid.PICC_HaltA();
    delay(2000);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Place your card");
    lcd.setCursor(0, 1);
    lcd.print("near reader...");
  }
}
