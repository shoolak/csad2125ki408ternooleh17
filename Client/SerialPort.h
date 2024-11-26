#pragma once
#include <windows.h>
#include <iostream>
#include <string>

HANDLE openSerialPort(const std::wstring& portName, DWORD baudRate);

void sendMessage(HANDLE hSerial, const std::string& message);

std::string readMessage(HANDLE hSerial);
