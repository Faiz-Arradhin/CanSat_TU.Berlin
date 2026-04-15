#include "Save.h"

Save::Save(SM_Sensors^ sm_sensors) : 
	VSensors(sm_sensors), sw(Stopwatch::StartNew()) 
{
    DateTime now = DateTime::Now;
    String^ timestamp = now.ToString("yyyy-MM-dd HH-mm");

    writer = gcnew StreamWriter("telemetry_AstroNeo_" + timestamp + ".csv", true);
    writer->WriteLine("Timestamps,Elapsed Seconds,Acc_x,Acc_y,Acc_z,g-Load,Speed,Angle_x,Angle_y,Angle_z,Temp_in,Temp_out,Altitude");
};

String^ Save::GetTimestamp() {
    DateTime now = DateTime::Now;
    return now.ToString("HH:mm:ss.fff");
}

String^ Save::GetDuration() {
    double elapsed = sw->Elapsed.TotalSeconds;
    return elapsed.ToString("F5");
}

error_state Save::write() {
	try {
		Monitor::Enter(VSensors->lockObject);
		Console::WriteLine("Acceleration X: {0}", VSensors->accX);
		Console::WriteLine("Acceleration Y: {0}", VSensors->accY);
		Console::WriteLine("Acceleration Z: {0}", VSensors->accZ);
		Console::WriteLine("Angle X: {0}", VSensors->angleX);
		Console::WriteLine("Angle Y: {0}", VSensors->angleY);
		Console::WriteLine("Angle Z: {0}", VSensors->angleZ);
		Console::WriteLine("Temperature Inside: {0}", VSensors->tempIn);
		Console::WriteLine("Temperature Outside: {0}", VSensors->tempOut);
		Console::WriteLine("Altitude: {0}", VSensors->alt);

        writer->WriteLine("{0},{1},{2},{3},{4},{5},{6},{7},{8},{9},{10},{11},{12}",
            GetTimestamp(),
            GetDuration(),
            VSensors->accX,
            VSensors->accY,
            VSensors->accZ,
            VSensors->g_load,
            VSensors->v_total,
            VSensors->angleX,
            VSensors->angleY,
            VSensors->angleZ,
            VSensors->tempIn,
            VSensors->tempOut,
            VSensors->alt
        );
    } catch (Exception^ e) {
        return ERR_MEMORY_ACCESS;
    } finally {
        Monitor::Exit(VSensors->lockObject);
    }

    return SUCCESS;
}

