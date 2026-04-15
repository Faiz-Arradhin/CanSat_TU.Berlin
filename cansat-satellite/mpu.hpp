#pragma once

#include <MPU6050_light.h>

class MPU {
public:
  struct Coordinate3D {
    float x;
    float y;
    float z;
  };

  struct Data {
    float temp;
    Coordinate3D acc;
    Coordinate3D angle;
  };

  MPU() : mpu(Wire) {}
  
  void setup() {
    byte status;

    while (status != 0) {
      status = mpu.begin();
//      Serial.print(F("MPU6050 status: "));
//      Serial.println(status);
    }
  
//    Serial.println(F("Calculating offsets, do not move MPU6050"));
    mpu.calcOffsets(true, true); // gyro and accelero
//    Serial.println("MPU SET!\n");
  }

  void updateFast() {
    mpu.update();
  }

  Data& getData() {
    reading.temp = mpu.getTemp();
    reading.acc.x = mpu.getAccX();
    reading.acc.y = mpu.getAccY();
    reading.acc.z = mpu.getAccZ();
    reading.angle.x = mpu.getAngleX();
    reading.angle.y = mpu.getAngleY();
    reading.angle.z = mpu.getAngleZ();

    return reading;
  }


private:
  MPU6050 mpu;
  Data reading;
};
