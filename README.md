# HomeSecurity 

A small local home security system that:

* receives sensor data via **MQTT**
* shows a real-time dashboard using **Flask + SocketIO**
* logs hourly snapshots to a **SQLite** database
* sends notifications through **Telegram Bot API**

The app runs at **[http://localhost:5000](http://localhost:5000)**.

---

## Requirements

* Python 3.10+
* **uv** (for virtualenv + dependency management)
* Mosquitto (or any MQTT broker at `localhost:1883`)
* Telegram bot token stored in an environment variable
  `TELEGRAM_TOKEN=<your token>`

---

## Installation

### 1. Create and activate the virtual environment

```bash
uv venv
source .venv/bin/activate   # Linux/macOS
```

Windows PowerShell:

```powershell
.\.venv\Scripts\Activate.ps1
```

### 2. Install all dependencies

```bash
uv sync
```

---

## Configure Telegram

For this project, we included the bot token in the code.
However, it is recommended to use environment variables 
instead for API and bot tokens.

Set the bot token as an environment variable:

Linux/macOS:

```bash
export TELEGRAM_TOKEN="your-token"
```

Windows PowerShell:

```powershell
setx TELEGRAM_TOKEN "your-token"
```

---

## Running the App

### 1. Start the MQTT broker

```bash
mosquitto
```

### 2. Run the web server

```bash
uv run webapp.py
```

Then open:

```
http://localhost:5000
```

---
