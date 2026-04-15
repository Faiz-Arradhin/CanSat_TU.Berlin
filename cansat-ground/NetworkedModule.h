#pragma once

#include "CansatModule.h"

//#define WEEDER_ADDRESS "192.168.1.200"
#define DISPLAY_ADDRESS "127.0.0.1" // Display is always running locally (run the MATLAB code separately)

ref class NetworkedModule abstract : public CansatModule {
public:
	virtual error_state connect(String^ hostName, int portNumber) = 0;	// Establish TCP connection
	virtual error_state communicate() = 0;								// Communicate over TCP connection (includes any error checking required on data)

protected:
	array<unsigned char>^ ReadData;		// Array to store sensor Data (only used for sensor modules)
	array<unsigned char>^ SendData;
};
