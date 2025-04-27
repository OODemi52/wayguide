#include "proximityIPC.h"

ProximityReceiver::ProximityReceiver() : 
  newDataAvailable(false), 
  state(WAITING_FOR_START),
  dataIndex(0) {
  // Initialize the message with safe defaults
  currentMessage.leftDistance = 999.9f;
  currentMessage.rightDistance = 999.9f;
  currentMessage.timestamp = 0;
}

void ProximityReceiver::processIncomingByte(byte data) {
  switch (state) {
    case WAITING_FOR_START:
      if (data == MESSAGE_START) {
        state = RECEIVING_DATA;
        dataIndex = 0;
      }
      break;
      
    case RECEIVING_DATA:
      if (dataIndex < sizeof(ProximityMessage)) {
        messageBuffer[dataIndex++] = data;
      }
      
      // Check if we've received all the data
      if (dataIndex >= sizeof(ProximityMessage)) {
        // Next byte should be the end marker
        state = WAITING_FOR_START;
        
        // Only process the message if it ends correctly
        if (data == MESSAGE_END) {
          // Copy the buffer data to our message structure
          memcpy(&currentMessage, messageBuffer, sizeof(ProximityMessage));
          newDataAvailable = true;
        }
      }
      break;
  }
}

bool ProximityReceiver::hasNewData() const {
  return newDataAvailable;
}

ProximityMessage ProximityReceiver::getLatestData() {
  newDataAvailable = false;
  return currentMessage;
}