#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include "config.h"

DHT dht(DHTPIN, DHTTYPE);

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client", ACCESS_TOKEN, NULL)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  setup_wifi();
  client.setServer(THINGSBOARD_SERVER, THINGSBOARD_PORT);
  pinMode(GAS_SENSOR_PIN, INPUT);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("⚠️ Failed to read from DHT sensor!");
    delay(2000);
    return;
  }

  int gasRaw = analogRead(GAS_SENSOR_PIN);
  float gasPercentage = (gasRaw / 4095.0) * 100.0;

  String payload = "{";
  payload += "\"temperature\":"; payload += temperature; payload += ",";
  payload += "\"humidity\":"; payload += humidity; payload += ",";
  payload += "\"air_quality_raw\":"; payload += gasPercentage;
  payload += "}";

  client.publish("v1/devices/me/telemetry", (char*) payload.c_str());

  Serial.print("Sent to ThingsBoard: ");
  Serial.println(payload);

  delay(2000);
}
