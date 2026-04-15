#pragma once

#include "Controller.h"
#include "CansatModule.h"
#include "Save.h"
#include "Sensors.h"
#include "Display.h"
#include "VC.h"

#define CONTROL ControllerSource::KEYBOARD

ref struct ThreadProperties {
    ThreadStart^ ThreadStart_;
    Action^ ThreadEnd_;
    bool Critical;
    String^ ThreadName;
    uint8_t BitID;

    ThreadProperties(ThreadStart^ start, Action^ end, bool critical, uint8_t bit_id, String^ threadName);
};

ref class ThreadManagement : public CansatModule {
public:
    // Create shared memory objects
    error_state setupSharedMemory();

    // Send/Recieve data from shared memory structures
    error_state processSharedMemory() override;

    // Shutdown all modules in the software
    void shutdownModules();

    // Get Shutdown signal for module, from Thread Management SM
    bool getShutdownFlag() override;

    // Thread function for TMM
    void threadFunction() override;

private:
    // Add any additional data members or helper functions here
    Sensors^ Sensors_;
    Controller^ Controller_;
    VC^ VC_;
    Display^ Display_;
    Save^ Save_;

    array<Thread^>^ ThreadList;
    array<ThreadProperties^>^ ThreadPropertiesList;

    error_state processHeartbeats();
};
