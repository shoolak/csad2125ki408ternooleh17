#include "SerialPort.h"
#include <iostream>
#include "D:\casd\csad2125ki408ternooleh17\3party\nlohmann\json.hpp"


HANDLE hConsole;
std::string port;
int baudRate;
void setColor(int textColor) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, textColor);
}

bool SerialCommunication::connect(const std::string& portName, int baudRate) {
    hSerial = CreateFile(portName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
    if (hSerial == INVALID_HANDLE_VALUE) {
        std::cerr << "Не вдалося відкрити порт: " << portName << std::endl;
        return false;
    }

    DCB dcb = { 0 };
    dcb.DCBlength = sizeof(dcb);
    dcb.BaudRate = baudRate;
    dcb.ByteSize = 8;
    dcb.StopBits = ONESTOPBIT;
    dcb.Parity = NOPARITY;

    if (!SetCommState(hSerial, &dcb)) {
        std::cerr << "Не вдалося налаштувати параметри порту." << std::endl;
        CloseHandle(hSerial);
        return false;
    }


    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(hSerial, &timeouts)) {
        std::cerr << "Не вдалося встановити таймаути комунікації." << std::endl;
        CloseHandle(hSerial);
        return false;
    }

    return true;
}

void SerialCommunication::disconnect() {
    if (hSerial != INVALID_HANDLE_VALUE) {
        CloseHandle(hSerial);
        hSerial = INVALID_HANDLE_VALUE;
    }
}

std::string SerialCommunication::sendMessage(const std::string& message) {
    if (hSerial == INVALID_HANDLE_VALUE) {
        std::cerr << "Серійний порт не відкритий." << std::endl;
        return "";
    }

    DWORD bytesWritten;
    if (!WriteFile(hSerial, message.c_str(), message.size(), &bytesWritten, nullptr)) {
        std::cerr << "Не вдалося записати в серійний порт." << std::endl;
        return "";
    }

    char buffer[256];
    DWORD bytesRead;
    if (!ReadFile(hSerial, buffer, sizeof(buffer) - 1, &bytesRead, nullptr)) {
        std::cerr << "Не вдалося прочитати з серійного порту." << std::endl;
        return "";
    }
    buffer[bytesRead] = '\0';

    return std::string(buffer);
}

void SerialCommunication::drawBoard(const std::string& boardState) {
    setColor(FOREGROUND_RED);
    std::cout << "-------------\n";
    for (int i = 0; i < 3; i++) {
        std::cout << "| ";
        for (int j = 0; j < 3; j++) {
            char cell = boardState[i * 3 + j];
            if (cell == 'X' || cell == 'O') {
                std::cout << cell << " | ";
            }
            else {
                std::cout << " " << " | ";
            }
        }
        std::cout << "\n-------------\n";
    }
}

using json = nlohmann::json;
void loadConfig(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open configuration file: " << filename << std::endl;
        return;
    }
    try {
        json j;
        file >> j;  
        port = j["Connection"]["port"].get<std::string>();
        baudRate = j["Connection"]["baudRate"].get<int>();

        if (port.empty() || baudRate == 0) {
            std::cerr << "Problem reading settings. Verify that the file has the correct format and value." << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error parsing JSON file: " << e.what() << std::endl;
    }
}