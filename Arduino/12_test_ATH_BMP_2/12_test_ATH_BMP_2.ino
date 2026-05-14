#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>

Adafruit_AHTX0   aht;
Adafruit_BMP280  bmp;

const int AHT_BMP_PWR_PIN   = 3;  // как в основном коде
const int SV1_INDIC_PWR_PIN = 4;

bool aht_ok = false;
bool bmp_ok = false;

void powerCycleEnv(const char* reason) {
  Serial.print("POWER CYCLE ENV, reason: ");
  Serial.println(reason);

  digitalWrite(AHT_BMP_PWR_PIN, HIGH);
  digitalWrite(SV1_INDIC_PWR_PIN, LOW);
  delay(400);

  digitalWrite(AHT_BMP_PWR_PIN, LOW);
  digitalWrite(SV1_INDIC_PWR_PIN, HIGH);
  delay(500);

  Serial.println("Calling aht.begin()...");
  aht_ok = aht.begin();
  Serial.print("aht.begin() -> ");
  Serial.println(aht_ok ? "OK" : "ERR");

  Serial.println("Calling bmp.begin()...");
  bmp_ok = bmp.begin(0x76) || bmp.begin(0x77);
  Serial.print("bmp.begin() -> ");
  Serial.println(bmp_ok ? "OK" : "ERR");
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {}

  Serial.println("=== ENV TEST START ===");
  Wire.begin();
  Wire.setClock(100000);

  pinMode(AHT_BMP_PWR_PIN,   OUTPUT);
  pinMode(SV1_INDIC_PWR_PIN, OUTPUT);

  // начальное питание
  digitalWrite(AHT_BMP_PWR_PIN, HIGH);
  digitalWrite(SV1_INDIC_PWR_PIN, LOW);
  delay(50);

  digitalWrite(AHT_BMP_PWR_PIN, LOW);
  digitalWrite(SV1_INDIC_PWR_PIN, HIGH);
  delay(500);

  powerCycleEnv("initial");
}

unsigned long lastPrint = 0;
unsigned long lastCycle = 0;

void loop() {
  unsigned long now = millis();

  // Каждые 3 секунды читаем датчики, если они "OK"
  if (now - lastPrint > 3000) {
    lastPrint = now;

    Serial.print("STATUS: AHT=");
    Serial.print(aht_ok ? "OK" : "ERR");
    Serial.print("  BMP=");
    Serial.println(bmp_ok ? "OK" : "ERR");

    if (aht_ok) {
      sensors_event_t hum, temp;
      if (aht.getEvent(&hum, &temp)) {
        Serial.print("  AHT: T=");
        Serial.print(temp.temperature, 1);
        Serial.print("C  H=");
        Serial.print(hum.relative_humidity, 1);
        Serial.println("%");
      } else {
        Serial.println("  AHT: getEvent FAILED");
        aht_ok = false;
      }
    }

    if (bmp_ok) {
      float pPa = bmp.readPressure();
      float tempBMP = bmp.readTemperature();
      Serial.print("  BMP: T=");
      Serial.print(tempBMP, 1);
      Serial.print("C  P=");
      Serial.print(pPa / 100.0f, 1);
      Serial.println(" hPa");
    }

    Serial.println("----");
  }

  // Каждые 15 секунд делаем полный power cycle ENV, чтобы проверить восстановление
  if (now - lastCycle > 15000) {
    lastCycle = now;
    powerCycleEnv("periodic test");
  }
}