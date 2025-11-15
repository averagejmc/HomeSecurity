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
            socketio.emit("system_alarm", {"status": "triggered"})

    def log_rfid(uid):
        print(f"Family member RFID detected: {uid} at {datetime.datetime.now()}")


    def on_message(client, userdata, msg):
        topic = msg.topic
        payload = msg.payload.decode().strip()

        socketio.emit("mqtt_message", {"data": f"{topic}: {payload}"})

        # Motion
        if topic == "home/motion" and "detected" in payload and state_vars.armed:
            send_alert(f"🚨 Motion detected at {datetime.datetime.now()}")
            socketio.emit(
                "system_alarm",
                {"status": "warning", "message": "Motion detected while armed."},
            )
            motion.set(payload["value"])

        # Door
        if topic == "home/door":
            if "open" in payload:
                if state_vars.door_open_time is None:
                    state_vars.door_open_time = datetime.datetime.now()
                    print("Door opened, monitoring...")
                    Timer(state_vars.ALERT_DELAY_SECONDS, trigger_alarm).start()
                    door.set(payload["value"])
            elif "closed" in payload:
                state_vars.door_open_time = None
                print("Door closed, reset timer.")
                door.set(payload["value"])

        # RFID
        if topic == "home/rfid":
            log_rfid(payload)
            rfid.set(payload["value"])


    mqtt_client = mqtt.Client()
    mqtt_client.on_message = on_message
    mqtt_client.connect(MQTT_BROKER, 1883)
    mqtt_client.subscribe(MQTT_TOPIC)
    mqtt_client.loop_start()