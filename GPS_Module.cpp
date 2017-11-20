#include "GPS_Module.h"
#include <Arduino.h>

GPS::GPS(const int baudRate) { Serial1.begin(baudRate); }

void GPS::update() {
  // NMEAの取得
  String nmeaData;
  do {
    nmeaData = Serial1.readStringUntil('\n');
  } while (nmeaData.indexOf("$GPGGA") == -1);

  // トークンの抽出
  const char nmeaDelimitter = ',';
  char *     p_latitude;
  char *     p_longitude;
  strtok((char *)nmeaData.c_str(), &nmeaDelimitter);  // $GPGGA
  strtok(NULL, &nmeaDelimitter);                      // UTC時刻
  p_latitude = strtok(NULL, &nmeaDelimitter);         // 緯度
  strtok(NULL, &nmeaDelimitter);                      // 南北
  p_longitude = strtok(NULL, &nmeaDelimitter);        // 経度

  latitude  = GPS::dataCoordinator(p_latitude);
  longitude = GPS::dataCoordinator(p_longitude);
}

double GPS::dataCoordinator(char *data) {
  double temp, deg, min;

  temp = atof(data);
  deg  = (int)(temp / 100);
  min  = temp - deg * 100;
  return (double)(deg + min / 60.0);
}
double GPS::getLatitude() { return latitude; }
double GPS::getLongitude() { return longitude; }
