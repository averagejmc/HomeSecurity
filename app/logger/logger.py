import sqlite3
import os

DB_PATH = os.path.join(os.path.dirname(__file__), "..", "database", "security.db")


# Initialize database + table if not exists
def init_db():
    conn = sqlite3.connect(DB_PATH)
    c = conn.cursor()
    c.execute(
        """CREATE TABLE IF NOT EXISTS snapshots (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
            armed INTEGER,                 -- 0 or 1
            door_open_time_seconds INTEGER,
            motion TEXT,
            door TEXT,
            rfid TEXT
        )"""
    )
    conn.commit()
    conn.close()


# Function to log an event
def log_event():
    while True:
        from app.helpers.sensor_values import motion, rfid, door
        from app.helpers import state_vars

        conn = sqlite3.connect(DB_PATH)
        c = conn.cursor()
        c.execute(
            "INSERT INTO snapshots (armed, door_open_time, motion, door, rfid) VALUES (?, ?, ?, ?, ?, ?)",
            (
                state_vars.armed,
                state_vars.door_open_time,
                motion.get(),
                door.get(),
                rfid.get(),
            ),
        )
        conn.commit()
        conn.close()

        from flask_socketio import SocketIO

        SocketIO.sleep(3600)


# Function to retrieve logs
def get_logs():
    conn = sqlite3.connect(DB_PATH)
    conn.row_factory = sqlite3.Row  # allows dict-like row access
    c = conn.cursor()

    c.execute("SELECT * FROM snapshots ORDER BY timestamp DESC")

    rows = c.fetchall()
    conn.close()

    # Convert SQLite rows to Python dicts
    return [dict(row) for row in rows]


# Run init on import
init_db()
