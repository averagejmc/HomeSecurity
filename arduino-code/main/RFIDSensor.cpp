#include "RFIDSensor.h"

RFIDSensor::RFIDSensor(byte ssPin, byte rstPin)
  : _mfrc522(ssPin, rstPin) {}

void RFIDSensor::begin() {
  SPI.begin();
  _mfrc522.PCD_Init();

  for (byte i = 0; i < 6; i++) {
    _key.keyByte[i] = 0xFF;
  }
}

String RFIDSensor::read() {
  if (!_mfrc522.PICC_IsNewCardPresent() || !_mfrc522.PICC_ReadCardSerial()) {
    return "";
  }

  String uidString = "";
  for (byte i = 0; i < _mfrc522.uid.size; i++) {
    if (_mfrc522.uid.uidByte[i] < 0x10) uidString += "0";
    uidString += String(_mfrc522.uid.uidByte[i], HEX);
  }
  uidString.toUpperCase();

  _mfrc522.PICC_HaltA();
  _mfrc522.PCD_StopCrypto1();

  return uidString;
}
