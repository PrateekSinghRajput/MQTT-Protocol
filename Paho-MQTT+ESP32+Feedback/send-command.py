import paho.mqtt.client as mqtt
import time
import sys

# MQTT Configuration
BROKER = "192.168.0.112"
PORT = 1883
USERNAME = "prateek"     # Replace with your MQTT username
PASSWORD = "1997"     # Replace with your MQTT password
TOPIC_COMMAND = "esp32/led"
TOPIC_ACK = "esp32/ack"

# Callback when connected to the broker
def on_connect(client, userdata, flags, reason_code, properties=None):
    print("Connection to MQTT broker established.")
    client.subscribe(TOPIC_ACK)

# Callback when a message is received
def on_message(client, userdata, msg):
    message = msg.payload.decode()
    print(f"Acknowledgment received on topic '{msg.topic}': {message}")
    if message == "LED Blinked via System Command":
        print("ESP32 has successfully executed the command.")
        client.disconnect()

# Send the command to ESP32
def send_command(command):
    client = mqtt.Client(callback_api_version=mqtt.CallbackAPIVersion.VERSION2)
    client.username_pw_set(USERNAME, PASSWORD)
    client.on_connect = on_connect
    client.on_message = on_message

    try:
        client.connect(BROKER, PORT)
    except Exception as e:
        print(f"Failed to connect to MQTT broker: {e}")
        sys.exit(1)

    client.loop_start()
    time.sleep(1)  # Wait for the connection to establish

    print(f"Sending command to ESP32: {command}")
    client.publish(TOPIC_COMMAND, command)

    while client.is_connected():
        time.sleep(0.5)

    client.loop_stop()

# Entry point
if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python mqtt_cli_client.py <COMMAND>")
        sys.exit(1)

    command = sys.argv[1]
    send_command(command)
