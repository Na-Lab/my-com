#include "servo_motor.h"

ServoMotor::ServoMotor() : pulseMin(1000), pulseMax(1940) {}

void ServoMotor::begin() { attach(9, pulseMin, pulseMax); }

void ServoMotor::turn(double angle) {
  double pulse = ((pulseMax - pulseMin) / 360.0) * angle + pulseMin;
  writeMicroseconds((int)pulse);
}
