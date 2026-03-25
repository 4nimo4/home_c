/*
Макрос 
Так же можно использовать довольно интересную функцию bitWrite(x, n, b) - где x - переменная, у
которой нужно изменить бит, n - номер бита, состояние которого необходимо изменить(начинается с
крайнего правого), b - новое значение бита(0 или 1).
Реализация бегущего огня с помощью bitWrite().
*/

int dataPin = 8;
int clockPin = 10;
int latchPin = 9;


void setup() {
 pinMode(latchPin, OUTPUT);
 pinMode(clockPin, OUTPUT);
 pinMode(dataPin, OUTPUT);
}

void loop() {
  byte byteToSend = 0; //Создаем пустой байт 0b00000000
  for (int bitPos = 0; bitPos < 8; bitPos++) { // В переменной хранится позиция изменяемого бита
  byteToSend = 0; // Обнуляем байт при каждом проходе
  bitWrite(byteToSend, bitPos, HIGH); // При bitPos=0 получим 0b00000001, 
                                      //при bitPos=1 - 0b00000010, при bitPos=2 - 0b00000100 и т.д.
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, byteToSend); // Инвертируем сигнал при помощи MSBFIRST, грузим с первого бит
  digitalWrite(latchPin, HIGH);
  delay(50);
  }
}