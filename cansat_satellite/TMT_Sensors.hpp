#pragma once

#include <TaskScheduler.h>

#include "Sensors.hpp"

class TMT_Sensors {
public:
  TMT_Sensors() = delete;
  TMT_Sensors(Sensors& sensors_): 
    sensors(sensors_),
    tMPU(7, TASK_FOREVER, &TMT_Sensors::runMPUStatic),
    tBaro(61, TASK_FOREVER, &TMT_Sensors::runBaroStatic),
    tTemp(503, TASK_FOREVER, &TMT_Sensors::runTempStatic)
  {
    instance = this;
  }

  void setup() {
    runner.addTask(tMPU);
    runner.addTask(tBaro);
    runner.addTask(tTemp);
    tMPU.enable();
    tBaro.enable();
    tTemp.enable();
  }

  void run() {
    sensors.updateFast();
    runner.execute();
  }
  
private:
  
  static void runMPUStatic() {
    if (instance) instance->runMPU();
  }

  static void runBaroStatic() {
    if (instance) instance->runBaro();
  }

  static void runTempStatic() {
    if (instance) instance->runTemp();
  }
  
  void runMPU() {
    sensors.updateMPU();
  }

  void runBaro() {
    sensors.updateBaro();
  }

  void runTemp() {
    sensors.updateTemp();
  }
  
  Sensors& sensors;
  Scheduler runner;
  Task tMPU;
  Task tBaro;
  Task tTemp;

  static TMT_Sensors* instance;
};

TMT_Sensors* TMT_Sensors::instance = nullptr;
