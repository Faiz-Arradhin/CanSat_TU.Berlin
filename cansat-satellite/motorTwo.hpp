#pragma once

#include "motor.hpp"

class MotorTwo {
public:
    MotorTwo(int motor1a, int motor1b, int motor2a, int motor2b) : 
      motor1(motor1a, motor1b), motor2(motor2a, motor2b) {}

    void setup() {
        motor1.setup();
        motor2.setup();
        stop();
    }

    // speed: 0–255
    void forward(int speed = 255) {
        motor1.forward(speed);
        motor2.forward(speed);
    }

    void reverse(int speed = 255) {
        motor1.reverse(speed);
        motor2.reverse(speed);
    }

    void FRight(int speed = 255) {
      motor1.forward(speed);
      motor2.stop();
    }

    void FLeft(int speed = 255) {
      motor1.stop();
      motor2.forward(speed);
    }

    void RRight(int speed = 255) {
      motor1.reverse(speed);
      motor2.stop();
    }

    void RLeft(int speed = 255) {
      motor1.stop();
      motor2.reverse(speed);
    }

    void stop() {  // coast
        motor1.stop();
        motor2.stop();
    }

    void brake() { // active brake
        motor1.brake();
        motor2.brake();
    }

private:
    MOTOR motor1;
    MOTOR motor2;
};
