#include <WiFi.h>
#include <PubSubClient.h>
#include "config.h"
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define DHTPIN D1
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal_I2C lcd(0x27, 16, 2);

WiFiClient espClient;
PubSubClient client(espClient);

void setupWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");
}

void reconnectMQTT() {
  
  while (!client.connected()) {
    Serial.print("Connecting to Adafruit IO...");
    if (client.connect(AIO_USERNAME, AIO_USERNAME, AIO_KEY)) {
      Serial.println(" connected");
    } else {
      Serial.print(" failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  
  Serial.begin(115200);
  dht.begin();
  lcd.init();
  lcd.backlight();
  setupWiFi();
  client.setServer(AIO_SERVER, AIO_SERVERPORT);
}

void loop() {
  
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sensor Error");
    delay(2000);
    return;
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature, 1);
  lcd.print(" C");

  lcd.setCursor(0, 1);
  lcd.print("Hum: ");
  lcd.print(humidity, 1);
  lcd.print(" %");

  // Publish to Adafruit IO feeds
  char tempStr[8], humStr[8];
  dtostrf(temperature, 1, 2, tempStr);
  dtostrf(humidity, 1, 2, humStr);

  if (client.publish(FEED_TEMPERATURE, tempStr)) {
    Serial.println("Published temperature: " + String(tempStr));
  } else {
    Serial.println("Publish temp failed");
  }

  if (client.publish(FEED_HUMIDITY, humStr)) {
    Serial.println("Published humidity: " + String(humStr));
  } else {
    Serial.println("Publish hum failed");
  }

  delay(15000); // 15 seconds delay
}
