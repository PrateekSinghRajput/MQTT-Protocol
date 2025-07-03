#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include "config.h"

// DHT Sensor
DHT dht(DHTPIN, DHTTYPE);

// Switch state
bool switchState = false;
bool lastManualSwitchState = HIGH;

WiFiClient MQTTclient;
PubSubClient client(MQTTclient);

StaticJsonBuffer<256> jsonBuffer;
char sensor_data_format_for_mqtt_publish[256];

unsigned long lastPublishTime = 0;

// Function declarations
void mqttCallback(char* topic, byte* payload, unsigned int length);
void reconnect();
void sendSwitchStateTelemetry();

void setup() {
  Serial.begin(115200);
  pinMode(switchPin, OUTPUT);
  digitalWrite(switchPin, LOW);
  pinMode(manualSwitchPin, INPUT_PULLUP);
  dht.begin();

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  Serial.print("ESP32 IP ADDRESS: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqttBroker, mqttPort);
  client.setCallback(mqttCallback);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to ThingsBoard MQTT...");
    if (client.connect("ESP32Client", accessToken, "")) {
      Serial.println("connected");
      client.subscribe(mqtt_topic_for_subscribe);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");

  char message[length + 1];
  memcpy(message, payload, length);
  message[length] = '\0';
  Serial.println(message);

  JsonObject& doc = jsonBuffer.parseObject(message);

  if (!doc.success()) {
    Serial.println("parseObject() failed");
    return;
  }

  if (doc.containsKey("method") && strcmp(doc["method"], "setSwitch") == 0) {
    bool newState = doc["params"];
    if (newState != switchState) {
      switchState = newState;
      digitalWrite(switchPin, switchState ? HIGH : LOW);
      Serial.print("Switch turned ");
      Serial.println(switchState ? "ON" : "OFF");
      sendSwitchStateTelemetry();
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  bool manualSwitchState = digitalRead(manualSwitchPin);
  if (manualSwitchState != lastManualSwitchState) {
    delay(50);
    if (digitalRead(manualSwitchPin) == manualSwitchState) {
      lastManualSwitchState = manualSwitchState;
      if (manualSwitchState == LOW) {
        switchState = !switchState;
        digitalWrite(switchPin, switchState ? HIGH : LOW);
        Serial.print("Manual switch toggled: ");
        Serial.println(switchState ? "ON" : "OFF");
        sendSwitchStateTelemetry();
      }
    }
  }

  if (millis() - lastPublishTime > publishInterval) {
    lastPublishTime = millis();

    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    JsonObject& data = jsonBuffer.createObject();
    if (!isnan(temperature)) data["temperature"] = temperature;
    if (!isnan(humidity)) data["humidity"] = humidity;
    data["switch"] = switchState;

    data.printTo(sensor_data_format_for_mqtt_publish, sizeof(sensor_data_format_for_mqtt_publish));
    client.publish(mqtt_topic_for_publish, sensor_data_format_for_mqtt_publish);

    Serial.println("Data sent to ThingsBoard:");
    Serial.println(sensor_data_format_for_mqtt_publish);

    jsonBuffer.clear();
  }
}

void sendSwitchStateTelemetry() {
  JsonObject& data = jsonBuffer.createObject();
  data["switch"] = switchState;
  char buffer[64];
  data.printTo(buffer, sizeof(buffer));
  client.publish(mqtt_topic_for_publish, buffer);
  jsonBuffer.clear();
}
