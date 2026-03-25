/*
Пример кода, где массив в двоичном виде используется в цикле. Обратите внимание, как выглядит
второй цикл - в нем, в функции shiftOut() четвертый аргумент это разность тройки и i-го элемента.
Изменяя этот аргумент можно добиться более быстрого перемещения бегущего огня.
*/

int dataPin = 8;
int clockPin = 10;
int latchPin = 9;
byte path[4] = {
  0b11000011,
  0b00111100,
  0b00100100,
  0b00011000
};

void setup() {
 pinMode(latchPin, OUTPUT);
 pinMode(clockPin, OUTPUT);
 pinMode(dataPin, OUTPUT);
}

void loop() {
 for (int i = 0; i < 4; i++) {
 digitalWrite(latchPin, LOW);
 shiftOut(dataPin, clockPin, LSBFIRST, path[i]);
 digitalWrite(latchPin, HIGH);
 delay(250);
 }

 for (int i = 0; i < 4; i++) {
 digitalWrite(latchPin, LOW);
 shiftOut(dataPin, clockPin, LSBFIRST, path[3 - i]);
 digitalWrite(latchPin, HIGH);
 delay(250);
 }
}