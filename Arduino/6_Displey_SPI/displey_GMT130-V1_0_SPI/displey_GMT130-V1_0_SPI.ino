#include <Wire.h>        // Подключаем библиотеку Wire для работы по шине I2C (MPU6050, AHT20, BMP280)
#include <SPI.h>         // Подключаем библиотеку SPI для работы по шине SPI (дисплей ST7789)

#include <Adafruit_GFX.h>       // Графическая библиотека Adafruit GFX (текст, линии, прямоугольники и т.п.)
#include <Adafruit_ST7789.h>    // Драйвер дисплея ST7789 для работы через Adafruit GFX
#include <Adafruit_AHTX0.h>     // Драйвер датчика AHT10/AHT20 (температура + влажность)
#include <Adafruit_BMP280.h>    // Драйвер датчика BMP280 (давление + температура)

// ---------- TFT ST7789 (GMT130‑V1.0, 240x240) ----------
#define TFT_CS   10      // Логический пин CS для библиотеки (физически к модулю не подключён)
#define TFT_DC    9      // Пин Arduino, подключённый к выводу DC (D/C) дисплея
#define TFT_RST   8      // Пин Arduino, подключённый к выводу RESET дисплея

Adafruit_ST7789 tft(TFT_CS, TFT_DC, TFT_RST);  // Создаём объект дисплея ST7789: CS, DC, RST

// ---------- MPU6050 ----------
const uint8_t MPU_ADDR = 0x68;     // Адрес MPU6050 на шине I2C (AD0 соединён с GND)
bool mpu_ok = false;               // Флаг: успешно ли инициализирован MPU6050

float ax_offset = 0, ay_offset = 0, az_offset = 0;  // Смещения для акселерометра по X, Y, Z
float gx_offset = 0, gy_offset = 0, gz_offset = 0;  // Смещения для гироскопа по X, Y, Z
float temp_offset = 0;                              // Поправка для измерений температуры (пока 0)

float ax_f = 0, ay_f = 0, az_f = 0;   // Сглаженные ускорения по X/Y/Z в g
float gx_f = 0, gy_f = 0, gz_f = 0;   // Сглаженные угловые скорости по X/Y/Z в град/с
float temp_f = 0;                     // Сглаженное значение температуры MPU, °C
const float alpha = 0.1f;             // Коэффициент сглаживания (0..1), меньше — плавнее изменения

// ---------- AHT20 + BMP280 ----------
Adafruit_AHTX0 aht;            // Объект датчика AHT20 (температура и влажность)
Adafruit_BMP280 bmp;           // Объект датчика BMP280 (давление и температура)

bool aht_ok = false;           // true, если AHT20 успешно инициализирован
bool bmp_ok = false;           // true, если BMP280 успешно инициализирован
bool poterya_svyazi = false;   // true, если во время работы была потеря связи по I2C с AHT/BMP
unsigned long lastAhtRetry = 0;                 // Время последней попытки переинициализации AHT/BMP
const unsigned long AHT_RETRY_INTERVAL = 5000;  // Интервал между попытками восстановить датчики (мс)

float aht_tC = 0;      // Температура воздуха с AHT20, °C
float aht_hR = 0;      // Относительная влажность с AHT20, %
float p_mmHg = 0;      // Давление с BMP280, мм рт. ст.

// ---------- режимы отображения ----------
int displayMode = 0;                    // Текущий режим: 0 — показываем MPU, 1 — AHT/BMP (ENV)
unsigned long lastScreenUpdate = 0;     // Время последнего обновления текста на экране
const unsigned long SCREEN_UPDATE_INTERVAL = 500;  // Интервал обновления значений на экране, мс

// ---------- кнопка переключения ----------
const int buttonPin = 2;              // Пин D2, к нему подключена кнопка (второй вывод кнопки к GND)
bool lastButtonState = HIGH;          // Предыдущее состояние кнопки (HIGH = не нажата при INPUT_PULLUP)
unsigned long lastButtonChange = 0;   // Время последнего изменения состояния кнопки
const unsigned long DEBOUNCE_MS = 50; // Время антидребезга, мс

// ---------- MPU: функции ----------
void mpuWrite(uint8_t reg, uint8_t data) {
  Wire.beginTransmission(MPU_ADDR);  // Начинаем передачу к адресу MPU_ADDR
  Wire.write(reg);                   // Отправляем номер регистра
  Wire.write(data);                  // Отправляем значение
  Wire.endTransmission();            // Завершаем передачу
}

void mpuRead(uint8_t reg, uint8_t *buf, uint8_t len) {
  Wire.beginTransmission(MPU_ADDR);  // Начинаем передачу к MPU
  Wire.write(reg);                   // Записываем стартовый регистр для чтения
  Wire.endTransmission(false);       // Завершаем с «повторным стартом» (не отпуская шину)
  Wire.requestFrom((int)MPU_ADDR, (int)len); // Запрашиваем len байт у устройства
  for (uint8_t i = 0; i < len && Wire.available(); i++) {
    buf[i] = Wire.read();           // Читаем каждый доступный байт в буфер
  }
}

void calibrateMPU(unsigned int samples = 200) {
  Serial.println("Calibrating MPU... Keep sensor still.");  // Сообщение в Serial

  long ax_sum = 0, ay_sum = 0, az_sum = 0;   // Суммы измерений акселя
  long gx_sum = 0, gy_sum = 0, gz_sum = 0;   // Суммы измерений гироскопа
  long temp_sum = 0;                         // Сумма значений температуры (пока не используем)

  for (unsigned int i = 0; i < samples; i++) {  // Цикл по количеству выборок
    uint8_t data[14];                           // Буфер на 14 байт (AX,AY,AZ,T,GX,GY,GZ)
    mpuRead(0x3B, data, 14);                    // Читаем блок регистров, начиная с ACCEL_XOUT_H (0x3B)

    // Склеиваем два байта (старший+младший) в 16‑битное значение для каждой величины
    int16_t ax_raw  = (int16_t)(data[0] << 8 | data[1]);
    int16_t ay_raw  = (int16_t)(data[2] << 8 | data[3]);
    int16_t az_raw  = (int16_t)(data[4] << 8 | data[5]);
    int16_t tempRaw = (int16_t)(data[6] << 8 | data[7]);
    int16_t gx_raw  = (int16_t)(data[8] << 8 | data[9]);
    int16_t gy_raw  = (int16_t)(data[10] << 8 | data[11]);
    int16_t gz_raw  = (int16_t)(data[12] << 8 | data[13]);

    // Накопление сумм для последующего усреднения
    ax_sum += ax_raw;
    ay_sum += ay_raw;
    az_sum += az_raw;
    gx_sum += gx_raw;
    gy_sum += gy_raw;
    gz_sum += gz_raw;
    temp_sum += tempRaw;

    delay(5);   // Короткая пауза между выборками (5 мс)
  }

  // Средние (усреднённые) сырые значения
  float ax_avg = (float)ax_sum / samples;
  float ay_avg = (float)ay_sum / samples;
  float az_avg = (float)az_sum / samples;
  float gx_avg = (float)gx_sum / samples;
  float gy_avg = (float)gy_sum / samples;
  float gz_avg = (float)gz_sum / samples;
  (void)temp_sum;  // temp_sum пока не используем (чтобы не было warning о неиспользованной переменной)

  // Задаём смещения (офсеты).
  // Для акселя хотим: X≈0g, Y≈0g, Z≈+1g (16384 сырьевых единиц при ±2g)
  ax_offset = ax_avg;             // Оффсет по X: вычтем его из всех дальнейших измерений
  ay_offset = ay_avg;             // Оффсет по Y
  az_offset = az_avg - 16384;     // Оффсет по Z так, чтобы в покое было около 1g

  // Для гироскопа хотим, чтобы в покое было ≈0°/с по всем осям
  gx_offset = gx_avg;
  gy_offset = gy_avg;
  gz_offset = gz_avg;

  temp_offset = 0.0f;             // Поправку по температуре пока не используем

  Serial.println("MPU calibration done.");  // Сообщение об окончании калибровки
}

// ---------- TFT: сообщения ----------
void showMessageTFT(const char *text,
                    uint8_t size = 3,
                    uint16_t color = ST77XX_GREEN) {
  tft.fillScreen(ST77XX_BLACK);         // Заливаем экран чёрным
  tft.setTextWrap(false);               // Отключаем автоматический перенос текста
  tft.setTextColor(color, ST77XX_BLACK);// Цвет текста, фон – чёрный
  tft.setTextSize(size);                // Устанавливаем масштаб шрифта

  int16_t x1, y1;       // здесь библиотека вернёт смещение «якоря»
  uint16_t w, h;        // ширина и высота текста в пикселях
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);  // Вычисляем размеры текста
  int16_t x = (tft.width()  - w) / 2;               // Координата X для центрирования
  int16_t y = (tft.height() - h) / 2;               // Координата Y для центрирования

  tft.setCursor(x, y);  // Устанавливаем курсор на рассчитанную позицию
  tft.print(text);      // Печатаем строку
}

// ---------- TFT: экран MPU ----------
void drawMPUFrame() {
  tft.fillScreen(ST77XX_BLACK);             // Очищаем экран (чёрный фон)
  tft.setTextWrap(false);
  tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);  // Подписи зелёным
  tft.setTextSize(3);                             // Шрифт среднего размера
  int clmn1=10;
  tft.setCursor(clmn1, 10);
  tft.print("MPU-6050");   // Заголовок экрана
  tft.setTextSize(2.5);

  tft.setCursor(clmn1, 40);  tft.print("AX:"); // Подпись ускорения X
  tft.setCursor(clmn1, 60);  tft.print("AY:"); // Подпись ускорения Y
  tft.setCursor(clmn1, 80);  tft.print("AZ:"); // Подпись ускорения Z
  tft.setCursor(clmn1, 110); tft.print("GX:"); // Подпись угл. скорости X
  tft.setCursor(clmn1, 130); tft.print("GY:"); // Подпись угл. скорости Y
  tft.setCursor(clmn1, 150); tft.print("GZ:"); // Подпись угл. скорости Z
  tft.setCursor(clmn1, 180); tft.print("T-MPU:");  // Подпись температуры
}

void updateMPUValues() {
  tft.setTextWrap(false);
  tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK); // Цифры – белым
  tft.setTextSize(2);
  int clmn2=130;
  tft.setCursor(clmn2, 40); tft.print("        "); tft.setCursor(clmn2, 40); tft.print(ax_f, 2);// AX
  tft.setCursor(clmn2, 60); tft.print("        "); tft.setCursor(clmn2, 60); tft.print(ay_f, 2);// AY
  tft.setCursor(clmn2, 80); tft.print("        "); tft.setCursor(clmn2, 80); tft.print(az_f, 2);// AZ

  tft.setCursor(clmn2, 110); tft.print("        "); tft.setCursor(clmn2, 110); tft.print(gx_f, 1);// GX
  tft.setCursor(clmn2, 130); tft.print("        "); tft.setCursor(clmn2, 130); tft.print(gy_f, 1);// GY
  tft.setCursor(clmn2, 150); tft.print("        "); tft.setCursor(clmn2, 150); tft.print(gz_f, 1);// GZ

  tft.setCursor(clmn2, 180); tft.print("        "); tft.setCursor(clmn2, 180); tft.print(temp_f, 1);// T
}

// ---------- TFT: экран ENV (AHT/BMP) ----------
void drawEnvFrame() {
  tft.fillScreen(ST77XX_BLACK);                 // Очищаем экран
  tft.setTextWrap(false);
  tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK); // Подписи зелёным
  tft.setTextSize(3); 

  int clmn3=10;
  tft.setCursor(clmn3, 10);                          // Крупный шрифт
  tft.print("AHT20/BMP280");   // Заголовок экрана

  tft.setCursor(clmn3, 10);  tft.print(""); // Заголовок экрана (environment)
  tft.setCursor(clmn3, 50);  tft.print("Temp:");  // Подпись температуры
  tft.setCursor(clmn3, 100); tft.print("Hyd:");  // Подпись влажности
  tft.setCursor(clmn3, 150); tft.print("Psc:");  // Подпись давления
}

void updateEnvValues() {
  tft.setTextWrap(false);
  tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK); // Значения – белым
  tft.setTextSize(3);
  int clmn4=130;

  tft.setCursor(clmn4, 50);        // Позиция значения температуры
  tft.print("        ");        // Стираем старый текст
  tft.setCursor(clmn4, 50);
  if (aht_ok) {                 // Если AHT20 работает
    tft.print(aht_tC, 1);       // Печатаем температуру с 1 знаком после запятой
    tft.print("C");
  } else {
    tft.print("ERR");           // Иначе пишем ERR
  }

  tft.setCursor(clmn4, 100);
  tft.print("        ");
  tft.setCursor(clmn4, 100);
  if (aht_ok) {
    tft.print(aht_hR, 0);       // Печатаем влажность без десятых
    tft.print("%");
  } else {
    tft.print("ERR");
  }

  tft.setCursor(clmn4, 150);
  tft.print("        ");
  tft.setCursor(clmn4, 150);
  if (bmp_ok) {
    tft.print(p_mmHg, 0);       // Давление в мм рт.ст. без десятых
  } else {
    tft.print("ERR");
  }
}


// ---------- setup ----------
void setup() {
  Serial.begin(115200);              // Запускаем последовательный порт на 115200 бод
  while (!Serial) { delay(10); }     // Ждём готовности Serial (для некоторых плат)

  Wire.begin();                      // Запуск шины I2C

  pinMode(buttonPin, INPUT_PULLUP);  // Настраиваем пин кнопки D2 с внутренней подтяжкой к +V

  // TFT
  tft.init(240, 240, SPI_MODE2);     // Инициализируем ST7789: размер 240x240, режим SPI_MODE2 (важно для GMT130)
  tft.setRotation(2);                // Поворот изображения (подбирается по модулю)
  tft.fillScreen(ST77XX_BLACK);      // Заливаем экран чёрным
  tft.setTextWrap(false);            // Отключаем перенос текста

  showMessageTFT("INIT SENS", 3, ST77XX_GREEN);  // Краткое сообщение «инициализация датчиков»
  delay(500);                                   // Пауза для отображения

  // MPU6050
  Wire.beginTransmission(MPU_ADDR);
  uint8_t err = Wire.endTransmission();
  if (err == 0) {
    uint8_t whoami = 0;
    mpuRead(0x75, &whoami, 1);
    Serial.print("MPU WHO_AM_I = 0x");
    Serial.println(whoami, HEX);

    mpuWrite(0x6B, 0x00);
    delay(100);
    mpuWrite(0x1C, 0x00);
    mpuWrite(0x1B, 0x00);

    mpu_ok = true;
    Serial.println("MPU initialized.");
    showMessageTFT("MPU OK", 3, ST77XX_GREEN);
    delay(500);

    calibrateMPU(200);

    uint8_t data[14];
    mpuRead(0x3B, data, 14);
    int16_t ax_raw  = (int16_t)(data[0] << 8 | data[1]);
    int16_t ay_raw  = (int16_t)(data[2] << 8 | data[3]);
    int16_t az_raw  = (int16_t)(data[4] << 8 | data[5]);
    int16_t tempRaw = (int16_t)(data[6] << 8 | data[7]);
    int16_t gx_raw  = (int16_t)(data[8] << 8 | data[9]);
    int16_t gy_raw  = (int16_t)(data[10] << 8 | data[11]);
    int16_t gz_raw  = (int16_t)(data[12] << 8 | data[13]);

    float ax = (ax_raw - ax_offset) / 16384.0f;
    float ay = (ay_raw - ay_offset) / 16384.0f;
    float az = (az_raw - az_offset) / 16384.0f;
    float gx = (gx_raw - gx_offset) / 131.0f;
    float gy = (gy_raw - gy_offset) / 131.0f;
    float gz = (gz_raw - gz_offset) / 131.0f;
    float tempC = (tempRaw / 340.0f) + 36.53f + temp_offset;

    ax_f = ax; ay_f = ay; az_f = az;
    gx_f = gx; gy_f = gy; gz_f = gz;
    temp_f = tempC;
  } else {
    mpu_ok = false;
    Serial.println("MPU not found at 0x68");
    showMessageTFT("MPU ERR", 3, ST77XX_RED);
    delay(500);
  }

  // AHT20
  if (!aht.begin()) {                 // Пытаемся инициализировать AHT20
    aht_ok = false;
    Serial.println("AHT20 not found");
    showMessageTFT("AHT20 ERR", 3, ST77XX_RED);  // Выводим ошибку на экран
    delay(500);
  } else {
    aht_ok = true;
    Serial.println("AHT20 OK");
    showMessageTFT("AHT20 OK", 3, ST77XX_GREEN); // Выводим «OK» на экран
    delay(500);
  }

  // BMP280
  if (!bmp.begin(0x76) && !bmp.begin(0x77)) {
    bmp_ok = false;
    Serial.println("BMP280 not found");
    showMessageTFT("BMP280 ERR", 3, ST77XX_RED);
    delay(500);
  } else {
    bmp_ok = true;
    Serial.println("BMP280 OK");
    showMessageTFT("BMP280 OK", 3, ST77XX_GREEN);
    delay(500);

    bmp.setSampling(
      Adafruit_BMP280::MODE_NORMAL,
      Adafruit_BMP280::SAMPLING_X2,
      Adafruit_BMP280::SAMPLING_X16,
      Adafruit_BMP280::FILTER_X16,
      Adafruit_BMP280::STANDBY_MS_500
    );
  }

   // Стартуем с MPU
  displayMode = 0;        // Начинаем с режима MPU
  drawMPUFrame();         // Рисуем каркас экрана MPU

  lastScreenUpdate = millis();  // Запоминаем время последнего обновления экрана
}

// ---------- loop ----------
void loop() {
  unsigned long now = millis();   // Текущее время в мс с момента старта

  // MPU
  if (mpu_ok) {
    uint8_t data[14];
    mpuRead(0x3B, data, 14);

    int16_t ax_raw  = (int16_t)(data[0] << 8 | data[1]);
    int16_t ay_raw  = (int16_t)(data[2] << 8 | data[3]);
    int16_t az_raw  = (int16_t)(data[4] << 8 | data[5]);
    int16_t tempRaw = (int16_t)(data[6] << 8 | data[7]);
    int16_t gx_raw  = (int16_t)(data[8] << 8 | data[9]);
    int16_t gy_raw  = (int16_t)(data[10] << 8 | data[11]);
    int16_t gz_raw  = (int16_t)(data[12] << 8 | data[13]);

    float ax = (ax_raw - ax_offset) / 16384.0f;
    float ay = (ay_raw - ay_offset) / 16384.0f;
    float az = (az_raw - az_offset) / 16384.0f;

    float gx = (gx_raw - gx_offset) / 131.0f;
    float gy = (gy_raw - gy_offset) / 131.0f;
    float gz = (gz_raw - gz_offset) / 131.0f;

    float tempC = (tempRaw / 340.0f) + 36.53f + temp_offset;

    ax_f = ax_f + alpha * (ax - ax_f);
    ay_f = ay_f + alpha * (ay - ay_f);
    az_f = az_f + alpha * (az - az_f);

    gx_f = gx_f + alpha * (gx - gx_f);
    gy_f = gy_f + alpha * (gy - gy_f);
    gz_f = gz_f + alpha * (gz - gz_f);

    temp_f = temp_f + alpha * (tempC - temp_f);
  }

  // AHT/BMP
  if (aht_ok) {                         // Если AHT20 инициализирован
    sensors_event_t humidity, temp;     // Структуры для приёма данных
    if (aht.getEvent(&humidity, &temp)) { // Успешно получили новые показания
      aht_tC = temp.temperature;           // Температура воздуха
      aht_hR = humidity.relative_humidity; // Влажность

      if (bmp_ok) {                        // Если BMP280 тоже работает
        float pPa  = bmp.readPressure();   // Давление в Паскалях
        float p_hPa = pPa / 100.0;         // Перевод в гектопаскали
        p_mmHg = p_hPa * 0.75006;          // Перевод в мм рт.ст.
      }
    } else if (!poterya_svyazi) {          // Если чтение не удалось и это первая ошибка
      aht_ok = false;                      // Считаем, что AHT не работает
      bmp_ok = false;                      // И BMP тоже
      poterya_svyazi = true;               // Отмечаем факт потери связи
      lastAhtRetry = now;                  // Запоминаем время, когда произошла ошибка
    }
  }

  if (poterya_svyazi && (now - lastAhtRetry > AHT_RETRY_INTERVAL)) {
    Serial.println("Trying to re-init AHT20 & BMP280...");
    if (aht.begin() && (bmp.begin(0x76) || bmp.begin(0x77))) {
      aht_ok = true;
      bmp_ok = true;
      poterya_svyazi = false;
      Serial.println("AHT20 & BMP280 re-init OK");
    } else {
      Serial.println("Still no AHT/BMP");
    }
  }

  // Кнопка переключения режимов
  bool buttonState = digitalRead(buttonPin);  // Считываем текущее состояние кнопки (HIGH/LOW)
  if (buttonState != lastButtonState &&       // Если состояние изменилось
      (now - lastButtonChange) > DEBOUNCE_MS) { // и прошло время антидребезга

     lastButtonChange = now;       // Запоминаем время изменения
    lastButtonState = buttonState;// Обновляем предыдущее состояние

    if (buttonState == LOW) {     // Фиксируем нажатие (с HIGH на LOW)
      displayMode = (displayMode + 1) % 2; // Переключаем режим 0 -> 1 -> 0 -> ...

      if (displayMode == 0) {
        drawMPUFrame();           // При режиме 0 перерисовываем каркас MPU
      } else {
        drawEnvFrame();           // При режиме 1 — каркас ENV
      }
    }
  }

  // Вывод в Serial (по желанию можно скрыть)
  if (mpu_ok) {
    Serial.print("MPU ACC[g] X=");
    Serial.print(ax_f, 2);
    Serial.print(" Y=");
    Serial.print(ay_f, 2);
    Serial.print(" Z=");
    Serial.print(az_f, 2);

    Serial.print("  GYRO[dps] X=");
    Serial.print(gx_f, 1);
    Serial.print(" Y=");
    Serial.print(gy_f, 1);
    Serial.print(" Z=");
    Serial.print(gz_f, 1);

    Serial.print("  T_MPU=");
    Serial.print(temp_f, 1);
    Serial.print("C");
  } else {
    Serial.print("MPU ERR");
  }

  Serial.print("  |  ");

  if (aht_ok) {
    Serial.print("T_AHT=");
    Serial.print(aht_tC, 1);
    Serial.print("C  H=");
    Serial.print(aht_hR, 0);
    Serial.print("%  ");

    if (bmp_ok) {
      Serial.print("P=");
      Serial.print(p_mmHg, 0);
      Serial.print("mmHg");
    } else {
      Serial.print("P=N/A");
    }
  } else {
    Serial.print("AHT/BMP ERR");
  }
  Serial.println();

  // Обновление значений на экране без мерцания
  if (now - lastScreenUpdate > SCREEN_UPDATE_INTERVAL) { // Если пора обновить экран
    lastScreenUpdate = now;                             // Обновляем метку времени
    if (displayMode == 0) {                             // Если сейчас режим MPU
      if (mpu_ok) updateMPUValues();                    // Обновляем только числа MPU
    } else {                                            // Иначе режим ENV
      updateEnvValues();                                // Обновляем числа ENV
    }
  }
}