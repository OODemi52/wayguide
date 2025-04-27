#include "motorControl.h"
#include <Arduino.h>

MotorControl::MotorControl(int pin1, int pin2, int pin3, int pin4, int ep1, int ep2) {
  motorPin1 = pin1;
  motorPin2 = pin2;
  motorPin3 = pin3;
  motorPin4 = pin4;
  enablePin1 = ep1;
  enablePin2 = ep2;
  
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
  pinMode(enablePin1, OUTPUT);
  pinMode(enablePin2, OUTPUT);
  
  digitalWrite(enablePin1, LOW);
  digitalWrite(enablePin2, LOW);
}

void MotorControl::driveForward() {
  digitalWrite(enablePin1, HIGH);
  digitalWrite(enablePin2, HIGH);
  
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, HIGH);
  digitalWrite(motorPin4, LOW);
}

void MotorControl::driveBackward() {
  digitalWrite(enablePin1, HIGH);
  digitalWrite(enablePin2, HIGH);
  
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, HIGH);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, HIGH);
}

void MotorControl::turnLeft() {
  digitalWrite(enablePin1, HIGH);
  digitalWrite(enablePin2, HIGH);
  
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, HIGH);
  digitalWrite(motorPin3, HIGH);
  digitalWrite(motorPin4, LOW);
}

void MotorControl::turnRight() {
  digitalWrite(enablePin1, HIGH);
  digitalWrite(enablePin2, HIGH);
  
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, HIGH);
}