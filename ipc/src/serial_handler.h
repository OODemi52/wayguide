#ifndef SERIAL_HANDLER_H
#define SERIAL_HANDLER_H

#include <string>
#include <libserial/SerialStream.h>

class SerialHandler {
public:
    SerialHandler(const std::string& port);
    void send(const std::string& data);
    std::string receive();
    ~SerialHandler();

private:
    std::string serialPort;
    LibSerial::SerialStream serialStream;
};

#endif