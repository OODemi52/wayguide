#ifndef HAPTIC_GUIDANCE_H
#define HAPTIC_GUIDANCE_H

#include <Arduino.h>

// Base class for motor commands
class MotorCommand {
public:
  virtual void execute() = 0;
  virtual void stop() = 0;
  virtual ~MotorCommand() {}
};

// Forward and Backward commands as base motor operations
class DriveForwardCommand : public MotorCommand {
protected:
  int motorPin1, motorPin2;
  int enablePin;
  
public:
  DriveForwardCommand(int pin1, int pin2, int ep);
  virtual void execute() override;
  virtual void stop() override;
};

class DriveBackwardCommand : public MotorCommand {
protected:
  int motorPin1, motorPin2;
  int enablePin;
  
public:
  DriveBackwardCommand(int pin1, int pin2, int ep);
  virtual void execute() override;
  virtual void stop() override;
};

// Vibration control with intensity level
class VibrationCommand : public DriveForwardCommand {
private:
  int intensity; // 0-255 for PWM control
  
public:
  VibrationCommand(int pin1, int pin2, int ep);
  void setIntensity(int level); // 0-100%
  int getIntensity() const;
  virtual void execute() override;
};

// Pulsating vibration with timing control
class PulseVibrationCommand : public VibrationCommand {
private:
  unsigned long pulseInterval; // in milliseconds
  unsigned long lastPulseTime;
  bool isVibrating;
  
public:
  PulseVibrationCommand(int pin1, int pin2, int ep);
  void setPulseInterval(unsigned long interval); // in milliseconds
  void update(); // Call in loop to handle timing
};

// Controls a single vibration motor
class HapticMotor {
private:
  VibrationCommand vibrationCmd;
  PulseVibrationCommand pulseCmd;
  bool isPulsing;
  
public:
  HapticMotor(int pin1, int pin2, int enablePin);
  void vibrate(int intensity);
  void pulse(unsigned long interval, int intensity);
  void stop();
  void update(); // Call in loop
};

// Configuration for proximity thresholds and feedback levels
struct ProximityConfig {
  float warningThreshold;     // Distance in meters to begin feedback
  float dangerThreshold;      // Distance in meters for urgent feedback
  int minIntensity;           // Minimum vibration intensity (0-100)
  int maxIntensity;           // Maximum vibration intensity (0-100)
  unsigned long pulseInterval; // Milliseconds between pulses when in danger zone
};

// Main controller that manages both motors and proximity response
class HapticGuidanceSystem {
private:
  HapticMotor leftMotor;
  HapticMotor rightMotor;
  ProximityConfig config;
  
public:
  HapticGuidanceSystem(int leftPin1, int leftPin2, int leftEnablePin,
                      int rightPin1, int rightPin2, int rightEnablePin);
  
  // Configure thresholds and feedback levels
  void configure(float warningThreshold, float dangerThreshold,
                int minIntensity, int maxIntensity, unsigned long pulseInterval);
  
  // Process proximity data and trigger appropriate feedback
  void processProximity(float leftDistance, float rightDistance);
  
  // Call in main loop to handle timing-based effects
  void update();
  
  // Stop all motor activity
  void stop();
};

#endif // HAPTIC_GUIDANCE_H