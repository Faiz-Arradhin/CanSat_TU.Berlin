#pragma once

#include <TaskScheduler.h>

#include "Sensors.hpp"
#include "MotorTwo.hpp"
#include "servo.hpp"
#include "burner.hpp"

struct Packet {
  uint8_t start1;   // 0x00
  uint8_t start2;   // 0x01

  int16_t accX;
  int16_t accY;
  int16_t accZ;

  int16_t angleX;
  int16_t angleY;
  int16_t angleZ;

  int16_t tempIn;
  int16_t tempOut;
  int16_t altitude;

  uint8_t counter;
  uint8_t checksum;
};


class TMT_Comm {
public:
  TMT_Comm() = delete;
  TMT_Comm(Sensors& sensors_, BUZZ& buzz_, MotorTwo& motors_, SERVO& servo_, BURNER& burner_): 
    sensors(sensors_),
    buzz(buzz_),
    motors(motors_),
    servo(servo_),
    burner(burner_),
    tSend(41, TASK_FOREVER, &TMT_Comm::runSendStatic),
    tRead(2, TASK_FOREVER, &TMT_Comm::runReadStatic)
  {
    instance = this;
  }

  void setup() {
    pkt.start1 = 0x00;
    pkt.start2 = 0x01;
    runner.addTask(tSend);
    runner.addTask(tRead);
    tSend.enable();
    tRead.enable();
  }

  void run() {
    runner.execute();
  }
  
private:
  static void runSendStatic() {
    if (instance) instance->runSend();
  }

  static void runReadStatic() {
    if (instance) instance->runRead();
  }

  void runSend() {
    pkt.accX = (int16_t)(sensors.getAcc().x * 100.0f);
    pkt.accY = (int16_t)(sensors.getAcc().y * 100.0f);
    pkt.accZ = (int16_t)(sensors.getAcc().z * 100.0f);
  
    pkt.angleX = (int16_t)(sensors.getAngle().x * 100.0f);
    pkt.angleY = (int16_t)(sensors.getAngle().y * 100.0f);
    pkt.angleZ = (int16_t)(sensors.getAngle().z * 100.0f);
  
    pkt.tempIn = (int16_t)(sensors.getTempIn() * 100.0f);
    pkt.tempOut = (int16_t)(sensors.getTempOut() * 100.0f);
    pkt.altitude = (int16_t)(sensors.getAltitude() * 100.0f);
  
    pkt.counter++;
  
    uint8_t* raw = (uint8_t*)&pkt;
    pkt.checksum = computeChecksum(raw + 2, 19); // all bytes except checksum
  
    Serial.write(raw, sizeof(Packet));
  

  }

  void runRead() {
    if (Serial.available() > 0) {
        rx = Serial.read();
        currentStateAction = rx & 0x0F;   // extract lower 4 bits
    }

      switch (currentStateAction){
        case 0x00:
          // Standby
          buzz.off();
          burner.off();
          servo.write(0);
          motors.stop();
          break;
          
        case 0x01: 
          // Burn
          buzz.off();
          burner.on();
          motors.stop();
          servo.write(90);
          break;
          
        case 0x02: 
          // Beep + forward
          buzz.on();          
          burner.off();
          servo.write(30);
          motors.forward();
          break;
          
        case 0x03: 
          // Beep + forward right
          buzz.on();
          burner.off();
          servo.write(45);
          motors.FRight();
          break;
          
        case 0x04: 
          // Beep + forward left
          buzz.on();
          burner.off();
          servo.write(60);
          motors.FLeft();
          break;
          
        case 0x05: 
          // Beep + reverse
          buzz.on();
          burner.off();
          servo.write(90); 
          motors.reverse();
          break;
          
        case 0x06: 
          // Beep + reverse right
          buzz.on();
          burner.off();
          motors.RRight();
          break;
          
        case 0x07: 
          // Beep + reverse left
          buzz.on();
          burner.off();
          motors.RLeft();
          break;
          
        case 0x08: 
          // Beep 
          buzz.on();
          burner.off();
          motors.stop();
          break;
        case 0x09: 
          // forward
          buzz.off();
          burner.off();
          motors.forward();
          break;
        case 0x0A: 
          // forward right
          buzz.off();
          burner.off();
          motors.FRight();
          break;
        case 0x0B: 
          // forward left
          buzz.off();
          burner.off();
          motors.FLeft();
          break;
        case 0x0C: 
          // reverse
          buzz.off();
          burner.off();
          motors.reverse();
          break;
        case 0x0D: 
          // reverse right
          buzz.off();
          burner.off();
          motors.RRight();
          break;
        case 0x0E: 
          // reverse left
          buzz.off();
          burner.off();
          motors.RLeft();
          break;
        case 0x0F: 
//            Serial.println("Unused state E"); 
          break;
        default:  
//            Serial.println("Invalid state"); 
          break;
        }

  }

  MotorTwo& motors;
  BUZZ& buzz;
  Sensors& sensors;
  SERVO& servo;
  BURNER& burner;
  
  Scheduler runner;
  Task tSend;
  Task tRead;
  Packet pkt;

  uint8_t rx;
  uint8_t currentStateAction = 0;

  static TMT_Comm* instance;

  uint8_t computeChecksum(uint8_t* data, int len) {
    uint8_t sum = 0;
    for (int i = 0; i < len; i++) sum += data[i];
    return sum;
  }


  void writeInt16(uint8_t* buf, int index, int16_t value) {
      buf[index]     = (value >> 8) & 0xFF;   // High byte
      buf[index + 1] = value & 0xFF;          // Low byte
  }
  
  void sendTelemetryPacket(
      int16_t accX, int16_t accY, int16_t accZ,
      int16_t angleX, int16_t angleY, int16_t angleZ,
      int16_t tempIn, int16_t tempOut,
      int16_t altitude)
  {
      const uint8_t START_BYTE  = 0xAA;
      const uint8_t PACKET_TYPE = 0x01;
      const uint8_t PAYLOAD_LEN = 18;   // 9 values × 2 bytes
  
      uint8_t packet[3 + PAYLOAD_LEN + 1];  // header + payload + checksum
  
      packet[0] = START_BYTE;
      packet[1] = PACKET_TYPE;
      packet[2] = PAYLOAD_LEN;
  
      // Payload
      writeInt16(packet, 3,  accX);
      writeInt16(packet, 5,  accY);
      writeInt16(packet, 7,  accZ);
  
      writeInt16(packet, 9,  angleX);
      writeInt16(packet, 11, angleY);
      writeInt16(packet, 13, angleZ);
  
      writeInt16(packet, 15, tempIn);
      writeInt16(packet, 17, tempOut);
  
      writeInt16(packet, 19, altitude);
  
      // Checksum (XOR of all previous bytes)
      uint8_t checksum = 0;
      for (int i = 0; i < 3 + PAYLOAD_LEN; i++) {
          checksum ^= packet[i];
      }
      packet[21] = checksum;
  
      // Send the packet
      Serial.write(packet, sizeof(packet));
  }
};

TMT_Comm* TMT_Comm::instance = nullptr;
