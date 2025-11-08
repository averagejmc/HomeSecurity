import paho.mqtt.client as mqtt
from datetime import datetime
from logger import log_event


def on_message(client, userdata, message):
    # Get current timestamp
    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    # Safely decode payload
    try:
        payload = message.payload.decode()
    except Exception:
        payload = message.payload.decode(errors="replace")

    # Print topic, message, and timestamp
    print(f"[{timestamp}] {message.topic}: {payload}")

    # Log to database (topic as event_type, payload as message)
    try:
        log_event(message.topic, payload)
    except Exception as e:
        # Avoid crashing the loop; print error for debugging
        print(f"Failed to log event to DB: {e}")


# Use the newer callback API if available to avoid DeprecationWarning
try:
    client = mqtt.Client(callback_api_version=2)
except TypeError:
    client = mqtt.Client()

client.connect("localhost", 1883, 60)  # Broker runs on this machine

client.subscribe("home/#")  # Subscribe to all home topics
client.on_message = on_message

print("Listening to MQTT topics...")
client.loop_forever()
