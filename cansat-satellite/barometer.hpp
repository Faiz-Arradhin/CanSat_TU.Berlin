#pragma once
#include <Adafruit_BMP280.h>

class BARO {
public:
  void setup() {
    while(!bmp.begin(0x76)) {
//      Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    }
    initPressure = bmp.readPressure() / 100.0F;
//    Serial.println("BMP280 initialized successfully!");
  }

  float getData() {
    float pressure = bmp.readPressure() / 100.0F;
    float altitude = 44330.0 * (1.0 - pow(pressure / initPressure, 0.1903));
    return altitude;
  }
private:
  Adafruit_BMP280 bmp;
  float initPressure;
};
