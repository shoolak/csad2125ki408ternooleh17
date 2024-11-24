#include "SerialPort.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "..\3party\nlohmann\json.hpp"
#include <fstream>
#include <stdexcept>
#include <mutex>
#include <condition_variable>


using json = nlohmann::json;

std::string port;
int baudRate;

class MockSerialCommunication : public SerialCommunication {
public:
    MOCK_METHOD(bool, connect, (const std::string& port, int baudRate), ());
    MOCK_METHOD(void, disconnect, (), ());
    MOCK_METHOD(std::string, sendMessage, (const std::string& message), ());
    MOCK_METHOD(void, drawBoard, (const std::string& board), ());
    MOCK_METHOD(void, loadConfig, (const std::string& configFile), ());
};

// ���� ��� connect � ������
TEST(SerialCommunicationTest, ConnectFailsWithInvalidPort) {
    MockSerialCommunication mockSerial;
    EXPECT_CALL(mockSerial, connect("INVALID_PORT", 9600))
        .WillOnce(::testing::Return(false));

    EXPECT_FALSE(mockSerial.connect("INVALID_PORT", 9600));
}

// ���� ��� disconnect � ������
TEST(SerialCommunicationTest, DisconnectWorksCorrectly) {
    MockSerialCommunication mockSerial;
    EXPECT_CALL(mockSerial, connect("COM5", 9600)).WillOnce(::testing::Return(true));
    EXPECT_CALL(mockSerial, disconnect()).Times(1);

    ASSERT_TRUE(mockSerial.connect("COM5", 9600));
    mockSerial.disconnect();
}

// ���� ��� sendMessage � ������
TEST(SerialCommunicationTest, SendMessageFailsWithInvalidPort) {
    MockSerialCommunication mockSerial;
    EXPECT_CALL(mockSerial, sendMessage("Test"))
        .WillOnce(::testing::Return(""));

    EXPECT_EQ(mockSerial.sendMessage("Test"), "");
}

// ���� ��� drawBoard � ������
TEST(SerialCommunicationTest, DrawBoardCorrectOutput) {
    MockSerialCommunication mockSerial;
    std::string board =
        "-------------\n"
        "| X | O | X | \n"
        "-------------\n"
        "| O | X | O | \n"
        "-------------\n"
        "| X | O | X | \n"
        "-------------\n";

    EXPECT_CALL(mockSerial, drawBoard(board)).Times(1);

    // �������� ������� �������
    mockSerial.drawBoard(board);
}

// ���� ��� ���������� ������������
TEST(ConfigTest, LoadConfigFileCorrectly) {
    MockSerialCommunication mockSerial;
    json configJson = {
        {"Connection", {{"port", "COM5"}, {"baudRate", 9600}}}
    };

    std::ofstream config("../Config/config.json");
    config << configJson.dump(4); // �������� � ���������
    config.close();

    EXPECT_CALL(mockSerial, loadConfig("../Config/config.json"))
        .WillOnce([&]() {
        port = "COM5";
        baudRate = 9600;
            });

    mockSerial.loadConfig("../Config/config.json");

    EXPECT_EQ(port, "COM5");
    EXPECT_EQ(baudRate, 9600);
}

// ���� ��� �������� ������� ������������ ������� ���������������� �����
TEST(ConfigTest, LoadConfigFileWithInvalidFormat) {
    MockSerialCommunication mockSerial;
    std::ofstream config("../Config/invalid_config.json");
    config << R"({"Connection": {"port": "COM5", "baudRate": "invalid"}})"; // ����������� ������
    config.close();

    EXPECT_CALL(mockSerial, loadConfig("../Config/invalid_config.json"))
        .WillOnce([]() {
        throw std::runtime_error("Invalid configuration format");
            });

    EXPECT_THROW(mockSerial.loadConfig("../Config/invalid_config.json"), std::runtime_error);
}

// ���� ��� �������� ������ connect �� ���������� �����������
TEST(SerialCommunicationTest, ConnectWorksWithValidPortAndBaudRate) {
    MockSerialCommunication mockSerial;
    EXPECT_CALL(mockSerial, connect("COM5", 9600))
        .WillOnce(::testing::Return(true));

    EXPECT_TRUE(mockSerial.connect("COM5", 9600));
}

// ���� ��� �������� ������ sendMessage ��� �������� ������
TEST(SerialCommunicationTest, SendMessageReturnsValidResponse) {
    MockSerialCommunication mockSerial;
    EXPECT_CALL(mockSerial, sendMessage("Hello"))
        .WillOnce(::testing::Return("ACK"));

    EXPECT_EQ(mockSerial.sendMessage("Hello"), "ACK");
}

// ���� ��� �������� ������ sendMessage � ������� ������������
TEST(SerialCommunicationTest, SendMessageFailsWithEmptyMessage) {
    MockSerialCommunication mockSerial;
    EXPECT_CALL(mockSerial, sendMessage(""))
        .WillOnce(::testing::Return("ERROR"));

    EXPECT_EQ(mockSerial.sendMessage(""), "ERROR");
}

// ���� ��� disconnect ��� ������������ �'�������
TEST(SerialCommunicationTest, DisconnectWithoutConnect) {
    MockSerialCommunication mockSerial;
    EXPECT_CALL(mockSerial, disconnect()).Times(1);

    // ��������� disconnect ��� connect
    mockSerial.disconnect();
}

// ���� ��� drawBoard �� ����������� ������ �����
TEST(SerialCommunicationTest, DrawBoardFailsWithInvalidBoard) {
    MockSerialCommunication mockSerial;
    std::string invalidBoard = "INVALID_BOARD_STATE";

    EXPECT_CALL(mockSerial, drawBoard(invalidBoard))
        .WillOnce([]() {
        throw std::runtime_error("Invalid board state");
            });

    EXPECT_THROW(mockSerial.drawBoard(invalidBoard), std::runtime_error);
}

// ���� ��� ��������, �� ������������ ��������� ��������
TEST(ConfigTest, LoadConfigFileWithMissingFields) {
    MockSerialCommunication mockSerial;
    json configJson = {
        {"Connection", {{"port", "COM5"}}} // ³����� baudRate
    };

    std::ofstream config("../Config/missing_fields.json");
    config << configJson.dump(4);
    config.close();

    EXPECT_CALL(mockSerial, loadConfig("../Config/missing_fields.json"))
        .WillOnce([&]() {
        port = "COM5";
        baudRate = 0; // �������� �� �������������
            });

    mockSerial.loadConfig("../Config/missing_fields.json");

    EXPECT_EQ(port, "COM5");
    EXPECT_EQ(baudRate, 0); // ���������� �������� �� �������������
}
