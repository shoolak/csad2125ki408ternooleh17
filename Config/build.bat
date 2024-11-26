@echo off

REM Компіляція клієнтського додатку
g++ -o ..\Build\main.exe ..\Client\TikTakToe.cpp ..\Client\SerialPort.cpp ..\Client\SerialPort.h

REM Компіляція Arduino програми через платформу Arduino (IDE або arduino-cli)
arduino-cli compile --fqbn arduino:avr:uno ..\Server\server\server.ino