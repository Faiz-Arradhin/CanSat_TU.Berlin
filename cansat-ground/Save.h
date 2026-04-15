#pragma once

#include "SMObjects.h"
#include "CansatModule.h"

ref class Save {
public:
	Save(SM_Sensors^ sm_sensors);
	error_state write();

private:
	String^ GetTimestamp();
	String^ GetDuration();

	SM_Sensors^ VSensors;
	StreamWriter^ writer;
	Stopwatch^ sw;
};