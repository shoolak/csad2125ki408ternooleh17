#ifndef SERIALPORT_H
#define SERIALPORT_H
#include <fstream> 
#include <string>
#include <windows.h> // Для використання Windows API
/**
 * Global variable representing the communication port.
 */
extern std::string port;
/**
 * Global variable representing the baud rate for communication.
 */
extern int baudRate;

class SerialCommunication {
private:
    /**
    * Handle of a serial port
    */
    HANDLE hSerial = INVALID_HANDLE_VALUE;

public:
    
    /**
     * @brief Connects to the serial port with the specified port name and baud rate.
     * @param port The name of the port to connect to (e.g., "COM5").
     * @param baudRate The baud rate for the connection (e.g., 9600).
     * @return True if the connection was successful, otherwise false.
     */
    bool connect(const std::string& port, int baudRate);
    
    /**
     * @brief Sends a message over the serial connection.
     * @param message The message to send to the connected device.
     * @return The response from the device as a string.
     */
    std::string sendMessage(const std::string& message);
    
    /**
    * @brief Disconnects from the serial port, if connected.
    */
    void disconnect();
    
    /**
     * @brief Prints a graphical representation of the board based on the provided state.
     * @param boardState The string representing the current board state.
     */
    void drawBoard(const std::string& boardState);
};
/**
 * @brief Loads configuration values from an JSON file.
 * @param filename The path to the configuration file.
 */
void loadConfig(const std::string& filename);

#endif