#include <WiFi.h>
#include <PubSubClient.h>
#include "config.h"

// MQTT and Wi-Fi clients
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// ---------- Callback when a message is received ----------
void handleIncomingMessage(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Received MQTT message: ");
  Serial.println(message);

  if (message == "BLINK") {
    blinkLED();
    mqttClient.publish(MQTT_ACK_TOPIC, "LED Blinked via System Command");
  }
}

// ---------- Blink the onboard LED ----------
void blinkLED() {
  digitalWrite(D4, HIGH);
  delay(5000);
  digitalWrite(D4, LOW);
}

// ---------- Connect to Wi-Fi ----------
void connectToWiFi() {
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWi-Fi connected");
}

// ---------- Connect to MQTT Broker ----------
void connectToMQTTBroker() {
  while (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT broker...");
    if (mqttClient.connect(MQTT_CLIENT_ID,MQTT_USERNAME, MQTT_PASSWORD)) {
      Serial.println("connected ");
      mqttClient.subscribe(MQTT_COMMAND_TOPIC);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      delay(2000);
    }
  }
}

// ---------- Arduino Setup ----------
void setup() {
  Serial.begin(115200);
  pinMode(D4, OUTPUT);

  connectToWiFi();

  mqttClient.setServer(MQTT_BROKER_ADDRESS, MQTT_BROKER_PORT);
  mqttClient.setCallback(handleIncomingMessage);
}

// ---------- Arduino Main Loop ----------
void loop() {
  if (!mqttClient.connected()) {
    connectToMQTTBroker();
  }
  mqttClient.loop();
}
