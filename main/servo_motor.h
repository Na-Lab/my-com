#ifndef SERVO_MOTOR_H
#define SERVO_MOTOR_H
#include <Servo.h>

class ServoMotor : public Servo {
 public:
  ServoMotor();

  void begin();

  void turn(double angle);

 private:
  const int pulseMax;
  const int pulseMin;
};

#endif
