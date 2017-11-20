#include <Arduino.h>

#include "9_axis.h"
#include "GPS_Module.h"
#include "servo_motor.h"

ServoMotor motor;
NineAxis   sensor;
GPS        gps(9600);

// 現在地目的地間の角度算出関数
double getTargetAngle(double targetLatitude, double targetLongitude) {
  double x     = targetLongitude - gps.getLongitude();
  double y     = targetLatitude - gps.getLatitude();
  double angle = atan(y / x) * 180.0 / PI;

  // arctan結果の第1象限以外の処理
  if ((x > 0.0) && (y < 0.0)) {
    angle += 180.0;  // 第2象限
  } else if ((x < 0.0) && (y < 0.0)) {
    angle += 180.0;  // 第3象限
  } else if ((x < 0.0) && (y > 0.0)) {
    angle += 360.0;  // 第4象限
  }

  // 軸に接する場合の処理
  if ((fabs(x) < 0.01) && (y > 0.0)) {
    angle = 0.0;
  } else if ((fabs(x) < 0.01) && (y < 0.0)) {
    angle = 180.0;
  } else if ((x > 0.0) && (fabs(y) < 0.01)) {
    angle = 90.0;
  } else if ((x < 0.0) && (fabs(y) < 0.01)) {
    angle = 270.0;
  }
  return angle;
}
void setup() {
  motor.begin();
  sensor.begin();
  Serial.begin(9600);
  delay(1000);
}

void loop() {
  double targetLatitude  = 26.210533;
  double targetLongitude = 127.681;
  gps.update();
  sensor.update();
  Serial.print("target angle: ");
  Serial.print(getTargetAngle(targetLatitude, targetLongitude));
  Serial.println("");
  Serial.print("Azimuth: ");
  Serial.print(sensor.getAzimuth());
  Serial.println("");
  motor.turn(sensor.getAzimuth());
  // motor.turn(0);
}
