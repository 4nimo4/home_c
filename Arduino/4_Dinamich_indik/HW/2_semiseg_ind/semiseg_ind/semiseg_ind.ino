
const int dataPin  = 8;
const int latchPin = 9;
const int clockPin = 10;

const int digit1Pin = 2;  // катод DIG1
const int digit2Pin = 3;  // катод DIG2


// Шаблоны цифр 0–9 для общекатодного индикатора
// порядок битов: DP G F E D C B A (пример, подстрой под свою распиновку)
const byte digitPatterns[10] = {
  //  DP G F E D C B A
  0b00111111, // 0
  0b00000110, // 1
  0b01011011, // 2
  0b01001111, // 3
  0b01100110, // 4
  0b01101101, // 5
  0b01111101, // 6
  0b00000111, // 7
  0b01111111, // 8
  0b01101111  // 9
};

int number = 0;                 // текущее число 0..99
unsigned long lastUpdate = 0;   // время последнего увеличения числа
const unsigned long STEP_MS = 100; // период счета (500 мс между числами)

void sendByte(byte value) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, value);
  digitalWrite(latchPin, HIGH);
}

void setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  pinMode(digit1Pin, OUTPUT);
  pinMode(digit2Pin, OUTPUT);

  // оба разряда погашены (для общекатодного — HIGH = выкл)
  digitalWrite(digit1Pin, HIGH);
  digitalWrite(digit2Pin, HIGH);

  sendByte(0x00);
}

void loop() {
  // ---------- 1. ОБНОВЛЯЕМ ЧИСЛО ПО ТАЙМЕРУ ----------
  unsigned long now = millis();
  if (now - lastUpdate >= STEP_MS) {   // прошло STEP_MS мс
    lastUpdate = now;
    number++;
    if (number > 99) number = 0;       // зацикливаем 0..99
  }

  // ---------- 2. ДИНАМИЧЕСКАЯ ИНДИКАЦИЯ ----------
  int tens = (number / 10) % 10;
  int ones = number % 10;

  // 1-й разряд (десятки)
  sendByte(digitPatterns[tens]);
  digitalWrite(digit1Pin, LOW);   // включить DIG1
  delay(3);                       // маленькая задержка для индикации
  digitalWrite(digit1Pin, HIGH);  // выключить DIG1

  // 2-й разряд (единицы)
  sendByte(digitPatterns[ones]);
  digitalWrite(digit2Pin, LOW);   // включить DIG2
  delay(3);
  digitalWrite(digit2Pin, HIGH);  // выключить DIG2

  // loop() сразу повторяется → мерцания нет, а число меняется раз в STEP_MS
}