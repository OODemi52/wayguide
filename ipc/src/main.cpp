#include <iostream>
#include "serial_handler.h"
#include "websocket_handler.h"
#include "ipc_manager.h"

int main() {
    // Initialize Serial and WebSocket handlers
    SerialHandler serialHandler("/dev/ttyUSB0"); // Update with correct serial port
    WebSocketHandler websocketHandler(8080); // Use port number instead of URL string

    // Initialize IPC Manager
    IPCManager ipcManager(serialHandler, websocketHandler);

    // Start communication
    ipcManager.run();
    
    return 0;
}