void sensing() {
  bme280.readData(&temp_act, &press_act, &hum_act);   // 温湿度気圧センサの値を読み込む
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);   // 6軸加速度センサの値を読み込む
  tsl.getEvent(&lux);   // 照度センサの値を読み込む
  sound = analogRead(mic_analogpin);   // 音センサの値を読み込む
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
    total_acceleration += (ax_high_filter * ax_high_filter + ay_high_filter * ay_high_filter + az_high_filter * az_high_filter) / 20;
    ax_low_filter_old = ax_low_filter;
    ay_low_filter_old = ay_low_filter;
    az_low_filter_old = az_low_filter;
  }
  return total_acceleration;
}

bool tweetMsg(String msg) {
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    // Serial.println("connection failed");
    return false;
  }

  client.println("POST /api/post/ HTTP/1.0");

  client.print("Host: ");
  client.println(host);

  //メッセージ以外の長さ（トークン含む）にメッセージ長を加算
  int msgLength = 40;
  msgLength += msg.length();
  client.print("Content-length:");
  client.println(msgLength);
  client.println("");

  client.print("_t=");
  client.print(token);
  client.print("&msg=");
  client.println(msg);

  delay(10);

  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  return true;
}

/* センシングの出力結果

   12/8 5:30:20秒の場合
>
> ------------------  12/8 5:30:20  ------------------
>

*/
void sensing_start_print(){
  time_t n = now();
  time_t t;

  char s[20];
  const char* format = "%04d-%02d-%02d %02d:%02d:%02d";

  // JST
  t = localtime(n, 9);
  sprintf(s, format, year(t), month(t), day(t), hour(t), minute(t), second(t));
  Serial.print("JST : ");
  Serial.println(s);

  Serial.print("------------------  ");
  Serial.print(month(t)); Serial.print("/"); Serial.print(day(t)); Serial.print(" ");
  Serial.print(hour(t)); Serial.print(":"); Serial.print(minute(t)); Serial.print(":"); Serial.print(second(t));
  Serial.println("  ------------------");
}

void sensing_print(double temp_act, double press_act, double hum_act, float acceleration, float lux, int sound, bool ans) {
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
  Serial.print(" / sound : "); Serial.print(sound);
  Serial.print(" / ans : "); Serial.println(ans);
}