class Sensor:
    def __init__(self, value=None):
        self.value = value

    def set(self, v):
        self.value = v

    def get(self):
        return self.value


motion = Sensor("No motion")
rfid = Sensor("No RFID")
door = Sensor("Door closed!")

def ping_sensors():
    latest_values = {
                "motion": motion.get(),
                "door": door.get(),
                "rfid": rfid.get()
            }
    
    return latest_values