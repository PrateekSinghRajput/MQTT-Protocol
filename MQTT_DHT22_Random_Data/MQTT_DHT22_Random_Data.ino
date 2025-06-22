#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "acts";         
const char* password = "";        

const char* mqttBroker = "demo.thingsboard.io";
const int mqttPort = 1883;        
const char* accessToken = "vypuqubzxn9oht2wne7y"; 

const char* mqtt_topic_for_publish = "v1/devices/me/telemetry";

WiFiClient MQTTclient;
PubSubClient client(MQTTclient);

DynamicJsonDocument sensor_data_payload(256);
char sensor_data_format_for_mqtt_publish[256];

void setup() {
  Serial.begin(115200);
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
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to ThingsBoard MQTT...");
    // Use access token as username, password is empty
    if (client.connect("ESP32Client", accessToken, "")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  float temperature = random(-16, 56);
  float humidity = random(0, 100);

  sensor_data_payload["temperature"] = temperature;
  sensor_data_payload["humidity"] = humidity;
  serializeJson(sensor_data_payload, sensor_data_format_for_mqtt_publish);

  client.publish(mqtt_topic_for_publish, sensor_data_format_for_mqtt_publish);
  Serial.println("Data sent to ThingsBoard:");
  Serial.println(sensor_data_format_for_mqtt_publish);

  delay(5000); 
}