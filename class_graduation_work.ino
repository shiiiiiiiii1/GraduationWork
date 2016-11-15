#include <Time.h>   // 時間のセット
#include <TimeLib.h>   // 時間のセット
#include <Wire.h>   // I2C
#include <SSCI_BME280.h>   // 温湿度気圧センサ
#include <I2Cdev.h>   // 6軸加速度センサ
#include <MPU6050.h>   // 6軸加速度センサ
#include <Adafruit_Sensor.h>   // 照度センサ
#include <Adafruit_TSL2561_U.h>   // 照度センサ


SSCI_BME280 bme280;
uint8_t temp_press_hum_i2c_addr = 0x76;   // 温湿度気圧センサ I2C Address
double temp_act, press_act, hum_act;   // 温湿度気圧センサ変数宣言

MPU6050 accelgyro;   // 6軸加速度センサ
int16_t ax, ay, az;
int16_t gx, gy, gz;

Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);   // 照度センサ
sensors_event_t lux;

int mic_pin = 3;   // 音センサのピン
int microphone;

int pir_pin = 13;   // 焦電センサのピン
int ans;

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

// ------------------------------- 現在の時刻を入れて実験開始 -------------------------------
  setTime(12, 38, 0, 25, 10, 2016);
}

void loop() {
  bme280.readData(&temp_act, &press_act, &hum_act);   // 温湿度気圧センサの値を読み込む
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);   // 6軸加速度センサの値を読み込む
  tsl.getEvent(&lux);   // 照度センサの値を読み込む
  microphone = analogRead(mic_pin);   // 音センサの値を読み込む
  microphone = map(microphone, 0, 1023, -511, 512);
  ans = digitalRead(pir_pin);   // 赤外線センサの値を読み込む
  // String pir = ans==0 ? "LOW" : "HIGH";

  if(ans == 1){
    sensing_output();
  }
  delay(100);
}


void sensing_output() {
  Serial.print(year()); Serial.print("/"); Serial.print(month()); Serial.print("/"); Serial.print(day()); Serial.print(" ");
  Serial.print(hour()); Serial.print(":"); Serial.print(minute()); Serial.print(":"); Serial.print(second());
  Serial.print("  Temperature : "); Serial.print(temp_act);
  Serial.print("DegC / Pressure : "); Serial.print(press_act);
  Serial.print("hPa / Humidity : "); Serial.print(hum_act);
  Serial.print("% / Acceleration_X : "); Serial.print(ax);
  Serial.print("g / Acceleration_Y : "); Serial.print(ay);
  Serial.print("g / Acceleration_Z : "); Serial.print(az);
  Serial.print("g / LUX : ");
  if (lux.light) {
    Serial.print(lux.light); Serial.print("lux");
  } else {
    Serial.print("'Sensor overload'");
  }
  Serial.print(" / mic : "); Serial.print(microphone);
  Serial.print(" / parallax : "); Serial.println(ans);
}