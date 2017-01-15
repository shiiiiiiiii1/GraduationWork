// ライブラリ
#include <ESP8266WiFi.h>   // WROOM
#include <Time.h>   // 時間のセット
#include <TimeLib.h>   // 時間のセット
#include <Wire.h>   // I2C
#include <SSCI_BME280.h>   // 温湿度気圧
#include <I2Cdev.h>   // 6軸加速度
#include <MPU6050.h>   // 6軸加速度
#include <Adafruit_Sensor.h>   // 照度
#include <Adafruit_TSL2561_U.h>   // 照度
// ヘッダーファイル
#include "wifi_set.h"
#include "global_setup.h"
#include "function.h"

void setup() {
  uint8_t osrs_t = 1;   // Temperature oversampling x 1
  uint8_t osrs_p = 1;   // Pressure oversampling x 1
  uint8_t osrs_h = 1;   // Humidity oversampling x 1
  uint8_t bme280mode = 3;   // Normal mode
  uint8_t t_sb = 5;   // Tstandby 1000ms
  uint8_t filter = 0;   // Filter off
  uint8_t spi3w_en = 0;   // SPI Disable

  // Serial.begin(115200);
  WiFi.begin(ssid, password);
  Wire.begin();

  bme280.setMode(temp_press_hum_i2c_addr, osrs_t, osrs_p, osrs_h, bme280mode, t_sb, filter, spi3w_en);
  bme280.readTrim();

  accelgyro.initialize();

// ------------------------------- 現在の時刻を入れて実験開始 -------------------------------
  setTime(14, 29, 0,13, 1, 2017);
}

void loop() {
  int i = 0;
  ans = digitalRead(pir_pin);   // 赤外線センサの値を読み込む
  sensing();
  float acceleration = total_acceleration();

  if(sound > 10 || acceleration > 30 || abs(old_lux - lux.light) >= 25 || abs(old_temp_act - temp_act) > 0.5){   // 観測出力の条件
    sensing_start_print();   // 時刻の表示
    sensing_print(old_temp_act, old_press_act, old_hum_act, old_acceleration, old_lux, old_sound, ans);
    for(int i=0; i < 5; i++){
      sensing_print(temp_act, press_act, hum_act, acceleration, lux.light, sound, ans);
      delay(300);
      sensing();
      acceleration = total_acceleration();
      // if(i > 2 && digitalRead(pir_pin) == 0){   // breakタイミングの設定
      //   break;
      // }
    }
    Serial.println("");
  }

  old_acceleration = acceleration;
  old_lux = lux.light;
  old_sound = sound;

  if(i % 1200 == 0){
    old_temp_act = temp_act;
    old_press_act = press_act;
    old_hum_act = hum_act;
  }
  i++;

  delay(500);
}