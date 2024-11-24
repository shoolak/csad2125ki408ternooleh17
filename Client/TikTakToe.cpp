#include <iostream>
#include "SerialPort.h"

int main()
{
    try
    {
        // Завантажуємо конфігурацію з файлу
        loadConfig("../Config/config.json");
        std::cout << "Configuration loaded: Port = " << port << ", BaudRate = " << baudRate << std::endl;

        SerialCommunication serial;

        if (!serial.connect(port, baudRate))
        {
            std::cerr << "Unable to connect to Arduino!" << std::endl;
            return 1;
        }


        std::cout << "Welcome to the game of Tic-Tac-Toe!" << std::endl;
        std::string response = serial.sendMessage("StartGame\n");

        if (response.find("GameStarted") != std::string::npos)
        {
            std::cout << "The game has successfully started." << std::endl;

            std::cout << "Choose game mode (1 - Man vs Man, 2 - Man vs AI, 3 - AI vs AI): ";
            std::string mode;
            std::getline(std::cin, mode);
            response = serial.sendMessage("SetMode " + mode + "\n");
            std::cout << "Server response: " << response << std::endl;

            if (mode == "3")
            {
                // Цикл для відображення стану гри в режимі AI vs AI
                while (true)
                {
                    response = serial.sendMessage("GetGameState\n");

                    // Виводимо стан дошки
                    if (response.find("BoardState:") == 0)
                    {
                        std::string boardState = response.substr(12);
                        serial.drawBoard(boardState);
                    }

                    std::cout << response << std::endl;

                    // Перевіряємо, чи гра завершилася
                    if (response.find("Wins") != std::string::npos || response.find("Draw") != std::string::npos)
                    {
                        std::cout << "The game is over!" << std::endl;
                        break;
                    }
                }
            }

            if (mode != "3")
            {
                while (true)
                {
                    std::string input;
                    std::cout << "Enter your move (1-9) or 'exit' to exit: ";
                    std::getline(std::cin, input);

                    if (input == "exit")
                    {
                        break;
                    }

                    try
                    {
                        int move = std::stoi(input);
                        if (move < 1 || move > 9)
                        {
                            std::cout << "Incorrect entry. Enter a number between 1 and 9." << std::endl;
                            continue;
                        }

                        if (mode == "2") {
                            response = serial.sendMessage("Move " + input + "\n");
                            std::cout << "Server response: " << response << std::endl;
                        }
                        else {
                            response = serial.sendMessage("Move " + input + "\n");
                            std::cout << "Server response:\n" << response << std::endl;
                        }

                        if (response.find("BoardState:") == 0)
                        {
                            std::string boardState = response.substr(12);
                            serial.drawBoard(boardState);
                        }

                        if (response.find("Wins") != std::string::npos || response.find("Draw") != std::string::npos)
                        {
                            std::cout << "The game is over!" << std::endl;
                            break;
                        }
                    }
                    catch (...)
                    {
                        std::cout << "Input recognition error. Enter the correct number." << std::endl;
                    }
                }

                serial.disconnect();
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "An error has occurred: " << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Unknown error!" << std::endl;
    }

    system("pause");
    return 0;
}