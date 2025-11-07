# HomeSecurity

Small local home security demo app. It receives MQTT messages from sensors, logs events to a local SQLite database (`security.db`), and exposes a small Flask+SocketIO web UI.

## Requirements

- Python 3.10+ (project uses a virtualenv at `.venv`)
- Mosquitto MQTT broker (or any MQTT broker reachable at `localhost:1883`)

## Install (Windows, PowerShell)

1. Create & activate the venv (if not already created):

```powershell
python -m venv .venv
.\.venv\Scripts\Activate.ps1
```

2. Install dependencies:

```powershell
pip install -r requirements.txt
```

## Running the services

- Start the MQTT broker (Mosquitto):
	- Download from https://mosquitto.org/download/ and install. If you installed the Mosquitto service, it may already be running.
	- If you installed Mosquitto but the `mosquitto` command is not in PATH, start it from the install folder (for example `C:\Program Files\mosquitto\mosquitto.exe`).

- Start the MQTT-to-DB listener:

```powershell
.\.venv\Scripts\python.exe home.py
```

- Start the web app (Flask + SocketIO):

```powershell
.\.venv\Scripts\python.exe webapp.py
```

By default the web UI listens on http://0.0.0.0:5000 — open http://localhost:5000 in your browser.

## Secure the Telegram token

The repo currently contains a Telegram token in `webapp.py`. Do NOT commit real tokens. Prefer using an environment variable instead.

Set the token for the current PowerShell session:

```powershell
$env:TELEGRAM_TOKEN = "<your-token-here>"
```

Or set it persistently for your user (PowerShell):

```powershell
setx TELEGRAM_TOKEN "<your-token-here>"
```

Then modify `webapp.py` to read the token from the environment (example already added in the code base is straightforward). You can also use a `.env` file and `python-dotenv` if you prefer.

## Quick checks

- Check the web server is listening on 5000 (PowerShell):

```powershell
C:/path/to/.venv/Scripts/python.exe -c "import socket; s=socket.socket(); print(s.connect_ex(('127.0.0.1',5000))); s.close()"
```

- Query recent logs from the SQLite DB:

```powershell
.\.venv\Scripts\python.exe -c "import sqlite3; conn=sqlite3.connect('security.db'); c=conn.cursor(); c.execute('SELECT id,event_type,message,timestamp FROM logs ORDER BY id DESC LIMIT 10'); print(c.fetchall()); conn.close()"
```

## Notes & next steps

- For production use don't expose the Flask dev server; run behind a proper WSGI server and secure access.
- Consider moving secrets into environment variables or a secrets manager.
- If sensor payloads are JSON, consider parsing and storing structured fields in the DB.

---

If you'd like, I can update `webapp.py` to read `TELEGRAM_TOKEN` from the environment and remove the hard-coded token.
