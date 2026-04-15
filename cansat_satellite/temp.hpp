#pragma once

class TEMP {
public:
  TEMP() = delete;
  TEMP(int temp_Pin): tempPin(temp_Pin) {};

  float getData() {
    return (analogRead(tempPin) * 5.0) / 1023.0 * 100.0;
  }
private:
  int tempPin;
};
