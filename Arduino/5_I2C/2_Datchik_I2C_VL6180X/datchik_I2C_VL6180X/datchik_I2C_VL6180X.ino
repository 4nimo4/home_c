/*
Датчик I2C VL6180X
Пример с датчиком освещенности и приближения. Обратите внимание, что данный датчик требует
подключения дополнительной библиотеки. Используется схема с прошлого занятия, со сдвиговым
регистром.
*/
#include <Wire.h> //библиотека I2C
#include <SparkFun VL6180X.h> // библиотека для датчика
#define VL6180X_ADDRESS 0x29

VL6180x sensor(VL6180X_ADDRESS); // инициализация датчика
const int SER = 8;
const int LATCH = 9;
const int CLK = 10;
int vs[8] = {1,2,4,8,32,64,128,255};

void setup() {
  Serial.begin(9600);
  while (sensor.VL6180xInit()) {
    Serial.println("Failed to initalize");
    delay(1000);
  }

  sensor.VL6180xDefautSettings(); // загрузка настроек датчика по умолчанию
  delay(1000);
  pinMode(SER, OUTPUT);
  pinMode(LATCH, OUTPUT);
  pinMode(CLK, OUTPUT);
}

void loop() {
  int dist = sensor.getDistance();// метод для измерения расстояния
  dist = map(dist, 0, 200, 0, 8);
  dist = constrain(dist, 0, 8);
  digitalWrite(LATCH, LOW);
  shiftOut(SER, CLK, MSBFIRST, vs[dist]);
  digitalWrite(LATCH, HIGH);
  delay(10);
}