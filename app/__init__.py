from flask import Flask
from flask_socketio import SocketIO
from app.helpers.mqtt_handler import init_mqtt

socketio = SocketIO()

def create_app():
    app = Flask(__name__)

    from app.routes.routes import main_bp
    app.register_blueprint(main_bp)

    socketio.init_app(app)
    init_mqtt(socketio)

    return app
