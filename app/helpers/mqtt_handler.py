import datetime
import paho.mqtt.client as mqtt
from threading import Timer
from app.helpers.tg_alerts import send_alert
from app.helpers import state_vars
from app.helpers.sensor_values import motion, rfid, door


def init_mqtt(socketio):
    MQTT_BROKER = "localhost"
    MQTT_TOPIC = "home/#"

    def trigger_alarm():
        if state_vars.armed:
            send_alert("🚨 Door left open too long while system is armed!")

    def log_rfid(uid):
        print(f"Family member RFID detected: {uid} at {datetime.datetime.now()}")

    def on_message(client, userdata, msg):
        try:
            topic = msg.topic
            payload = msg.payload.decode().strip()
            print(f"DEBUG: received mqtt message: {topic}:{payload}")

            socketio.emit("mqtt_message", {"data": f"{topic}:{payload}"})

            if topic == "home/motion":
                motion.set(payload)
                if payload == "Motion detected!" and state_vars.armed:
                    send_alert(f"🚨 Motion detected at {datetime.datetime.now()}")

            elif topic == "home/door":
                door.set(payload)
                if payload == "Door open":
                    if state_vars.door_open_time is None:
                        state_vars.door_open_time = datetime.datetime.now()
                        print("Door opened. Timer started.")
                        Timer(state_vars.ALERT_DELAY_SECONDS, trigger_alarm).start()
                
                elif payload == "Door closed!":
                    if state_vars.door_open_time is not None:
                        print("Door closed. Timer reset.")
                        state_vars.door_open_time = None

            elif topic == "home/rfid":
                rfid.set(payload)
                if payload != "No RFID":
                    log_rfid(payload)

            elif topic == "home/mode":
                mode_map = {"0": "auto", "1": "maintenance", "2": "sleep"}
                if payload in mode_map:
                    new_mode = mode_map[payload]
                    if state_vars.mode != new_mode:
                        state_vars.mode = new_mode
                        print(f"Mode updated to: {new_mode}")
                        socketio.emit("mode_update", {"mode": state_vars.mode})

        except Exception as e:
            print(f"Error parsing MQTT message: {e}")

    mqtt_client = mqtt.Client()
    mqtt_client.on_message = on_message
    mqtt_client.connect(MQTT_BROKER, 1883)
    mqtt_client.subscribe(MQTT_TOPIC)
    mqtt_client.loop_start()
