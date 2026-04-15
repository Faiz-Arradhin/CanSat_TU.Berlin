#include <avr/wdt.h>

#include "burner.hpp"
#include "motorTwo.hpp"
#include "servo.hpp"
#include "TMT_Buzzer.hpp"
#include "TMT_Comm.hpp"
#include "TMT_Sensors.hpp"

#define BUZZ_PIN 10
#define TEMP_PIN A0
#define SERVO_PIN 9
#define MOTOR_PIN_1a 2
#define MOTOR_PIN_1b 3
#define MOTOR_PIN_2a 5
#define MOTOR_PIN_2b 6 
#define RESET_PIN 4
#define BURNER_PIN 11

SERVO servo(SERVO_PIN);
BURNER burner(BURNER_PIN);

BUZZ buzzer(BUZZ_PIN);
TMT_Buzzer tmtBuzzer(buzzer);

Sensors sensors(TEMP_PIN);
TMT_Sensors tmtSensors(sensors);

MotorTwo motors(MOTOR_PIN_1a, MOTOR_PIN_1b, MOTOR_PIN_2a, MOTOR_PIN_2b);

TMT_Comm tmtComm (sensors, buzzer, motors, servo, burner);

int i = 0;
void setup() {
  Wire.begin();
  Serial.begin(57600);
  pinMode(RESET_PIN, INPUT_PULLUP);

  servo.setup();
  servo.write(0);

  burner.setup();
  burner.off();
  
  buzzer.setup();
  tmtBuzzer.setup();
  
  motors.setup();
  sensors.setup();
  tmtSensors.setup();  

  tmtComm.setup();
}

void loop() {
  if (digitalRead(RESET_PIN) == LOW) {

    delay(50);  // debounce

    // Enable watchdog with shortest timeout (15 ms)
    wdt_enable(WDTO_15MS);

    while (true) { }
  }
  
  tmtSensors.run();
//  Serial.print("Altitude: ");

//  Serial.println(sensors.getAltitude());
//  Serial.print("Acceleration X: ");
//  Serial.println(sensors.getAngle().x);
//  Serial.print("Acceleration Y: ");
//  Serial.println(sensors.getAngle().y);
//  Serial.print("Acceleration Z: ");
//  Serial.println(sensors.getAcc().z);
//  Serial.print("TempOut: ");
//  Serial.println(sensors.getTempOut());
//  Serial.println("");
  tmtBuzzer.run();
  tmtComm.run();
//  motors.forward();
}
