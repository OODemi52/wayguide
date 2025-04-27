#include "hapticGuidance.h"

// DriveForwardCommand implementation
DriveForwardCommand::DriveForwardCommand(int pin1, int pin2, int ep) : 
  motorPin1(pin1), motorPin2(pin2), enablePin(ep) {
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(enablePin, OUTPUT);
  digitalWrite(enablePin, LOW);
}

void DriveForwardCommand::execute() {
  digitalWrite(enablePin, HIGH);
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, LOW);
}

void DriveForwardCommand::stop() {
  digitalWrite(enablePin, LOW);
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
}

// DriveBackwardCommand implementation
DriveBackwardCommand::DriveBackwardCommand(int pin1, int pin2, int ep) : 
  motorPin1(pin1), motorPin2(pin2), enablePin(ep) {
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(enablePin, OUTPUT);
  digitalWrite(enablePin, LOW);
}

void DriveBackwardCommand::execute() {
  digitalWrite(enablePin, HIGH);
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, HIGH);
}

void DriveBackwardCommand::stop() {
  digitalWrite(enablePin, LOW);
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
}

// VibrationCommand implementation
VibrationCommand::VibrationCommand(int pin1, int pin2, int ep) : 
  DriveForwardCommand(pin1, pin2, ep), intensity(0) {
  // Use PWM for variable intensity
  pinMode(enablePin, OUTPUT);
}

void VibrationCommand::setIntensity(int level) {
  // Clamp intensity between 0-100%
  intensity = constrain(level, 0, 100);
}

int VibrationCommand::getIntensity() const {
  return intensity;
}

void VibrationCommand::execute() {
  if (intensity > 0) {
    // Map intensity from 0-100 to 0-255 for analogWrite
    int pwmValue = map(intensity, 0, 100, 0, 255);
    analogWrite(enablePin, pwmValue);
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, LOW);
  } else {
    stop();
  }
}

// PulseVibrationCommand implementation
PulseVibrationCommand::PulseVibrationCommand(int pin1, int pin2, int ep) :
  VibrationCommand(pin1, pin2, ep), 
  pulseInterval(500), lastPulseTime(0), isVibrating(false) {
}

void PulseVibrationCommand::setPulseInterval(unsigned long interval) {
  pulseInterval = interval;
}

void PulseVibrationCommand::update() {
  unsigned long currentTime = millis();
  
  if (currentTime - lastPulseTime >= pulseInterval) {
    lastPulseTime = currentTime;
    isVibrating = !isVibrating;
    
    if (isVibrating) {
      execute();
    } else {
      stop();
    }
  }
}

// HapticMotor implementation
HapticMotor::HapticMotor(int pin1, int pin2, int enablePin) :
  vibrationCmd(pin1, pin2, enablePin),
  pulseCmd(pin1, pin2, enablePin),
  isPulsing(false) {
}

void HapticMotor::vibrate(int intensity) {
  isPulsing = false;
  vibrationCmd.setIntensity(intensity);
  vibrationCmd.execute();
}

void HapticMotor::pulse(unsigned long interval, int intensity) {
  isPulsing = true;
  pulseCmd.setIntensity(intensity);
  pulseCmd.setPulseInterval(interval);
}

void HapticMotor::stop() {
  isPulsing = false;
  vibrationCmd.stop();
  pulseCmd.stop();
}

void HapticMotor::update() {
  if (isPulsing) {
    pulseCmd.update();
  }
}

// HapticGuidanceSystem implementation
HapticGuidanceSystem::HapticGuidanceSystem(
    int leftPin1, int leftPin2, int leftEnablePin,
    int rightPin1, int rightPin2, int rightEnablePin) :
  leftMotor(leftPin1, leftPin2, leftEnablePin),
  rightMotor(rightPin1, rightPin2, rightEnablePin),
  config{2.0f, 0.5f, 20, 100, 200} { // Default configuration
}

void HapticGuidanceSystem::configure(float warningThreshold, float dangerThreshold,
                                   int minIntensity, int maxIntensity, unsigned long pulseInterval) {
  config.warningThreshold = warningThreshold;
  config.dangerThreshold = dangerThreshold;
  config.minIntensity = minIntensity;
  config.maxIntensity = maxIntensity;
  config.pulseInterval = pulseInterval;
}

void HapticGuidanceSystem::processProximity(float leftDistance, float rightDistance) {
  // Process left side proximity
  if (leftDistance < config.dangerThreshold) {
    // Danger zone - use pulsing vibration
    leftMotor.pulse(config.pulseInterval, config.maxIntensity);
  } 
  else if (leftDistance < config.warningThreshold) {
    // Warning zone - use gradual intensity
    float normalizedDistance = (config.warningThreshold - leftDistance) / 
                              (config.warningThreshold - config.dangerThreshold);
    int intensity = config.minIntensity + 
                    normalizedDistance * (config.maxIntensity - config.minIntensity);
    leftMotor.vibrate(intensity);
  } 
  else {
    leftMotor.stop();
  }
  
  // Process right side proximity
  if (rightDistance < config.dangerThreshold) {
    // Danger zone - use pulsing vibration
    rightMotor.pulse(config.pulseInterval, config.maxIntensity);
  } 
  else if (rightDistance < config.warningThreshold) {
    // Warning zone - use gradual intensity
    float normalizedDistance = (config.warningThreshold - rightDistance) / 
                              (config.warningThreshold - config.dangerThreshold);
    int intensity = config.minIntensity + 
                    normalizedDistance * (config.maxIntensity - config.minIntensity);
    rightMotor.vibrate(intensity);
  } 
  else {
    rightMotor.stop();
  }
}

void HapticGuidanceSystem::update() {
  leftMotor.update();
  rightMotor.update();
}

void HapticGuidanceSystem::stop() {
  leftMotor.stop();
  rightMotor.stop();
}