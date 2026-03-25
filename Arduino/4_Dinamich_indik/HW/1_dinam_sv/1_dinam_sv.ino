
const int dataPin = 8;
const int latchPin = 9;
const int clockPin = 10;

// последовательность символов
int vs[14] = {1,2,4,8,16,32,64,128,64,32,16,8,4,2};

void fillRightThenBack(int);
void fillLeftThenForward(int);
void sendByte(byte value);

void setup() {
 pinMode(latchPin, OUTPUT);
 pinMode(clockPin, OUTPUT);
 pinMode(dataPin, OUTPUT);
 sendByte(0x00); // всё погашено
}
void loop() {
  fillRightThenBack(30);
  fillLeftThenForward(30);
}

// ---------- ВСПОМОГАТЕЛЬНАЯ ФУНКЦИЯ ВЫВОДА В 74HC595 ----------
void sendByte(byte value) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, value);
  digitalWrite(latchPin, HIGH);
}

// ---------- ЭФФЕКТ: вправо с заполнением назад ----------
void fillRightThenBack(int delayMs) {
  for (int start = 0; start < 8; start++) {
    byte mask = 0;

    // идём от start до 7, накапливая биты
    for (int pos = start; pos < 8; pos++) {
      bitWrite(mask, pos, HIGH);
      sendByte(mask);
      delay(delayMs);
    }

    // идём назад от 7 до 0, тоже накапливая
    for (int pos = 7; pos >= 0; pos--) {
      bitWrite(mask, pos, HIGH);
      sendByte(mask);
      delay(delayMs);
    }

    // погасить всё перед следующим циклом
    sendByte(0x00);
  }
}

// ---------- ЭФФЕКТ: влево с заполнением вперёд ----------
void fillLeftThenForward(int delayMs) {
  for (int start = 7; start >= 0; start--) {
    byte mask = 0;

    for (int pos = start; pos >= 0; pos--) {
      bitWrite(mask, pos, HIGH);
      sendByte(mask);
      delay(delayMs);
    }

    for (int pos = 0; pos < 8; pos++) {
      bitWrite(mask, pos, HIGH);
      sendByte(mask);
      delay(delayMs);
    }

    sendByte(0x00);
  }
}