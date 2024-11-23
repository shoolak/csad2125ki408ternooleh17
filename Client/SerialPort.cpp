#include "SerialPort.h"

HANDLE openSerialPort(const std::wstring& portName, DWORD baudRate) {
    HANDLE hSerial = CreateFileW(portName.c_str(), 
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    if (hSerial == INVALID_HANDLE_VALUE) {
        std::cout << "Failed to open serial port." << std::endl;
        return INVALID_HANDLE_VALUE;
    }

    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams)) {
        std::cout << "Error getting serial port state." << std::endl;
        CloseHandle(hSerial);
        return INVALID_HANDLE_VALUE;
    }

    dcbSerialParams.BaudRate = baudRate;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        std::cout << "Error setting serial port state." << std::endl;
        CloseHandle(hSerial);
        return INVALID_HANDLE_VALUE;
    }

    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    SetCommTimeouts(hSerial, &timeouts);

    return hSerial;
}

void sendMessage(HANDLE hSerial, const std::string& message) {
    DWORD bytesWritten;
    WriteFile(hSerial, message.c_str(), message.size(), &bytesWritten, NULL);
}

std::string readMessage(HANDLE hSerial) {
    char buffer[256];
    DWORD bytesRead;
    std::string result;

    while (true) {
        if (ReadFile(hSerial, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
            buffer[bytesRead] = '\0';
            result += buffer;

            if (result.find('\n') != std::string::npos) {
                break;
            }
        }
    }
    return result;
}
