class Sensor:
    def __init__(self, value=None):
        self.value = value

    def set(self, v):
        self.value = v

    def get(self):
        return self.value
    

motion = Sensor("Unknown")
rfid = Sensor("None")
door = Sensor("Unknown")