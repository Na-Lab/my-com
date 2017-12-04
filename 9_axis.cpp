#include <Arduino.h>
#include <Wire.h>

#include "9_axis.h"

NineAxis::NineAxis() : MPU9250Address(0x68), MPU9250CompassAddress(0x0C) {
  // いい初期化の方法ないかな
  rawAccel.x = 0.0;
  rawAccel.y = 0.0;
  rawAccel.z = 0.0;
  rawGyro.x  = 0.0;
  rawGyro.y  = 0.0;
  rawGyro.z  = 0.0;
  rawMag.x   = 0.0;
  rawMag.y   = 0.0;
  rawMag.z   = 0.0;
}

void NineAxis::begin() {
  const byte PWR_MGMT_1 = 0x6B;  // パワーマネジメントレジスタ1
  const byte INT_PIN_CFG = 0x37;  // 割り込みピンコンフィギュレーションレジスタ

  const byte AK8963_CNTL = 0x0A;  // コントロールレジスタ1
  const byte AK8963_ASAX = 0x10;  // 感度調整レジスタ

  Wire.begin();
  writeByte(MPU9250Address, PWR_MGMT_1, 0x00);  // スリープモードをクリア
  delay(10);
  writeByte(MPU9250Address, INT_PIN_CFG, 0x02);  // バイパスモードをセット
  delay(10);

  writeByte(MPU9250CompassAddress, AK8963_CNTL,
            0x00);  // 地磁気センサをリセット
  delay(10);
  writeByte(MPU9250CompassAddress, AK8963_CNTL,
            0x0F);  // ROMアクセスモードに変更
  delay(10);
  byte tempData[3] = {0, 0, 0};
  readBytes(MPU9250CompassAddress, AK8963_ASAX, 3, tempData);
  magCalibration[0] = (double)(tempData[0] - 128) / 256. + 1.;
  magCalibration[1] = (double)(tempData[1] - 128) / 256. + 1.;
  magCalibration[2] = (double)(tempData[2] - 128) / 256. + 1.;
  writeByte(MPU9250CompassAddress, AK8963_CNTL,
            0x00);  // パワーダウンモードに戻す
  delay(10);
  writeByte(MPU9250CompassAddress, AK8963_CNTL, 0x12);  // 連続測定モード1に変更
  delay(10);
}

void NineAxis::update() {
  readAccelData();
  readGyroData();
  readMagData();
}

void NineAxis::writeByte(byte slaveAddress, byte registerAddress, byte data) {
  Wire.beginTransmission(slaveAddress);
  Wire.write(registerAddress);
  Wire.write(data);
  Wire.endTransmission();
}

byte NineAxis::readByte(byte slaveAddress, byte registerAddress) {
  Wire.beginTransmission(slaveAddress);
  Wire.write(registerAddress);
  Wire.endTransmission(false);
  Wire.requestFrom((int)slaveAddress, 1);

  if (!Wire.available()) return -1;
  return (Wire.read());
}

void NineAxis::readBytes(byte slaveAddress, byte registerAddress, byte count,
                         byte *data) {
  Wire.beginTransmission(slaveAddress);
  Wire.write(registerAddress);
  Wire.endTransmission(false);
  uint8_t i = 0;
  Wire.requestFrom(slaveAddress, count);
  while (Wire.available()) {
    data[i] = Wire.read();
    i++;
  }
}

void NineAxis::readAccelData() {
  const double unitConversion = 0.061;  /**< 単位換算値 */
  byte         rawData[6]     = {0};    /**< 生データ */

  readBytes(MPU9250Address, 0x3b, 6, rawData);
  rawAccel.x = ((rawData[0] << 8) | rawData[1]) * unitConversion;
  rawAccel.y = ((rawData[2] << 8) | rawData[3]) * unitConversion;
  rawAccel.z = ((rawData[4] << 8) | rawData[5]) * unitConversion;
}

void NineAxis::readGyroData() {
  const double unitConversion = 0.00763;        /**< 単位換算値 */
  byte         rawData[6]     = {0};            /**< 生データ */

  readBytes(MPU9250Address, 0x43, 6, rawData);
  rawGyro.x = ((rawData[0] << 8) | rawData[1]) * unitConversion;
  rawGyro.y = ((rawData[2] << 8) | rawData[3]) * unitConversion;
  rawGyro.z = ((rawData[4] << 8) | rawData[5]) * unitConversion;
}

void NineAxis::readMagData() {
  const double unitConversion = 0.15;    /**< 単位換算値 */
  const double offsetX        = -57.588; /**< X軸オフセット */
  const double offsetY        = -4.93; /**< Y軸オフセット */
  const double offsetZ        = 0.0;     /**< Z軸オフセット */
  byte         rawData[6]     = {0};     /**< 生データ */

  readBytes(MPU9250CompassAddress, 0x03, 6, rawData);
  // 注意：リトルエンディアン
  rawMag.x =
      ((rawData[1] << 8) | rawData[0]) * unitConversion * magCalibration[0] +
      offsetX;
  rawMag.y =
      ((rawData[3] << 8) | rawData[2]) * unitConversion * magCalibration[1] +
      offsetY;
  rawMag.z =
      ((rawData[5] << 8) | rawData[4]) * unitConversion * magCalibration[2] +
      offsetZ;
  readByte(MPU9250CompassAddress, 0x09);
}

double NineAxis::getAzimuth() {
  double per     = rawMag.x / rawMag.y;
  double azimuth = atan(per) * 180.0 / PI;

  // // 1,2,3,4それぞれで場合わけｘが０のときも
  // //座標軸　yが→　xが↑　で考えている．注意

  // arctan結果の第1象限以外の処理
  if ((rawMag.x > 0.0) && (rawMag.y < 0.0)) {
    azimuth += 180.0;  // 第2象限
  } else if ((rawMag.x < 0.0) && (rawMag.y < 0.0)) {
    azimuth += 180.0;  // 第3象限
  } else if ((rawMag.x < 0.0) && (rawMag.y > 0.0)) {
    azimuth += 360.0;  // 第4象限
  }

  // 軸に接する場合の処理
  if ((fabs(rawMag.x) < 0.01) && (rawMag.y > 0.0)) {
    azimuth = 0.0;
  } else if ((fabs(rawMag.x) < 0.01) && (rawMag.y < 0.0)) {
    azimuth = 180.0;
  } else if ((rawMag.x > 0.0) && (fabs(rawMag.y) < 0.01)) {
    azimuth = 90.0;
  } else if ((rawMag.x < 0.0) && (fabs(rawMag.y) < 0.01)) {
    azimuth = 270.0;
  }

  return azimuth;
}

double NineAxis::getRawAccelX() { return rawAccel.x; }
double NineAxis::getRawAccelY() { return rawAccel.y; }
double NineAxis::getRawAccelZ() { return rawAccel.z; }
double NineAxis::getRawGyroX() { return rawGyro.x; }
double NineAxis::getRawGyroY() { return rawGyro.y; }
double NineAxis::getRawGyroZ() { return rawGyro.z; }
double NineAxis::getRawMagX() { return rawMag.x; }
double NineAxis::getRawMagY() { return rawMag.y; }
double NineAxis::getRawMagZ() { return rawMag.z; }
