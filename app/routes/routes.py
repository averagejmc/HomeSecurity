from flask import Blueprint, render_template, request, jsonify
from app.helpers.tg_alerts import send_alert
from app.helpers import state_vars
from app.logger.logger import get_logs
from app.helpers.sensor_values import ping_sensors

main_bp = Blueprint("main", __name__)


@main_bp.route("/")
def index():
    return render_template("index.html")


@main_bp.route("/toggle_security", methods=["POST"])
def toggle_security():
    data = request.get_json()
    new_armed_status = data.get("armed", False)

    if isinstance(new_armed_status, bool):
        state_vars.armed = new_armed_status
        if not state_vars.armed:
            door_open_time = None
            print("Security system DISARMED.")
            send_alert("🔓 Security system DISARMED.")
        else:
            print("Security system ARMED.")
            send_alert("🔒 Security system ARMED.")

        return jsonify({"status": "ok", "armed": state_vars.armed})

    else:
        return jsonify({"status": "error", "message": "Invalid 'armed' value"}), 400


@main_bp.route("/logs", methods=["GET"])
def log_data():
    logs = get_logs()
    return jsonify({"status": "ok", "logs": logs})


@main_bp.route("/ping", methods=["GET"])
def ping():
    data = ping_sensors()
    return jsonify(data)
