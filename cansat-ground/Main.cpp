#include "TMM.h"

int main(void) {
    ThreadManagement^ myTMT = gcnew ThreadManagement();

    myTMT->setupSharedMemory();

    myTMT->threadFunction();

    Console::ReadKey();
    Console::ReadKey();
    return 0;
}
/*
LPCWSTR portName = L"\\\\.\\COM8";

HANDLE hSerial = CreateFileW(
    portName,
    GENERIC_READ | GENERIC_WRITE,
    0,
    nullptr,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    nullptr
);

if (hSerial == INVALID_HANDLE_VALUE) {
    std::cerr << "Error opening COM port\n";
    Console::ReadKey();
    return 1;
}

// Configure serial port
DCB dcbSerialParams = {0};
dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

if (!GetCommState(hSerial, &dcbSerialParams)) {
    std::cerr << "Failed to get COM state\n";
    return 1;
}

dcbSerialParams.BaudRate = CBR_9600;   // Match your XBee baud rate
dcbSerialParams.ByteSize = 8;
dcbSerialParams.StopBits = ONESTOPBIT;
dcbSerialParams.Parity = NOPARITY;

if (!SetCommState(hSerial, &dcbSerialParams)) {
    std::cerr << "Failed to set COM state\n";
    return 1;
}

// Set timeouts
COMMTIMEOUTS timeouts = {0};
timeouts.ReadIntervalTimeout = 30;
timeouts.ReadTotalTimeoutConstant = 30;
timeouts.ReadTotalTimeoutMultiplier = 5;

SetCommTimeouts(hSerial, &timeouts);

std::cout << "Listening for XBee data...\n";

char buffer[256];
DWORD bytesRead;

while (true) {
    if (ReadFile(hSerial, buffer, sizeof(buffer) - 1, &bytesRead, nullptr)) {
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            for (DWORD i = 0; i < bytesRead; i++) {
                printf("%02X ", (unsigned char)buffer[i]);
            }
            printf("\n");
        }
    }
}

CloseHandle(hSerial);
*/