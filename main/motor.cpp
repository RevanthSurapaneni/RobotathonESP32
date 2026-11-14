
#include <Arduino.h>

#include "motor.h"
#include "robot_config.h"

namespace {
// drv8833 uses one pin for each direction
void setSingleMotor(uint8_t in1, uint8_t in2, int speed) {
  speed = constrain(speed, -kMotorMax, kMotorMax);
  if (speed > 0) {
    analogWrite(in1, speed);
    digitalWrite(in2, LOW);
  } else if (speed < 0) {
    digitalWrite(in1, LOW);
    analogWrite(in2, -speed);
  } else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
  }
}

}  // namespace

void setupMotor() {
  pinMode(RobotPins::leftMotorIn1, OUTPUT);
  pinMode(RobotPins::leftMotorIn2, OUTPUT);
  pinMode(RobotPins::rightMotorIn1, OUTPUT);
  pinMode(RobotPins::rightMotorIn2, OUTPUT);
  stopMotors();
}

void setMotorSpeeds(int left, int right) {
  setSingleMotor(RobotPins::leftMotorIn1, RobotPins::leftMotorIn2, left);
  setSingleMotor(RobotPins::rightMotorIn1, RobotPins::rightMotorIn2, right);

}

void stopMotors() {
  setMotorSpeeds(0, 0);
}
