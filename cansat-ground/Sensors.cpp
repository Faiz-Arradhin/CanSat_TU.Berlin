#include "Sensors.h"

Sensors::Sensors(SM_ThreadManagement^ SM_TM, SM_Sensors^ sm_sensors, SM_Port^ sm_port) {
	SM_TM_ = SM_TM;
	VSensors = sm_sensors;
    VPort = sm_port;
	VStopwatch = gcnew Stopwatch;
    buf = gcnew array<Byte>(20);

    rxState = RxState::WAIT_HEADER_0;
    payloadIndex = 0;

    speedwatch = gcnew Stopwatch;
}

void Sensors::threadFunction() {
	while (connect("", 0) != SUCCESS) {};
	SM_TM_->ThreadBarrier->SignalAndWait();

	VStopwatch->Start();
    speedwatch->Start();

	int i = 0;
	while (!getShutdownFlag()) {
		Console::WriteLine("Sensors Thread is running {0}     ", i++);

		//if (communicate() != SUCCESS) continue;
		//if (processSharedMemory() != SUCCESS) continue;
        communicate();
		processHeartbeats();

		Thread::Sleep(5);
	}

	disconnect();
	Console::WriteLine("Sensors Thread is terminated");
};

error_state Sensors::connect(String^ hostName, int portNumber) {
	try {
        if (!VPort->port->IsOpen && !VPort->connect()) {
            return ERR_CONNECTION;
        }

        return SUCCESS;
	} catch (Exception^ e) {
		Console::WriteLine("Sensors Connection Failed: {0}", e->Message);
		return ERR_CONNECTION;
	}
}

void Sensors::disconnect() {
    VPort->disconnect();
}

error_state Sensors::communicate() {
    try {
        while (VPort->port->BytesToRead > 0) {
            int b = VPort->s->ReadByte();
            if (b < 0) return SUCCESS;

            switch (rxState) {

            case RxState::WAIT_HEADER_0:
                if (b == 0x00)
                    rxState = RxState::WAIT_HEADER_1;
                break;

            case RxState::WAIT_HEADER_1:
                if (b == 0x01) {
                    payloadIndex = 0;
                    rxState = RxState::READ_PAYLOAD;
                } else {
                    rxState = RxState::WAIT_HEADER_0;
                }
                break;

            case RxState::READ_PAYLOAD:
                buf[payloadIndex++] = (Byte)b;

                if (payloadIndex == 20) {
                    processSharedMemory();
                    rxState = RxState::WAIT_HEADER_0;
                }
                break;
            }
        }

        return SUCCESS;
    } catch (Exception^) {
        return ERR_CONNECTION;
    }
}

uint8_t Sensors::computeChecksum(int len) {
    uint8_t sum = 0;
    for (int i = 0; i < len; i++)
        sum += buf[i];
    return sum;
}

error_state Sensors::processSharedMemory() {
    uint8_t crc = buf[19];
    uint8_t calc = computeChecksum(19);

    if (crc != calc) {
        Console::WriteLine("Checksum fail");
        return ERR_INVALID_DATA;
    }

    Console::WriteLine("CHECKSUM GOOD");

    int16_t accX_raw = BitConverter::ToInt16(buf, 0);
    int16_t accY_raw = BitConverter::ToInt16(buf, 2);
    int16_t accZ_raw = BitConverter::ToInt16(buf, 4);

    int16_t angleX_raw = BitConverter::ToInt16(buf, 6);
    int16_t angleY_raw = BitConverter::ToInt16(buf, 8);
    int16_t angleZ_raw = BitConverter::ToInt16(buf, 10);

    int16_t tempIn = BitConverter::ToInt16(buf, 12);
    int16_t tempOut = BitConverter::ToInt16(buf, 14);
    int16_t altitude = BitConverter::ToInt16(buf, 16);

    Byte counter = buf[18];

    double accX = accX_raw / 100.0;
    double accY = accY_raw / 100.0;
    double accZ = accZ_raw / 100.0;
    double angleX = angleX_raw / 100.0;
    double angleY = angleY_raw / 100.0;
    double angleZ = angleZ_raw / 100.0;

    double roll = angleX * pi / 180.0;
    double pitch = angleY * pi / 180.0;
    double yaw = angleZ * pi / 180.0;
    double ax_b = accX * g;
    double ay_b = accY * g;
    double az_b = accZ * g;

    double cphi = cos(-roll);
    double sphi = sin(-roll);
    double ctheta = cos(-pitch);
    double stheta = sin(-pitch);
    double cpsi = cos(-yaw);
    double spsi = sin(-yaw);
    double R11 = ctheta * cpsi;
    double R12 = ctheta * spsi;
    double R13 = -stheta;
    double R21 = sphi * stheta * cpsi - cphi * spsi;
    double R22 = sphi * stheta * spsi + cphi * cpsi;
    double R23 = sphi * ctheta;
    double R31 = cphi * stheta * cpsi + sphi * spsi;
    double R32 = cphi * stheta * spsi - sphi * cpsi;
    double R33 = cphi * ctheta;
    
    double gx_b = R13 * g;
    double gy_b = R23 * g;
    double gz_b = R33 * g;

    double ax_lin_b = ax_b - gx_b;
    double ay_lin_b = ay_b - gy_b;
    double az_lin_b = az_b - gz_b;

    double ax_lin = R11 * ax_lin_b + R12 * ay_lin_b + R13 * az_lin_b;
    double ay_lin = R21 * ax_lin_b + R22 * ay_lin_b + R23 * az_lin_b;
    double az_lin = R31 * ax_lin_b + R32 * ay_lin_b + R33 * az_lin_b;

    double ax_prop = R11 * ax_b + R12 * ay_b + R13 * az_b;
    double ay_prop = R21 * ax_b + R22 * ay_b + R23 * az_b;
    double az_prop = R31 * ax_b + R32 * ay_b + R33 * az_b;

    double g_load = Math::Sqrt(
        ax_prop * ax_prop +
        ay_prop * ay_prop +
        az_prop * az_prop
    ) / g;

    double t_now = speedwatch->Elapsed.TotalSeconds;
    double dt = (t_now - t_prev); // seconds

    if (!bias_ready) {
        if (ax_lin != bias_ax_prev || ay_lin != bias_ay_prev || az_lin != bias_az_prev) {
            bias_sum_ax += ax_lin - bias_ax_prev;
            bias_sum_ay += ay_lin - bias_ay_prev;
            bias_sum_az += az_lin - bias_az_prev;
            bias_ax_prev = ax_lin;
            bias_ay_prev = ay_lin;
            bias_az_prev = az_lin;
            bias_count++;
        }

        if (bias_count > 200)   // ~2 seconds at 100 Hz
        {
            bias_ax = bias_sum_ax / bias_count;
            bias_ay = bias_sum_ay / bias_count;
            bias_az = bias_sum_az / bias_count;
            vx = 0;
            vy = 0;
            vz = 0;
            firstSample = true;
            bias_ready = true;
        }
    } else {
        ax_lin -= bias_ax;
        ay_lin -= bias_ay;
        az_lin -= bias_az;
    }


    if (!firstSample) {
        // trapezoidal integration
        vx += ax_lin * dt;
        vy += ay_lin * dt;
        vz += az_lin * dt;
    } else {
        firstSample = false;
    }

    // update previous values
    ax_prev = ax_lin;
    ay_prev = ay_lin;
    az_prev = az_lin;
    t_prev = t_now;

    double v_total = Math::Sqrt(pow(vx, 2) + pow(vy, 2) + pow(vz, 2));

    try {
        Monitor::Enter(VSensors->lockObject);
        VSensors->oriAccX = accX;
        VSensors->oriAccY = accY;
        VSensors->oriAccZ = accZ;
        VSensors->accX = ax_lin;
        VSensors->accY = ay_lin;
        VSensors->accZ = az_lin;

        VSensors->g_load = g_load;
        VSensors->v_total = v_total;

        VSensors->angleX = angleX;
        VSensors->angleY = angleY;
        VSensors->angleZ = angleZ;

        VSensors->tempIn = tempIn / 100.0;
        VSensors->tempOut = tempOut / 100.0;
        VSensors->alt = altitude / 100.0;

    } catch (Exception^ e) {
        return ERR_MEMORY_ACCESS;
    } finally {
        Monitor::Exit(VSensors->lockObject);
    }

    return SUCCESS;
};

bool Sensors::getShutdownFlag() {
    return SM_TM_->shutdown & bit_SENSORS;
};

error_state Sensors::processHeartbeats() {
    //check if the LiDAR bit in the hearbeat byte is low
    if ((SM_TM_->heartbeat & bit_SENSORS) == 0) {
        //put the LiDAR bit high
        SM_TM_->heartbeat |= bit_SENSORS;
        //reset the stopwatch
        VStopwatch->Restart();
    } else {
        //check if the time elasped exceed the crash time limit
        if (VStopwatch->ElapsedMilliseconds > CRASH_LIMIT) {
            //shut down all threads
            shutdownModules();
            return ERR_TMT_FAILURE;
        }
    }

    return SUCCESS;
}

void Sensors::shutdownModules() {
    SM_TM_->shutdown = 0xFF;
}