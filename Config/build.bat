@echo off

g++ -o ..\Build\main.exe ..\Client\TikTakToe.cpp ..\Client\SerialPort.cpp ..\Client\SerialPort.h
if %errorlevel% neq 0 (
    echo [ERROR] Compilation of client application failed.
    pause
    exit /b %errorlevel%
)




arduino-cli compile --fqbn arduino:avr:uno ..\Server\server\server.ino
if %errorlevel% neq 0 (
    echo [ERROR] Compilation of Arduino code failed.
    pause
    exit /b %errorlevel%
)

echo [INFO] Uploading server code to Arduino...
arduino-cli upload -p COM5 --fqbn arduino:avr:uno ..\Server\server\server.ino
if %errorlevel% neq 0 (
    echo [ERROR] Failed to upload server code to Arduino.
    pause
    exit /b %errorlevel%
)
echo [INFO] Server code uploaded successfully.