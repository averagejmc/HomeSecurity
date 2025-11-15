import datetime
from app.helpers.tg_alerts import send_alert
from app.helpers import state_vars
from app import socketio


def check_door():
    while True:
        socketio.sleep(5)
        if state_vars.door_open_time and (datetime.datetime.now() - state_vars.door_open_time).seconds > 180:
            send_alert("🚨 Door has been open for more than 3 minutes!")
            state_vars.door_open_time = None