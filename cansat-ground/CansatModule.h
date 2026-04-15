#pragma once

#include "SMObjects.h"

#define CRASH_LIMIT 15000

// ERROR HANDLING. Use this as the return value in your functions
enum error_state {
	SUCCESS,
	ERR_STARTUP,
	ERR_NO_DATA,
	ERR_INVALID_DATA,
	ERR_SM,
	ERR_CONNECTION,
	// Define your own additional error types as needed
	ERR_MEMORY_ACCESS,
	ERR_CRITICAL,
	ERR_TMT_FAILURE,
	ERR
};

enum ControllerSource {
	KEYBOARD
};

ref struct Address {
	static String^ Local = "127.0.0.1";
};

ref struct Port {
	static int Display = 28000;
};

ref class CansatModule abstract {
public:
	/**
	 * Send/Recieve data from shared memory structures
	 *
	 * @returns the success of this function at completion
	*/
	virtual error_state processSharedMemory() = 0;

	/**
	 * Get Shutdown signal for the current, from Process Management SM
	 *
	 * @returns the current state of the shutdown flag
	*/
	virtual bool getShutdownFlag() = 0;

	/**
	 * Main runner for the thread to perform all required actions
	*/
	virtual void threadFunction() = 0;

	/**
	 * A helper function for printing a helpful error message based on the error type
	 * returned from a function
	 *
	 * @param error the error that should be printed out
	*/
	static void printError(error_state error) {
		switch (error) {
		case SUCCESS:
			std::cout << "Success." << std::endl;
			break;
		case ERR_NO_DATA:
			std::cout << "ERROR: No Data Available." << std::endl;
			break;
		case ERR_INVALID_DATA:
			std::cout << "ERROR: Invalid Data Received." << std::endl;
			break;
			// ADD PRINTOUTS FOR OTHER ERROR TYPES
		}
	}

protected:
	SM_ThreadManagement^ SM_TM_;
	SM_Sensors^ VSensors;
	SM_VehicleControl^ VCon;
	SM_Port^ VPort;

	Stopwatch^ VStopwatch;
};
