#ifndef NINE_AXIS_H
#define NINE_AXIS_H
#include <Arduino.h>

/** 加速度センサデータ **/
struct acceleration {
  double x; /**< 加速度X軸方向 */
  double y; /**< 加速度Y軸方向 */
  double z; /**< 加速度Z軸方向 */
};

/** 角加速度センサデータ **/
struct angularAcceleration {
  double x; /**< 角加速度X軸方向 */
  double y; /**< 角加速度Y軸方向 */
  double z; /**< 角加速度Z軸方向 */
};

/** 磁束密度センサデータ **/
struct magneticFluxDensity {
  double x; /**< 磁束密度X軸方向 */
  double y; /**< 磁束密度Y軸方向 */
  double z; /**< 磁束密度Z軸方向 */
};

/**
 * @brief 9軸センサMPU-9250制御クラス
 *
 */
class NineAxis {
 public:
  NineAxis();

  void begin();
  /**
   * @brief センサ情報更新関数
   *
   * MPU-9250から加速度、角加速度、磁束密度を受信して緯度経度を取得します。
   */
  void   update();

    /**
   * @brief 方位角算出関数
   *
   * 磁束密度データから方位角を算出して出力します。
   * @return  方位角
   */
  double getAzimuth();

  double getRawAccelerationX();
  double getRawAccelerationY();
  double getRawAccelerationZ();
  double getRawAngularAccelerationX();
  double getRawAngularAccelerationY();
  double getRawAngularAccelerationZ();
  double getRawMagneticFluxDensityX();
  double getRawMagneticFluxDensityY();
  double getRawMagneticFluxDensityZ();

 private:
  const byte MPU9250Address; /**< MPU9250のデバイスアドレス */
  const byte MPU9250CompassAddress; /**< 地磁気センサのデバイスアドレス */


  struct acceleration        rawAcceleration;        /**< 加速度[mG] */
  struct angularAcceleration rawAngularAcceleration; /**< 角加速度[deg/s] */
  struct magneticFluxDensity rawMagneticFluxDensity; /**< 磁束密度[microT] */

  /**
   * @brief レジスタデータ書き込み関数
   *
   * MPU-9250内のレジスタにデータを1バイト書き込みます。
   * @param[in] slaveAddress    通信対象アドレス
   * @param[in] registerAddress 書き込みレジスタアドレス
   * @param[in] data            書き込みデータ
   */
  void writeRegisterData(byte slaveAddress, byte registerAddress, byte data);

  /**
   * @brief レジスタデータ読み取り関数
   *
   * MPU-9250内のレジスタからデータを1バイト読み取ります。
   * @param[in] slaveAddress    通信対象アドレス
   * @param[in] registerAddress 読み取りレジスタアドレス
   * @return                    受信データ
   */
  byte readRegisterData(byte slaveAddress, byte registerAddress);

  /**
   * @brief 加速度読み取り関数
   *
   * MPU-9250から加速度を読み取ります。
   */
  void readAcceleration();

  /**
   * @brief 角加速度読み取り関数
   *
   * MPU-9250から角加速度を読み取ります。
   */
  void readAngularAcceleration();

  /**
   * @brief 磁束密度読み取り関数
   *
   * MPU-9250から磁束密度を読み取ります。
   */
  void readMagneticFluxDensity();
};

#endif
