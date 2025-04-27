#include "ipc_manager.h"
#include <iostream>

IPCManager::IPCManager(SerialHandler& serialHandler, WebSocketHandler& websocketHandler) 
    : serialHandler(serialHandler), websocketHandler(websocketHandler) {
    
    // Set up WebSocket message handler
    websocketHandler.set_message_handler([this](const std::string& data) {
        // Forward WebSocket messages to Serial
        this->serialHandler.send(data);
    });
}

void IPCManager::run() {
    // Start the WebSocket server
    websocketHandler.start();
    
    // Main loop to handle Serial data
    std::string data;
    while (true) {
        // Read from Serial and forward to WebSocket
        data = serialHandler.receive();
        if (!data.empty()) {
            websocketHandler.send(data);
        }
    }
}