#pragma once
#pragma comment(lib, "User32.lib")

#using <System.dll>

#include <windows.h>
#include <iostream>
#include <cstdint>
#include <cmath>

using namespace System;
using namespace System::Threading;
using namespace System::IO;
using namespace System::IO::Ports;
using namespace System::Net;
using namespace System::Net::Sockets;
using namespace System::Text;
using namespace System::Diagnostics;
using namespace System::Collections::Generic;

constexpr double pi = System::Math::PI;
constexpr double g = 9.80665;

// Defines which processes are critical and not critical
// Ordering of processes in the mask should be based on the order in the UnitFlags type below.
// You should change this based on what processes are and are not critical
constexpr uint8_t NONCRITICALMASK = 0xff;
constexpr uint8_t CRITICALMASK = 0x00;

constexpr uint8_t bit_ALL = 0b11111;
constexpr uint8_t bit_TM = 0b00001;
constexpr uint8_t bit_SENSORS = 0b00010;
constexpr uint8_t bit_CONTROLLER = 0b00100;
constexpr uint8_t bit_DISPLAY = 0b01000;
constexpr uint8_t bit_VC = 0b10000;

ref class SM_ThreadManagement {
public:
    Object^ lockObject;
    uint8_t shutdown = 0;
    Barrier^ ThreadBarrier;
    array<Stopwatch^>^ WatchList;
    uint8_t heartbeat = 0;

    SM_ThreadManagement() {
        lockObject = gcnew Object();
    }
};

ref class SM_Sensors {
public:
    Object^ lockObject;
    double oriAccX;
    double oriAccY;
    double oriAccZ;
    double accX;
    double accY;
    double accZ;
    double g_load;
    double v_total;
    double angleX;
    double angleY;
    double angleZ;
    double tempIn;
    double tempOut;
    double alt;

    SM_Sensors() {
        lockObject = gcnew Object();
    }
};

public enum class SatAction : Byte {
    Standby = 0x00,
    Burn = 0x01,
    BeepForward = 0x02,
    BeepForwardRight = 0x03,
    BeepForwardLeft = 0x04,
    BeepReverse = 0x05,
    BeepReverseRight = 0x06,
    BeepReverseLeft = 0x07,
    Beep = 0x08,
    Forward = 0x09,
    ForwardRight = 0x0A,
    ForwardLeft = 0x0B,
    Reverse = 0x0C,
    ReverseRight = 0x0D,
    ReverseLeft = 0x0E,
    NoChange = 0x0F
};

ref class SM_VehicleControl {
public:
    Object^ lockObject;
    SatAction action;
    bool update = false;

    SM_VehicleControl() {
        lockObject = gcnew Object();
    }
};

ref class SM_Port {
public:
    SerialPort^ port;
    Stream^ s;

    SM_Port(String^ portName, int baudRate) {
        port = gcnew SerialPort(portName, baudRate);
    }

    bool connect() {
        try {
            if (port->IsOpen)
                port->Close();

            port->Parity = Parity::None;
            port->DataBits = 8;
            port->StopBits = StopBits::One;
            port->Handshake = Handshake::None;

            port->DtrEnable = true;
            port->RtsEnable = true;

            port->ReadTimeout = 20;
            port->WriteTimeout = 20;

            port->Encoding = System::Text::Encoding::ASCII;

            port->Open();
            if (!port->IsOpen)
                return false;

            // MUST be after Open()
            s = port->BaseStream;
            s->ReadTimeout = 20;

            return true;
        } catch (Exception^ e) {
            Console::WriteLine("Port connection failed: {0}", e->Message);
            return false;
        }
    }

    void disconnect() {
        try {
            if (port != nullptr && port->IsOpen)
                port->Close();
        } catch (Exception^) {}
    }
};