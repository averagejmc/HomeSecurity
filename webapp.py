from flask import Flask, render_template, request, jsonify
from flask_socketio import SocketIO, emit
import paho.mqtt.client as mqtt
from threading import Timer
import datetime
import requests

app = Flask(__name__)
socketio = SocketIO(app)

# ---------------- Global State ----------------
armed = False
door_open_time = None
ALERT_DELAY_SECONDS = 10

# ---------------- Telegram Config ----------------
TELEGRAM_TOKEN = "7830410011:AAGqiLow-xTWLSzsQDstVh_D-xkSBNmtzYQ"
CHAT_ID = "1265206769"


# ---------------- Alert + RFID ----------------
def send_alert(message):
    url = f"https://api.telegram.org/bot{TELEGRAM_TOKEN}/sendMessage"
    try:
        resp = requests.post(url, json={"chat_id": CHAT_ID, "text": message}, timeout=5)
        if resp.status_code == 200:
            print("✅ Telegram alert sent:", message)
        else:
            print("❌ Telegram failed:", resp.text)
    except Exception as e:
        print("❌ Failed to send Telegram alert:", e)


def log_rfid(uid):
    print(f"Family member RFID detected: {uid} at {datetime.datetime.now()}")


# ---------------- Alarm Timer Functions ----------------
def trigger_alarm():
    if armed:
        send_alert("🚨 Door left open too long while system is armed!")
        socketio.emit(
            "system_alarm",
            {"status": "triggered", "message": "Door left open while armed!"},
        )


def check_door():
    global door_open_time
    while True:
        socketio.sleep(5)
        if door_open_time and (datetime.datetime.now() - door_open_time).seconds > 180:
            send_alert("🚨 Door has been open for more than 3 minutes!")
            door_open_time = None


# ---------------- MQTT Setup ----------------
MQTT_BROKER = "localhost"
MQTT_TOPIC = "home/#"


def on_message(client, userdata, msg):
    global armed, door_open_time
    topic = msg.topic
    payload = msg.payload.decode().strip()

    socketio.emit("mqtt_message", {"data": f"{topic}: {payload}"})

    # Motion
    if topic == "home/motion" and "detected" in payload and armed:
        send_alert(f"🚨 Motion detected at {datetime.datetime.now()}")
        socketio.emit(
            "system_alarm",
            {"status": "warning", "message": "Motion detected while armed."},
        )

    # Door
    if topic == "home/door":
        if "open" in payload:
            if door_open_time is None:
                door_open_time = datetime.datetime.now()
                print("Door opened, monitoring...")
                Timer(ALERT_DELAY_SECONDS, trigger_alarm).start()
        elif "closed" in payload:
            door_open_time = None
            print("Door closed, reset timer.")

    # RFID
    if topic == "home/rfid":
        log_rfid(payload)


mqtt_client = mqtt.Client()
mqtt_client.on_message = on_message
mqtt_client.connect(MQTT_BROKER, 1883)
mqtt_client.subscribe(MQTT_TOPIC)
mqtt_client.loop_start()


# ---------------- Flask Routes ----------------
@app.route("/")
def index():
    return render_template("index.html")


@app.route("/toggle_security", methods=["POST"])
def toggle_security():
    global armed, door_open_time
    data = request.get_json()
    new_armed_status = data.get("armed", False)

    if isinstance(new_armed_status, bool):
        armed = new_armed_status
        if not armed:
            door_open_time = None
            print("Security system DISARMED.")
            send_alert("🔓 Security system DISARMED.")
        else:
            print("Security system ARMED.")
            send_alert("🔒 Security system ARMED.")
        return jsonify({"status": "ok", "armed": armed})
    else:
        return jsonify({"status": "error", "message": "Invalid 'armed' value"}), 400


# ---------------- Run App ----------------
if __name__ == "__main__":
    socketio.start_background_task(check_door)
    socketio.run(app, host="0.0.0.0", port=5000, debug=True)
