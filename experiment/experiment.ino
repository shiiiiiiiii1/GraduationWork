#include <Arduino.h>

// ライブラリ
#include <ESP8266WiFi.h>   // WROOM
#include <NTP.h>   // Time
#include <Wire.h>   // I2C
#include <SSCI_BME280.h>   // 温湿度気圧
#include <I2Cdev.h>   // 6軸加速度
#include <MPU6050.h>   // 6軸加速度
#include <Adafruit_Sensor.h>   // 照度
#include <Adafruit_TSL2561_U.h>   // 照度
#include <Adafruit_ADS1015.h>   // analog-I2C
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

  Serial.begin(9600);
  WiFi.begin(ssid, password);
  Wire.begin(4, 14);   // Wire.begin(SDApin, SCLpin);

  bme280.setMode(temp_press_hum_i2c_addr, osrs_t, osrs_p, osrs_h, bme280mode, t_sb, filter, spi3w_en);
  bme280.readTrim();

  accelgyro.initialize();

  ads.begin();

  ntp_begin(2390);

  float acceleration;
  for(int i = 0; i < 50; i++){
    sensing();
    acceleration = total_acceleration();
    Serial.println(acceleration);
    delay(50);
  }
  old_acceleration = acceleration;
  old_lux = lux.light;
  old_sound = sound;
  old_temp_act = temp_act;
  old_press_act = press_act;
  old_hum_act = hum_act;
}

void loop() {
  sensing();
  float acceleration = total_acceleration();

  sensing_start_print();
  sensing_print(temp_act, press_act, hum_act, acceleration, lux.light, 0, ans);

  /*
    - 季節を見て状況判断？
    - 暖房つけた時と昼間普通に温度が上がる時の差がわかりにくい？
      - 温度の上がり具合の急激さで判断
  */
  if(temp_act - old_temp_act >= 1.0 && temp_act - old_temp_act <= 1.5){
    tweetMsg(tweetString("暖房つけた〜！"));
    Serial.println("暖房つけた");
    old_temp_act = temp_act;
  }
  if(old_temp_act - temp_act >= 1.0 && old_temp_act - temp_act <= 1.5){
    tweetMsg(tweetString("暖房消した〜！"));
    Serial.println("暖房消した");
   old_temp_act = temp_act;
  }

  if(old_lux >= 71.0 && lux.light <= 21){
    tweetMsg(tweetString("電気消した！"));
    Serial.println("電気消した");
  }
  if(old_lux <= 43.0 && lux.light >= 71.0){
    tweetMsg(tweetString("電気つけた！"));
    Serial.println("電気つけた");
  }
  // if(lux.light - old_lux => 250){ /* カーテン開けた */ }


  old_acceleration = acceleration;
  old_lux = lux.light;
  old_sound = sound;
  if(count % 1500 == 0){
    old_temp_act = temp_act;
    old_press_act = press_act;
    old_hum_act = hum_act;
    count = 0;
  }
  count++;

  delay(1000);
  // delay(60000);
}