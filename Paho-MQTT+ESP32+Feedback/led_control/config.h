#ifndef CONFIG_H
#define CONFIG_H

// Wi-Fi Configuration
const char* WIFI_SSID = "Prateek";          // Replace with your Wi-Fi name
const char* WIFI_PASSWORD = "justdoelectronics@#12345";  // Replace with your Wi-Fi password

// MQTT Broker Configuration
const char* MQTT_BROKER_ADDRESS = "192.168.0.112";
const int MQTT_BROKER_PORT = 1883;
const char* MQTT_CLIENT_ID = "ESP32Client";

const char* MQTT_USERNAME = "prateek";  // Required for secured brokers
const char* MQTT_PASSWORD = "1997";

// MQTT Topics
const char* MQTT_COMMAND_TOPIC = "esp32/led";
const char* MQTT_ACK_TOPIC = "esp32/ack";

#endif
