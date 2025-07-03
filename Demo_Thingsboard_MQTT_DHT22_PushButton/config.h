#ifndef CONFIG_H
#define CONFIG_H

// WiFi Credentials
const char* ssid = "Prateek";
const char* password = "justdoelectronics@#12345";

// MQTT Broker Settings
const char* mqttBroker = "demo.thingsboard.io";
const int mqttPort = 1883;
const char* accessToken = "vypuqubzxn9oht2wne7y";

// MQTT Topics
const char* mqtt_topic_for_publish = "v1/devices/me/telemetry";
const char* mqtt_topic_for_subscribe = "v1/devices/me/rpc/request/+";

// DHT Sensor
#define DHTPIN 5
#define DHTTYPE DHT22

// Switch Pins
const int switchPin = 13;
const int manualSwitchPin = 26;

// Publish interval (ms)
const unsigned long publishInterval = 5000;

#endif
