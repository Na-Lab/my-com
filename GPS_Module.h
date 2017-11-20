#ifndef GPS_MODULE_h
#define GPS_MODULE_h
/**
 * @brief GPSモジュール制御クラス
 *
 */
class GPS {
 public:
  /**
   * @brief コンストラクタ
   *
   * ボーレートを設定してGPSモジュールとシリアル通信を開始します。
   * @param[in] baudRate シリアル通信のボーレート
   */
  GPS(const int baudRate);

  /**
   * @brief 測位情報更新関数
   *
   * GPSモジュールからNMEAフォーマットを受信して緯度経度を取得します。
   */
  void update();

  double getLatitude();
  double getLongitude();

 private:
  /**
   * @brief 緯度経度調整関数
   *
   * 緯度経度を数値化し、60進法から10進法に変換します。
   * @param[in] data  文字列座標データへのポインタ
   * @return          10進法数値座標
   */
  double dataCoordinator(char *data);

  double latitude;  /**< 緯度 */
  double longitude; /**< 経度 */
};

#endif
