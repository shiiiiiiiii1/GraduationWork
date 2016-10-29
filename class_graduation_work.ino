#include <Time.h>   // 時間のセット
#include <TimeLib.h>   // 時間のセット
#include <Wire.h>   // I2C
#include <SSCI_BME280.h>   // 温湿度気圧センサ
#include <I2Cdev.h>   // 6軸加速度センサ
#include <MPU6050.h>   // 6軸加速度センサ


SSCI_BME280 bme280;
uint8_t temp_press_hum_i2c_addr = 0x76;   // 温湿度気圧センサ I2C Address

MPU6050 accelgyro;   // 6軸加速度センサ
int16_t ax, ay, az;
int16_t gx, gy, gz;

void setup() {
  uint8_t osrs_t = 1;   // Temperature oversampling x 1
  uint8_t osrs_p = 1;   // Pressure oversampling x 1
  uint8_t osrs_h = 1;   // Humidity oversampling x 1
  uint8_t bme280mode = 3;   // Normal mode
  uint8_t t_sb = 5;   // Tstandby 1000ms
  uint8_t filter = 0;   // Filter off
  uint8_t spi3w_en = 0;   // SPI Disable

  Serial.begin(9600);
  Wire.begin();
  bme280.setMode(temp_press_hum_i2c_addr, osrs_t, osrs_p, osrs_h, bme280mode, t_sb, filter, spi3w_en);

  bme280.readTrim();

  accelgyro.initialize();
}


void loop() {
  double temp_act, press_act, hum_act;   // 最終的に表示される値を入れる変数

  bme280.readData(&temp_act, &press_act, &hum_act);   // 温湿度気圧センサの値を読み込む
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);   // 6軸加速度センサの値を読み込む

  setTime(12, 38, 0, 25, 10, 2016);   // 現在の時刻を入れて実験開始 ---------------------------------------------------------------------------------
  Serial.print(year()); Serial.print("/"); Serial.print(month()); Serial.print("/"); Serial.print(day()); Serial.print(" ");
  Serial.print(hour()); Serial.print(":"); Serial.print(minute()); Serial.print(":"); Serial.print(second());
  Serial.print("  TEMP : "); Serial.print(temp_act);
  Serial.print(" DegC / PRESS : "); Serial.print(press_act);
  Serial.print(" hPa / HUM : "); Serial.print(hum_act);
  Serial.print(" % / ax : "); Serial.print(ax);
  Serial.print(" g / ay : "); Serial.print(ay);
  Serial.print(" g / az : "); Serial.println(az);

  delay(1000);
}