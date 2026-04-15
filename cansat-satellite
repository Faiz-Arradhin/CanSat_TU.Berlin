#pragma once

#include <Wire.h>

#include "barometer.hpp"
#include "mpu.hpp"
#include "temp.hpp"

struct XYZ {
  float x;
  float y;
  float z;
};

class Sensors {
public:
  Sensors() = delete;
  Sensors(int temp_Pin): temp(temp_Pin) {};
  
  void setup() {
    baro.setup();
    mpu.setup();
  }

  void updateFast() {
    mpu.updateFast();
  }
  
  void updateMPU() {
    MPU::Data& mpuVal = mpu.getData();
    acc.x = mpuVal.acc.x;
    acc.y = mpuVal.acc.y;
    acc.z = mpuVal.acc.z;

    angle.x = mpuVal.angle.x;
    angle.y = mpuVal.angle.y;
    angle.z = mpuVal.angle.z;

    tempIn = mpuVal.temp;
  }

  void updateTemp() {
    tempOut = temp.getData();
  }

  void updateBaro() {
    altitude = baro.getData();
  }

//  void sendData();

  XYZ getAcc() {
    return acc;
  }
  
  XYZ getAngle() {
    return angle;
  }
  
  float getTempIn() {
    return tempIn;  
  }
  
  float getTempOut() {
    return tempOut;
  }
  
  float getAltitude() {
    return altitude;
  }
  
private:
  BARO baro;
  MPU mpu;
  TEMP temp;
    
  XYZ acc;
  XYZ angle;
  float tempIn;
  float tempOut;
  float altitude;
};
