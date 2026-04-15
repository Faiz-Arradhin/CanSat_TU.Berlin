#pragma once

#include <Servo.h>

class SERVO {
public:
  SERVO() = delete;
  SERVO(int servo_Pin): servoPin(servo_Pin) {};

  void setup() {
    servo.attach(servoPin);
  }

  void write(int newPos) {
    pos = newPos;
    servo.write(pos);
  }
  
private:
  Servo servo;
  int servoPin;
  int pos = 0;
};
