#include <Arduino.h>
#include <Wire.h>

const uint8_t MPU_ADDR = 0x68;

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }

  Wire.begin();
  Wire.setClock(100000);   // 100 кГц – как в основном проекте

  Serial.println("MPU bare test...");

  // Проверка связи
  Wire.beginTransmission(MPU_ADDR);
  uint8_t err = Wire.endTransmission();
  if (err != 0) {
    Serial.print("MPU not found, I2C error = ");
    Serial.println(err);
    return;
  }

  // WHO_AM_I
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x75);
  Wire.endTransmission(false);
  Wire.requestFrom((int)MPU_ADDR, 1);
  uint8_t who = Wire.read();
  Serial.print("WHO_AM_I = 0x");
  Serial.println(who, HEX);

  // Разбудим MPU
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);   // PWR_MGMT_1
  Wire.write(0x00);   // wake up
  Wire.endTransmission();
  delay(100);
}

void loop() {
  // Читаем 14 байт начиная с 0x3B
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  uint8_t err = Wire.endTransmission(false);
  if (err != 0) {
    Serial.print("endTransmission error = ");
    Serial.println(err);
    delay(500);
    return;
  }

  uint8_t rc = Wire.requestFrom((int)MPU_ADDR, 14);
  if (rc < 14) {
    Serial.print("short read, got ");
    Serial.println(rc);
    delay(500);
    return;
  }

  int16_t ax_raw = (int16_t)(Wire.read() << 8 | Wire.read());
  int16_t ay_raw = (int16_t)(Wire.read() << 8 | Wire.read());
  int16_t az_raw = (int16_t)(Wire.read() << 8 | Wire.read());
  int16_t t_raw  = (int16_t)(Wire.read() << 8 | Wire.read());
  int16_t gx_raw = (int16_t)(Wire.read() << 8 | Wire.read());
  int16_t gy_raw = (int16_t)(Wire.read() << 8 | Wire.read());
  int16_t gz_raw = (int16_t)(Wire.read() << 8 | Wire.read());

  Serial.print("ax="); Serial.print(ax_raw);
  Serial.print(" ay="); Serial.print(ay_raw);
  Serial.print(" az="); Serial.print(az_raw);
  Serial.print(" gx="); Serial.print(gx_raw);
  Serial.print(" gy="); Serial.print(gy_raw);
  Serial.print(" gz="); Serial.print(gz_raw);
  Serial.println();

  delay(200);
}