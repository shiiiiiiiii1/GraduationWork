// 温湿度気圧センサー
SSCI_BME280 bme280;
uint8_t temp_press_hum_i2c_addr = 0x76;   // 温湿度気圧センサ I2C Address
double temp_act, press_act, hum_act;   // 温湿度気圧センサ変数宣言
double old_temp_act, old_press_act, old_hum_act;
// 6軸加速度センサー
MPU6050 accelgyro;
int16_t ax, ay, az;
int16_t gx, gy, gz;
const float factor = 0.99;
float ax_low_filter_old = 0.0;
float ay_low_filter_old = 0.0;
float az_low_filter_old = 0.0;
float old_acceleration;
// 照度センサー
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);
sensors_event_t lux;
float old_lux;
// 音センサー
const int mic_analogpin = 3;
int sound;
int old_sound;
// 赤外線センサー
const int pir_pin = 13;
bool ans;
// 10分ごとに温湿度気圧センサーを観測
int count = 0;