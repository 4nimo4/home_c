#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>

Adafruit_AHTX0 aht;
Adafruit_BMP280 bmp;

void setup() {
  Serial.begin(115200);
  while (!Serial) {}

  Wire.begin();
  Wire.setClock(100000);

  Serial.println("AHT/BMP bare test...");

  if (!aht.begin()) {
    Serial.println("AHT20 not found");
  } else {
    Serial.println("AHT20 OK");
  }

  if (!bmp.begin(0x76) && !bmp.begin(0x77)) {
    Serial.println("BMP280 not found");
  } else {
    Serial.println("BMP280 OK");
  }
}

void loop() {
  sensors_event_t hum, temp;
  if (aht.begin() && aht.getEvent(&hum, &temp)) {
    Serial.print("T="); Serial.print(temp.temperature);
    Serial.print(" H="); Serial.print(hum.relative_humidity);
  } else {
    Serial.print("AHT_ERR");
  }

  if (bmp.begin(0x76) || bmp.begin(0x77)) {
    float p = bmp.readPressure() / 100.0;
    Serial.print(" P="); Serial.print(p);
  } else {
    Serial.print(" BMP_ERR");
  }

  Serial.println();
  delay(1000);
}