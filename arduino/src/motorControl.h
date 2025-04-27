#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

class MotorControl {
  public:
    MotorControl(int pin1, int pin2, int pin3, int pin4, int enablePin1, int enablePin2);
    void driveForward();
    void driveBackward();
    void turnLeft();
    void turnRight();

  private:
    int motorPin1, motorPin2, motorPin3, motorPin4;
    int enablePin1, enablePin2;
};

#endif