#pragma once

const int DOT = 60;
const int DASH = 250;
const int GAP = 60;      // gap between elements
const int LETTER_GAP = 60;
const int WORD_GAP = 1500;

class BUZZ {
public:
  BUZZ() = delete;
  BUZZ(int buzz_Pin): buzzPin(buzz_Pin) {}

  void setup() {
    pinMode(buzzPin, OUTPUT);
  }

  void runSOS() {
    if (buzzOn) {
      // Toggle buzzer state
      digitalWrite(buzzPin, buzzState ? HIGH : LOW);
    
      // Move to next element
      indexPos++;
      buzzState = !buzzState;
    
      // Loop pattern
      if (indexPos >= patternLength) {
        indexPos = 0;
        buzzState = true;
      }
    } else {
      digitalWrite(buzzPin, LOW);
    }
  }

  int nextInterval() const {
    return pattern[indexPos];
  }

  bool onOff() const {
    return buzzOn;
  }

  void on() {
    buzzOn = true;
  }

  void off() {
    buzzOn = false;
  }

private:
  bool buzzOn = false;
  int buzzPin;
  bool buzzState = true;
  int pattern[18] = {
    DOT, GAP, DOT, GAP, DOT, LETTER_GAP,     // S: ···
    DASH, GAP, DASH, GAP, DASH, LETTER_GAP,  // O: –––
    DOT, GAP, DOT, GAP, DOT, WORD_GAP        // S: ···
  };
  int patternLength = sizeof(pattern) / sizeof(pattern[0]);
  int indexPos = 0;
};
