#ifndef PROXIMITY_IPC_H
#define PROXIMITY_IPC_H

#include <Arduino.h>

// Message structure for proximity data
struct ProximityMessage {
  float leftDistance;   // Distance in meters from left sensor
  float rightDistance;  // Distance in meters from right sensor
  uint32_t timestamp;   // Timestamp in milliseconds
};

// Protocol constants
const byte MESSAGE_START = 0xAA;
const byte MESSAGE_END = 0xFF;
const int MESSAGE_SIZE = sizeof(ProximityMessage) + 2; // +2 for start/end bytes

class ProximityReceiver {
private:
  ProximityMessage currentMessage;
  bool newDataAvailable;
  
public:
  ProximityReceiver();
  void processIncomingByte(byte data);
  bool hasNewData() const;
  ProximityMessage getLatestData();
  
private:
  enum ReceiveState {
    WAITING_FOR_START,
    RECEIVING_DATA
  };
  
  ReceiveState state;
  int dataIndex;
  byte messageBuffer[sizeof(ProximityMessage)];
};

#endif // PROXIMITY_IPC_H