import requests

TELEGRAM_TOKEN = "7830410011:AAGqiLow-xTWLSzsQDstVh_D-xkSBNmtzYQ"
CHAT_ID = "1265206769"


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
