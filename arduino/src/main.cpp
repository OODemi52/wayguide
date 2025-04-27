#include <Arduino.h>
#include "hapticGuidance.h"
#include "proximityIPC.h"

// Define pins
#define LEFT_MOTOR_PIN1 22
#define LEFT_MOTOR_PIN2 23
#define LEFT_MOTOR_ENABLE 12

#define RIGHT_MOTOR_PIN1 24
#define RIGHT_MOTOR_PIN2 25
#define RIGHT_MOTOR_ENABLE 13

// Create the main controller
HapticGuidanceSystem guidanceSystem(
  LEFT_MOTOR_PIN1, LEFT_MOTOR_PIN2, LEFT_MOTOR_ENABLE,
  RIGHT_MOTOR_PIN1, RIGHT_MOTOR_PIN2, RIGHT_MOTOR_ENABLE
);

// Create the proximity data receiver
ProximityReceiver proximityReceiver;

// Configuration values (can be adjusted as needed)
const float WARNING_THRESHOLD = 2.0f;  // Meters
const float DANGER_THRESHOLD = 0.5f;   // Meters
const int MIN_INTENSITY = 20;          // Minimum vibration intensity (0-100)
const int MAX_INTENSITY = 100;         // Maximum vibration intensity (0-100)
const int PULSE_INTERVAL = 200;        // Milliseconds between pulses

// For debugging
unsigned long lastDebugPrint = 0;
const unsigned long DEBUG_INTERVAL = 1000; // 1 second

void setup() {
  Serial.begin(115200);
  Serial.println("com.arduino.wayguideprotocol")
  Serial.println("Haptic Guidance System Ready");
  
  // Configure the guidance system with default values
  guidanceSystem.configure(WARNING_THRESHOLD, DANGER_THRESHOLD, 
                           MIN_INTENSITY, MAX_INTENSITY, PULSE_INTERVAL);
  
  // For testing without camera input
  Serial.println("Send test data with format: L<left_distance>,R<right_distance>");
  Serial.println("Example: L1.5,R2.3");
}

void processSerialCommand() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    
    // Check for test proximity data in format L<left_distance>,R<right_distance>
    if (command.startsWith("L") && command.indexOf(",R") > 0) {
      int commaPos = command.indexOf(",");
      float leftDistance = command.substring(1, commaPos).toFloat();
      float rightDistance = command.substring(commaPos + 2).toFloat();
      
      Serial.print("Test proximity: Left=");
      Serial.print(leftDistance);
      Serial.print("m, Right=");
      Serial.print(rightDistance);
      Serial.println("m");
      
      guidanceSystem.processProximity(leftDistance, rightDistance);
    }
    // For testing individual motors
    else if (command == "left") {
      Serial.println("Testing left motor");
      guidanceSystem.processProximity(0.3, 999.0); // Close on left, far on right
    }
    else if (command == "right") {
      Serial.println("Testing right motor");
      guidanceSystem.processProximity(999.0, 0.3); // Far on left, close on right
    }
    else if (command == "stop") {
      Serial.println("Stopping all motors");
      guidanceSystem.stop();
    }
    else if (command.startsWith("config:")) {
      // Format: config:warning,danger,min,max,pulse
      // Example: config:2.0,0.5,20,100,200
      String params = command.substring(7);
      int pos1 = params.indexOf(',');
      int pos2 = params.indexOf(',', pos1+1);
      int pos3 = params.indexOf(',', pos2+1);
      int pos4 = params.indexOf(',', pos3+1);
      
      if (pos1 > 0 && pos2 > 0 && pos3 > 0 && pos4 > 0) {
        float warning = params.substring(0, pos1).toFloat();
        float danger = params.substring(pos1+1, pos2).toFloat();
        int minInt = params.substring(pos2+1, pos3).toInt();
        int maxInt = params.substring(pos3+1, pos4).toInt();
        unsigned long pulse = params.substring(pos4+1).toInt();
        
        guidanceSystem.configure(warning, danger, minInt, maxInt, pulse);
        Serial.println("Configuration updated");
      }
    }
  }
}

void processIPCData() {
  // Process each incoming byte
  while (Serial.available()) {
    byte data = Serial.read();
    proximityReceiver.processIncomingByte(data);
  }
  
  // Check if we have new proximity data
  if (proximityReceiver.hasNewData()) {
    ProximityMessage message = proximityReceiver.getLatestData();
    
    // Debug print (not too frequently)
    unsigned long currentTime = millis();
    if (currentTime - lastDebugPrint > DEBUG_INTERVAL) {
      lastDebugPrint = currentTime;
      Serial.print("Proximity data: Left=");
      Serial.print(message.leftDistance);
      Serial.print("m, Right=");
      Serial.print(message.rightDistance);
      Serial.println("m");
    }
    
    // Process the proximity data
    guidanceSystem.processProximity(message.leftDistance, message.rightDistance);
  }
}

void loop() {
  // Process any commands from the serial interface
  processSerialCommand();
  
  // Process any incoming proximity data from the IPC
  processIPCData();
  
  // Update the motors (needed for pulsing effects)
  guidanceSystem.update();

  // Delay to give system time
  delay(10);
}