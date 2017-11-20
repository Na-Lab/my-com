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
  double angle = atan2(sin(dx), cos(y1) * tan(y2) - sin(y1) * cos(dx)) *180.0 / PI;

  if (angle < 0) return (angle + 360.0);
  return angle;
}
void setup() {
  motor.begin();
  sensor.begin();
  Serial.begin(9600);
  delay(1000);
}

void loop() {
  double targetLatitude  = 26.212561;
  double targetLongitude = 127.689168;
  gps.update();
  sensor.update();

  double destination =
      getTargetAngle(targetLatitude, targetLongitude) + sensor.getAzimuth();
  Serial.print("target angle: ");
  Serial.print(getTargetAngle(targetLatitude, targetLongitude));
  Serial.println("");
  Serial.print("Azimuth: ");
  Serial.print(sensor.getAzimuth());
  Serial.println("");
  motor.turn(destination);
  Serial.print("Latitude : ");
  Serial.println(gps.getLatitude());
  Serial.print("Longnitude : ");
  Serial.println(gps.getLongitude()); 
  // motor.turn(0);
}
