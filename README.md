# MQTT-Protocol 📡

Welcome to the **MQTT-Protocol** repository! This project demonstrates the use of the MQTT protocol with ESP32 devices and sensors. Below you will find an introduction to MQTT and explanations of its basic concepts with helpful emojis to make it easy to understand. 🚀

---

## What is MQTT? 🤔

**MQTT** stands for *Message Queuing Telemetry Transport*. It is a lightweight, publish/subscribe messaging protocol designed for connecting devices in the Internet of Things (IoT) 🌐. It enables efficient, reliable, and scalable communication between sensors, embedded devices, and servers over the internet.

MQTT is widely used in IoT applications because it is simple, bandwidth-efficient, and supports real-time messaging. It works by sending messages between clients through a central broker.

---

## Core MQTT Concepts Explained 🔑

### Broker 🕸️
The **broker** is the central server that receives all messages, filters them, and distributes them to the subscribed clients. It acts like a post office, routing messages to the right recipients.

### Client 🤖
An **MQTT client** can be any device or software that connects to the broker. Clients can be:
- **Publishers**: Send messages to the broker.
- **Subscribers**: Receive messages from the broker by subscribing to topics.

### Topic 📚
A **topic** is a UTF-8 string that acts like an address or channel for messages. Topics are hierarchical and use forward slashes `/` to separate levels, for example:
prateek/home/temperature
Publishers send messages to specific topics, and subscribers receive messages from topics they subscribe to.

### Publish 📤
**Publishing** means sending a message to a topic. For example, a temperature sensor publishes the current temperature to the topic `home/livingroom/temperature`.

### Subscribe 📥
**Subscribing** means registering interest in a topic to receive messages published on it. For example, a display device subscribes to `home/livingroom/temperature` to show the latest temperature.

### QoS (Quality of Service) ⚖️
MQTT supports 3 levels of message delivery assurance:
- **QoS 0**: At most once delivery (fire and forget)
- **QoS 1**: At least once delivery (message is retried until acknowledged)
- **QoS 2**: Exactly once delivery (guaranteed no duplicates)

### Retained Messages 📌
A **retained message** is stored by the broker and sent immediately to any new subscribers of the topic, ensuring they get the last known value.

### Last Will and Testament (LWT) ⚰️
LWT is a message that the broker sends on behalf of a client if it disconnects unexpectedly, useful for notifying other clients about offline devices.

---

## How MQTT Works - Summary 🔄

1. Clients connect to the **broker**.
2. Publishers send messages to **topics**.
3. Subscribers receive messages from topics they are subscribed to.
4. The broker manages message routing and delivery based on subscriptions.

---

## Repository Contents 📂

- **ESP32_C3_With_DHT22_Sensor**: Example code for ESP32-C3 with a DHT22 temperature and humidity sensor.
- **Air_Quality_ESP32_Thingsboard**: Example for air quality monitoring with ESP32 and ThingsBoard integration.
- **README.md**: This file explaining the project and MQTT basics.

---

Happy IoT coding! 🤖✨
