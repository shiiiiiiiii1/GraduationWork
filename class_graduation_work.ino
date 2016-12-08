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
const float factor = 0.99;
float ax_low_filter_old = 0.0;
float ay_low_filter_old = 0.0;
float az_low_filter_old = 0.0;

Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);   // 照度センサ
sensors_event_t lux;

const int mic_analogpin = 3;   // 音センサのピン
int sound;

const int pir_pin = 13;   // 焦電センサのピン
int ans;


double old_temp_act, old_press_act, old_hum_act;
float old_acceleration, old_lux;
int old_sound;

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
  setTime(2, 17, 0, 8, 12, 2016);
}

void loop() {
  ans = digitalRead(pir_pin);   // 赤外線センサの値を読み込む
  sensing();
  float acceleration = 0.0;
  acceleration = total_acceleration();

  if(ans == 1){
    sensing_start_print();   // 時刻の表示
    sensing_print(old_temp_act, old_press_act, old_hum_act, old_acceleration, old_lux, old_sound);
    for(int i=0; i < 1000; i++){
      sensing_print(temp_act, press_act, hum_act, acceleration, lux.light, sound);
      delay(300);
      sensing();
      acceleration = total_acceleration();
      if(i > 3 && digitalRead(pir_pin) == 0){
        break;
      }
    }
    Serial.println("");
  }

  old_temp_act = temp_act;
  old_press_act = press_act;
  old_hum_act = hum_act;
  old_acceleration = acceleration;
  old_lux = lux.light;
  old_sound = sound;
  delay(500);
}



float total_acceleration() {
  float total_acceleration = 0.0;
  for(int i=0; i<20; i++){
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    float ax_low_filter = factor * ax_low_filter_old + (1-factor) * (float)ax;
    float ax_high_filter = factor * (ax_high_filter + ax_low_filter - ax_low_filter_old);
    float ay_low_filter = factor * ay_low_filter_old + (1-factor) * (float)ay;
    float ay_high_filter = factor * (ay_high_filter + ay_low_filter - ay_low_filter_old);
    float az_low_filter = factor * az_low_filter_old + (1-factor) * (float)az;
    float az_high_filter = factor * (az_high_filter + az_low_filter - az_low_filter_old);
    total_acceleration += (ax_high_filter * ax_high_filter + ay_high_filter * ay_high_filter + az_high_filter * az_high_filter)/20;
    ax_low_filter_old = ax_low_filter;
    ay_low_filter_old = ay_low_filter;
    az_low_filter_old = az_low_filter;
  }
  return total_acceleration;
}


/* センシングの出力結果

   12/8 5:30:20秒の場合
>
> ------------------  12/8 5:30:20  ------------------
>

*/
void sensing_start_print() {
  Serial.print("------------------  ");
  Serial.print(month()); Serial.print("/"); Serial.print(day()); Serial.print(" ");
  Serial.print(hour()); Serial.print(":"); Serial.print(minute()); Serial.print(":"); Serial.print(second());
  Serial.println("  ------------------");
}

void sensing() {
  bme280.readData(&temp_act, &press_act, &hum_act);   // 温湿度気圧センサの値を読み込む
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);   // 6軸加速度センサの値を読み込む
  tsl.getEvent(&lux);   // 照度センサの値を読み込む
  sound = analogRead(mic_analogpin);   // 音センサの値を読み込む
}

void sensing_print(double temp_act, double press_act, double hum_act, float acceleration, float lux, int sound) {
  Serial.print("Temperature : "); Serial.print(temp_act);
  Serial.print("DegC / Pressure : "); Serial.print(press_act);
  Serial.print("hPa / Humidity : "); Serial.print(hum_act);
  Serial.print("% / Acceleration : "); Serial.print(acceleration);
  Serial.print("g / LUX : ");
  if (lux) {
    Serial.print(lux); Serial.print("lux");
  } else {
    Serial.print("'Sensor overload'");
  }
  Serial.print(" / sound : "); Serial.println(sound);
}