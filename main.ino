#include <Arduino.h>

#include "9_axis.h"
#include "GPS_Module.h"
#include "servo_motor.h"

ServoMotor motor;
NineAxis   sensor;
GPS        gps(9600);

// 現在地目的地間の角度算出関数
double getTargetAngle(double targetLatitude, double targetLongitude) {
  double x1, x2, y1, y2;
  x1           = gps.getLongitude() * PI / 180;
  x2           = targetLongitude * PI / 180;
  y1           = gps.getLatitude() * PI / 180;
  y2           = targetLatitude * PI / 180;
  double dx    = x2 - x1;
  double r     = 6378.137;
  double angle =(PI/2 )- atan2(sin(dx), cos(y1) * tan(y2) - sin(y1) * cos(dx)) *180.0 / PI;

  if (angle < 0) return (angle + 360.0);

  return angle;
}
void setup() {
  motor.begin();
  sensor.begin();
  delay(1000);
}

void loop() {
  double targetLatitude  = 35.373014;
  double targetLongitude = 139.271313;
  gps.update();
  sensor.update();

  double destination =
  getTargetAngle(targetLatitude, targetLongitude) + sensor.getAzimuth();
  destination += 180;
  if(destination >= 360){
    destination = destination - 360;
  }
  motor.turn(destination);
}
