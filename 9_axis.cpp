#include <Arduino.h>
#include <Wire.h>

#include "9_axis.h"

NineAxis::NineAxis() : MPU9250Address(0x68), MPU9250CompassAddress(0x0C) {
  // いい初期化の方法ないかな
  rawAcceleration.x        = 0.0;
  rawAcceleration.y        = 0.0;
  rawAcceleration.z        = 0.0;
  rawAngularAcceleration.x = 0.0;
  rawAngularAcceleration.y = 0.0;
  rawAngularAcceleration.z = 0.0;
  rawMagneticFluxDensity.x = 0.0;
  rawMagneticFluxDensity.y = 0.0;
  rawMagneticFluxDensity.z = 0.0;
}

void NineAxis::begin() {
  Wire.begin();
  writeRegisterData(MPU9250Address, 0x6B, 0x00);
  writeRegisterData(MPU9250Address, 0x37, 0x02);
  writeRegisterData(MPU9250CompassAddress, 0x0A, 0x12);
}

void NineAxis::update() {
  readAcceleration();
  readAngularAcceleration();
  readMagneticFluxDensity();
}

void NineAxis::writeRegisterData(byte slaveAddress, byte registerAddress,
                                 byte data) {
  Wire.beginTransmission(slaveAddress);
  Wire.write(registerAddress);
  Wire.write(data);
  Wire.endTransmission();
}

byte NineAxis::readRegisterData(byte slaveAddress, byte registerAddress) {
  Wire.beginTransmission(slaveAddress);
  Wire.write(registerAddress);
  Wire.endTransmission(false);
  Wire.requestFrom((int)slaveAddress, 1);

  if (!Wire.available()) return -1;
  return (Wire.read());
}

void NineAxis::readAcceleration() {
  const double unitConversion = 0.061;
  int          top, bottom;

  top               = readRegisterData(MPU9250Address, 0x3b);
  bottom            = readRegisterData(MPU9250Address, 0x3c);
  rawAcceleration.x = ((top << 8) | bottom) * unitConversion;

  top               = readRegisterData(MPU9250Address, 0x3d);
  bottom            = readRegisterData(MPU9250Address, 0x3e);
  rawAcceleration.y = ((top << 8) | bottom) * unitConversion;

  top               = readRegisterData(MPU9250Address, 0x3f);
  bottom            = readRegisterData(MPU9250Address, 0x40);
  rawAcceleration.z = ((top << 8) | bottom) * unitConversion;
}

void NineAxis::readAngularAcceleration() {
  const double unitConversion = 0.00763;
  int          top, bottom;

  top                      = readRegisterData(MPU9250Address, 0x43);
  bottom                   = readRegisterData(MPU9250Address, 0x44);
  rawAngularAcceleration.x = ((top << 8) | bottom) * unitConversion;

  top                      = readRegisterData(MPU9250Address, 0x45);
  bottom                   = readRegisterData(MPU9250Address, 0x46);
  rawAngularAcceleration.y = ((top << 8) | bottom) * unitConversion;

  top                      = readRegisterData(MPU9250Address, 0x47);
  bottom                   = readRegisterData(MPU9250Address, 0x48);
  rawAngularAcceleration.z = ((top << 8) | bottom) * unitConversion;
}

// 注意：これのみリトルエンディアンである
void NineAxis::readMagneticFluxDensity() {
  const double               unitConversion = 0.15; /**< 単位換算 */
  const double               offsetX     = -39.588; /**< X軸オフセット */
  const double               offsetY     = -9.9535; /**< Y軸オフセット */
  struct magneticFluxDensity avgMagnetic = {0.0};
  int                        top, bottom;

  for (int i = 0; i < 10; i++) {
    bottom = readRegisterData(MPU9250CompassAddress, 0x03);
    top    = readRegisterData(MPU9250CompassAddress, 0x04);
    avgMagnetic.x += ((top << 8) | bottom) * unitConversion + offsetX;

    bottom = readRegisterData(MPU9250CompassAddress, 0x05);
    top    = readRegisterData(MPU9250CompassAddress, 0x06);
    avgMagnetic.y += ((top << 8) | bottom) * unitConversion + offsetY;

    bottom = readRegisterData(MPU9250CompassAddress, 0x07);
    top    = readRegisterData(MPU9250CompassAddress, 0x08);
    avgMagnetic.z += ((top << 8) | bottom) * unitConversion;

    // データをリフレッシュするには0x09を読む必要がある
    readRegisterData(MPU9250CompassAddress, 0x09);
  }

  rawMagneticFluxDensity.x = avgMagnetic.x / 10.0;
  rawMagneticFluxDensity.y = avgMagnetic.y / 10.0;
  rawMagneticFluxDensity.z = avgMagnetic.z / 10.0;
}

double NineAxis::getAzimuth() {
  double per     = rawMagneticFluxDensity.x / rawMagneticFluxDensity.y;
  double azimuth = atan(per) * 180.0 / PI;


  // // 1,2,3,4それぞれで場合わけｘが０のときも
  // //座標軸　yが→　xが↑　で考えている．注意

  // arctan結果の第1象限以外の処理
  if ((rawMagneticFluxDensity.x > 0.0) && (rawMagneticFluxDensity.y < 0.0)) {
    azimuth += 180.0;  // 第2象限
  } else if ((rawMagneticFluxDensity.x < 0.0) &&
             (rawMagneticFluxDensity.y < 0.0)) {
    azimuth += 180.0;  // 第3象限
  } else if ((rawMagneticFluxDensity.x < 0.0) &&
             (rawMagneticFluxDensity.y > 0.0)) {
    azimuth += 360.0;  // 第4象限
  }

  // 軸に接する場合の処理
  if ((fabs(rawMagneticFluxDensity.x) < 0.01) &&
      (rawMagneticFluxDensity.y > 0.0)) {
    azimuth = 0.0;
  } else if ((fabs(rawMagneticFluxDensity.x) < 0.01) &&
             (rawMagneticFluxDensity.y < 0.0)) {
    azimuth = 180.0;
  } else if ((rawMagneticFluxDensity.x > 0.0) &&
             (fabs(rawMagneticFluxDensity.y) < 0.01)) {
    azimuth = 90.0;
  } else if ((rawMagneticFluxDensity.x < 0.0) &&
             (fabs(rawMagneticFluxDensity.y) < 0.01)) {
    azimuth = 270.0;
  }

  return azimuth;
}

double NineAxis::getRawAccelerationX() { return rawAcceleration.x; }
double NineAxis::getRawAccelerationY() { return rawAcceleration.y; }
double NineAxis::getRawAccelerationZ() { return rawAcceleration.z; }
double NineAxis::getRawAngularAccelerationX() {
  return rawAngularAcceleration.x;
}
double NineAxis::getRawAngularAccelerationY() {
  return rawAngularAcceleration.y;
}
double NineAxis::getRawAngularAccelerationZ() {
  return rawAngularAcceleration.z;
}
double NineAxis::getRawMagneticFluxDensityX() {
  return rawMagneticFluxDensity.x;
}
double NineAxis::getRawMagneticFluxDensityY() {
  return rawMagneticFluxDensity.y;
}
double NineAxis::getRawMagneticFluxDensityZ() {
  return rawMagneticFluxDensity.z;
}
