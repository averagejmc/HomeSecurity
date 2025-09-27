import paho.mqtt.client as mqtt
from datetime import datetime

def on_message(client, userdata, message):
    # Get current timestamp
    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    # Print topic, message, and timestamp
    print(f"[{timestamp}] {message.topic}: {message.payload.decode()}")

client = mqtt.Client()
client.connect("localhost")  # Broker runs on this machine

client.subscribe("home/#")   # Subscribe to all home topics
client.on_message = on_message

print("Listening to MQTT topics...")
client.loop_forever()
