# Мінімальна версія CMake
cmake_minimum_required(VERSION 3.10)

# Назва проекту
project(ClientServerProject)

# Встановлюємо стандарт C++
set(CMAKE_CXX_STANDARD 17)

# Вказуємо директорію з заголовочними файлами
include_directories(${CMAKE_SOURCE_DIR}/../Client)
include_directories(${CMAKE_SOURCE_DIR}/../3party/nlohmann)


# Додаємо виконуваний файл для клієнта
add_executable(client
    ../Client/SerialPort.cpp
    ../Client/TikTakToe.cpp
)

# Змінні для Arduino
set(ARDUINO_CLI "arduino-cli")
set(ARDUINO_BOARD "arduino:avr:uno")
set(ARDUINO_PORT "COM5")
set(ARDUINO_SRC "${CMAKE_SOURCE_DIR}/../Server/server/server.ino")

# Компіляція серверного коду для Arduino
add_custom_target(compile_server ALL
    COMMAND ${ARDUINO_CLI} compile --fqbn ${ARDUINO_BOARD} ${ARDUINO_SRC}
    COMMENT "Compiling Arduino server..."
)

# Додаємо залежність компіляції Arduino до клієнта
add_dependencies(client compile_server)

# Очищення зібраних файлів
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${CMAKE_BINARY_DIR}/client;${ARDUINO_SRC}")
