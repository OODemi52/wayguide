#include "serial_handler.h"
#include <libserial/SerialStream.h>
#include <iostream>

SerialHandler::SerialHandler(const std::string& port) : serialPort(port) {
    serialStream.Open(port);
    if (!serialStream.good()) {
        std::cerr << "Failed to open serial port!" << std::endl;
    }
}

void SerialHandler::send(const std::string& data) {
    serialStream << data;
}

std::string SerialHandler::receive() {
    std::string data;
    std::getline(serialStream, data);
    return data;
}

SerialHandler::~SerialHandler() {
    if (serialStream.IsOpen()) {
        serialStream.Close();
    }
}