#pragma once

class BURNER {
public:
  BURNER() = delete;
  BURNER(int burn_Pin): burnPin(burn_Pin) {}

  void setup() {
    pinMode(burnPin, OUTPUT);
  }

  void on() {
    digitalWrite(burnPin, HIGH);
  }

  void off() {
    digitalWrite(burnPin, LOW);
  }

private:
  int burnPin;
};
