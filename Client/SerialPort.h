#ifndef SERIALPORT_H
#define SERIALPORT_H
#include <fstream> 
#include <string>
#include <windows.h> // Для використання Windows API

extern std::string port;
extern int baudRate;

class SerialCommunication {
private:
    HANDLE hSerial = INVALID_HANDLE_VALUE;

public:
    bool connect(const std::string& port, int baudRate);
    std::string sendMessage(const std::string& message);
    void disconnect();
    void drawBoard(const std::string& boardState);
};

void loadConfig(const std::string& filename);

#endif // COMMUNICATION_H