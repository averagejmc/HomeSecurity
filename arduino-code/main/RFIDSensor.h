<<<<<<< HEAD
#ifndef RFIDSENSOR_H
#define RFIDSENSOR_H

#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

class RFIDSensor {
public:
  RFIDSensor(byte ssPin, byte rstPin);
  void begin();
  String read();

private:
  MFRC522 _mfrc522;
  MFRC522::MIFARE_Key _key;
};

#endif
=======
#ifndef RFIDSENSOR_H
#define RFIDSENSOR_H

#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

class RFIDSensor {
public:
  RFIDSensor(byte ssPin, byte rstPin);
  void begin();
  String read();

private:
  MFRC522 _mfrc522;
  MFRC522::MIFARE_Key _key;
};

#endif
>>>>>>> 27dc04e0428cb78814e1be60a14c747cebfc24e0
