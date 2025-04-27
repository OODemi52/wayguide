#ifndef IPC_MANAGER_H
#define IPC_MANAGER_H

#include "serial_handler.h"
#include "websocket_handler.h"

class IPCManager {
public:
    IPCManager(SerialHandler& serialHandler, WebSocketHandler& websocketHandler);
    void run();

private:
    SerialHandler& serialHandler;
    WebSocketHandler& websocketHandler;
};

#endif