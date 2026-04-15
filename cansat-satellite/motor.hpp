#pragma once

class MOTOR {
public:
    MOTOR(int in1, int in2) : pin1(in1), pin2(in2) {}

    void setup() {
        pinMode(pin1, OUTPUT);
        pinMode(pin2, OUTPUT);
        stop();
    }

    // speed: 0–255
    void forward(int speed = 255) {
        analogWrite(pin1, speed);
        digitalWrite(pin2, LOW);
    }

    void reverse(int speed = 255) {
        digitalWrite(pin1, LOW);
        analogWrite(pin2, speed);
    }

    void stop() {  // coast
        digitalWrite(pin1, LOW);
        digitalWrite(pin2, LOW);
    }

    void brake() { // active brake
        digitalWrite(pin1, HIGH);
        digitalWrite(pin2, HIGH);
    }

private:
    int pin1, pin2;
};
