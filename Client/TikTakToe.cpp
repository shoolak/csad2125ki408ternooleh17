#include <iostream>
#include "SerialPort.h"

int main() {
   
    HANDLE hSerial = openSerialPort(L"COM5", 9600);
    if (hSerial == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to open serial port." << std::endl;
        return 1;
    }

    // Відправляємо повідомлення на Arduino
    std::string messageToSend;
    std::cout << "Input msg:" << std::endl;
    std::cin >> messageToSend;
    //std::string messageToSend = "Hello, Arduino!";
    sendMessage(hSerial, messageToSend);

    // Виводимо повідомлення на консоль
    std::cout << "Message sent: " << messageToSend << std::endl;

    // Читаємо відповідь від Arduino
    std::string response = readMessage(hSerial);

    // Виводимо відповідь на консоль
    std::cout << "Message received: " << response << std::endl;

    // Закриваємо серійний порт
    CloseHandle(hSerial);
    return 0;
}
