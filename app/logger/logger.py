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
            rfid TEXT,
            mode TEXT
        )"""
    )
    conn.commit()
    conn.close()

    print("logger: database initialized")


# Function to log an event
def log_event():
    from app import socketio

    while True:
        socketio.sleep(600)

        from app.helpers.sensor_values import motion, rfid, door
        from app.helpers import state_vars

        conn = sqlite3.connect(DB_PATH)
        c = conn.cursor()
        c.execute(
            "INSERT INTO snapshots (armed, door_open_time_seconds, motion, door, rfid, mode) VALUES (?, ?, ?, ?, ?, ?)",
            (
                state_vars.armed,
                state_vars.door_open_time,
                motion.get(),
                door.get(),
                rfid.get(),
                state_vars.mode,
            ),
        )
        conn.commit()
        conn.close()

        print("logger: event logged")


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
