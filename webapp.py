from app import create_app, socketio
from app.logger.logger import log_event

app = create_app()

# ---------------- Run App ----------------
if __name__ == "__main__":
    from app.helpers.door_monitor import check_door

    socketio.start_background_task(check_door)
    socketio.start_background_task(log_event)
    socketio.run(app, host="0.0.0.0", port=5000, debug=True)
