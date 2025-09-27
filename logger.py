import sqlite3

# Initialize database + table if not exists
def init_db():
    conn = sqlite3.connect("security.db")
    c = conn.cursor()
    c.execute(
        """CREATE TABLE IF NOT EXISTS logs (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            event_type TEXT,
            message TEXT,
            timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
        )"""
    )
    conn.commit()
    conn.close()

# Function to log an event
def log_event(event_type, message):
    conn = sqlite3.connect("security.db")
    c = conn.cursor()
    c.execute(
        "INSERT INTO logs (event_type, message, timestamp) VALUES (?, ?, datetime('now'))",
        (event_type, message)
    )
    conn.commit()
    conn.close()
    print(f"📒 Logged: {event_type} - {message}")

# Run init on import
init_db()
