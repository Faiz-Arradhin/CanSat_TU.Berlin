#pragma once

#include <TaskScheduler.h>

#include "buzzer.hpp"

class TMT_Buzzer {
public:
  TMT_Buzzer() = delete;
  TMT_Buzzer(BUZZ& buzzer): buzz(buzzer), tSOS(100, TASK_FOREVER, &TMT_Buzzer::runSOSStatic) {
    instance = this;
  }

  void setup() {
    runner.addTask(tSOS);
    tSOS.enable();
  }

  void run() {
    runner.execute();
  }
  
private:
  static void runSOSStatic() {
    if (instance) instance->runSOS();
  }
  void runSOS() {
    if (buzz.onOff()) {
      tSOS.setInterval(buzz.nextInterval());
    }
    buzz.runSOS();
  }
  BUZZ& buzz;
  Scheduler runner;
  Task tSOS;

  static TMT_Buzzer* instance;
};

TMT_Buzzer* TMT_Buzzer::instance = nullptr;
