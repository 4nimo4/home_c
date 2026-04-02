#include <Wire.h>                  // Библиотека для шины I2C
#include "ArduinoGraphics.h"       // Графика для встроенной матрицы UNO R4
#include "Arduino_LED_Matrix.h"    // Управление светодиодной матрицей UNO R4

#include <Adafruit_AHTX0.h>        // Датчик темп./влажности AHT20
#include <Adafruit_BMP280.h>       // Датчик давления/темп. BMP280

// ----------------- ОБЩЕЕ: МАТРИЦА -----------------
ArduinoLEDMatrix matrix;           // Объект встроенной LED‑матрицы
char buffer[32];                   // Буфер для формирования текстовых строк
uint16_t speed = 40;               // Скорость прокрутки текста (мс на шаг)

// ----------------- MPU6050 (работаем напрямую по I2C через Wire) -----------------
const uint8_t MPU_ADDR = 0x68;     // Адрес MPU на шине I2C (найден сканером)
bool mpu_ok = false;               // Флаг: MPU инициализирован и отвечает

// Смещения (результат калибровки нулей)
float ax_offset = 0, ay_offset = 0, az_offset = 0;   // оффсеты акселерометра
float gx_offset = 0, gy_offset = 0, gz_offset = 0;   // оффсеты гироскопа
float temp_offset = 0;                               // при необходимости можем сдвинуть показания температуры

// Сглаженные значения (экспоненциальное сглаживание)
float ax_f = 0, ay_f = 0, az_f = 0;   // ускорения по осям X/Y/Z в g
float gx_f = 0, gy_f = 0, gz_f = 0;   // угловые скорости по осям в град/с
float temp_f = 0;                     // температура кристалла MPU, °C
const float alpha = 0.1f;             // Коэффициент сглаживания (0..1), чем меньше – тем более плавные значения

// ----------------- AHT20 + BMP280 -----------------
Adafruit_AHTX0 aht;                 // Объект датчика AHT20
Adafruit_BMP280 bmp;                // Объект датчика BMP280

bool aht_ok = false;                // Флаг: AHT20 инициализирован
bool bmp_ok = false;                // Флаг: BMP280 инициализирован
bool poterya_svyazi = false;        // Был ли во время работы сбой связи с AHT/BMP
unsigned long lastAhtRetry = 0;     // Время последней попытки переинициализации
const unsigned long AHT_RETRY_INTERVAL = 5000; // Пауза между попытками восстановления (мс)

// Режимы отображения на матрице:
// 0 – показываем только MPU (акселерометр/гироскоп/темп. MPU)
// 1 – показываем AHT/BMP (температура/влажность/давление)
int displayMode = 0;
unsigned long lastModeChange = 0;              // Время последнего переключения режима
const unsigned long MODE_INTERVAL = 5000;      // Интервал смены режимов (мс)

// --------------------------------------------------
// Вспомогательные функции
// --------------------------------------------------

// Универсальная функция прокрутки строки text по матрице
void scrollText(const char *text, uint16_t speed) {
  matrix.beginDraw();                 // Начинаем «сеанс рисования» на матрице
  matrix.stroke(0xFFFFFFFF);          // Цвет «кисти» (для матрицы – просто «вкл» светодиода)
  matrix.textScrollSpeed(speed);      // Скорость прокрутки текста
  matrix.clear();                     // Очистить текущий кадр матрицы
  matrix.textFont(Font_4x6);          // Выбор шрифта (4x6 – компактный)

// Позиция текста: x=0 – левый край, y=1 – чуть ниже верхней строки
  matrix.beginText(0, 1, 0xFFFFFF);   // Начало вывода текста (цвет – белый)
  matrix.println(text);               // Печатаем строку в буфер матрицы
  matrix.endText(SCROLL_LEFT);        // Завершаем текст и запускаем прокрутку влево

  matrix.endDraw();                   // Завершаем рисование – отправляем кадр на матрицу
}

// Запись одного байта в регистр MPU6050 по адресу reg
void mpuWrite(uint8_t reg, uint8_t data) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission();
}

// Чтение len байт из MPU6050, начиная с регистра reg, в буфер buf
void mpuRead(uint8_t reg, uint8_t *buf, uint8_t len) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);        // Повторный старт, без освобождения шины
  Wire.requestFrom((int)MPU_ADDR, (int)len); // Запрашиваем len байт
  for (uint8_t i = 0; i < len && Wire.available(); i++) {
    buf[i] = Wire.read();
  }
}

// --------- Калибровка MPU (датчик должен лежать неподвижно и ровно) ---------
void calibrateMPU(unsigned int samples = 200) {
  Serial.println("Calibrating MPU... Keep sensor still.");
  scrollText(" CALIBRATE ", speed);    // Сообщение на матрице, что идёт калибровка

  long ax_sum = 0, ay_sum = 0, az_sum = 0;   // Суммы для усреднения акселя
  long gx_sum = 0, gy_sum = 0, gz_sum = 0;   // Суммы для усреднения гироскопа
  long temp_sum = 0;                         // Сумма температуры (пока не используем)

  // Несколько сотен измерений подряд – усредняем шум/дрейф
  for (unsigned int i = 0; i < samples; i++) {
    uint8_t data[14];
    mpuRead(0x3B, data, 14);           // 14 байт: AX,AY,AZ,T,GX,GY,GZ (по 2 байта на каждый)

    int16_t ax_raw  = (int16_t)(data[0] << 8 | data[1]);
    int16_t ay_raw  = (int16_t)(data[2] << 8 | data[3]);
    int16_t az_raw  = (int16_t)(data[4] << 8 | data[5]);
    int16_t tempRaw = (int16_t)(data[6] << 8 | data[7]);
    int16_t gx_raw  = (int16_t)(data[8] << 8 | data[9]);
    int16_t gy_raw  = (int16_t)(data[10] << 8 | data[11]);
    int16_t gz_raw  = (int16_t)(data[12] << 8 | data[13]);

    ax_sum += ax_raw;
    ay_sum += ay_raw;
    az_sum += az_raw;
    gx_sum += gx_raw;
    gy_sum += gy_raw;
    gz_sum += gz_raw;
    temp_sum += tempRaw;

    delay(5);                          // Небольшая пауза между измерениями
  }

  // Средние значения сырых данных
  float ax_avg = (float)ax_sum / samples;
  float ay_avg = (float)ay_sum / samples;
  float az_avg = (float)az_sum / samples;
  float gx_avg = (float)gx_sum / samples;
  float gy_avg = (float)gy_sum / samples;
  float gz_avg = (float)gz_sum / samples;
  (void)temp_sum;                      // temp_avg_raw сейчас не используем

  // Для акселерометра хотим:
  // X ≈ 0 g, Y ≈ 0 g, Z ≈ +1 g (16384 в сырых значениях при диапазоне ±2g)
  ax_offset = ax_avg;                  // Сдвиг по X
  ay_offset = ay_avg;                  // Сдвиг по Y
  az_offset = az_avg - 16384;          // Сдвиг по Z так, чтобы в покое было ~1g

  // Для гироскопа хотим нули по всем осям при неподвижном датчике
  gx_offset = gx_avg;
  gy_offset = gy_avg;
  gz_offset = gz_avg;

  // Температуру пока не подстраиваем (можно будет задать поправку вручную)
  temp_offset = 0.0f;

  Serial.println("MPU calibration done.");
  scrollText(" CALIB OK ", speed);     // Краткое сообщение на матрице – калибровка завершена
}

// --------------------------------------------------
// setup() – выполняется один раз при старте/ресете
// --------------------------------------------------
void setup() {
  Serial.begin(115200);               // Старт порта на 115200 бод
  while (!Serial) { delay(10); }      // Ждём готовности USB‑Serial

  matrix.begin();                     // Инициализация LED‑матрицы
  Wire.begin();                       // Запуск шины I2C

  scrollText(" INIT SENS ", speed);   // Сообщение «инициализация датчиков»

  // ---------- Инициализация MPU ----------
  Wire.beginTransmission(MPU_ADDR);
  uint8_t err = Wire.endTransmission();
  if (err == 0) {
    uint8_t whoami = 0;
    mpuRead(0x75, &whoami, 1);        // Читаем WHO_AM_I – ID чипа
    Serial.print("MPU WHO_AM_I = 0x");
    Serial.println(whoami, HEX);

    mpuWrite(0x6B, 0x00);             // PWR_MGMT_1 = 0 – выводим из сна
    delay(100);

    mpuWrite(0x1C, 0x00);             // ACCEL_CONFIG: диапазон ±2g
    mpuWrite(0x1B, 0x00);             // GYRO_CONFIG: диапазон ±250°/с

    mpu_ok = true;
    Serial.println("MPU initialized.");
    scrollText(" MPU OK ", speed);

    // Калибровка MPU (во время этого датчик должен лежать ровно и неподвижно)
    calibrateMPU(200);

    // Первое чтение после калибровки – инициализируем фильтр сглаживания
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
    scrollText(" MPU ERR ", speed);
  }

  // ---------- Инициализация AHT20 ----------
  if (!aht.begin()) {                 // Пробуем запустить AHT20
    aht_ok = false;
    Serial.println("AHT20 not found");
    scrollText(" AHT20 ERR ", speed);
  } else {
    aht_ok = true;
    Serial.println("AHT20 OK");
    scrollText(" AHT20 OK ", speed);
  }

  // ---------- Инициализация BMP280 ----------
  // BMP280 обычно висит на адресе 0x76 или 0x77, пробуем оба
  if (!bmp.begin(0x76) && !bmp.begin(0x77)) {
    bmp_ok = false;
    Serial.println("BMP280 not found");
    scrollText(" BMP280 ERR ", speed);
  } else {
    bmp_ok = true;
    Serial.println("BMP280 OK");
    scrollText(" BMP280 OK ", speed);

    // Конфигурация BMP280: режим, пересэмплинг и фильтрация
    bmp.setSampling(
      Adafruit_BMP280::MODE_NORMAL,   // Нормальный непрерывный режим
      Adafruit_BMP280::SAMPLING_X2,   // Пересэмплинг температуры ×2
      Adafruit_BMP280::SAMPLING_X16,  // Пересэмплинг давления ×16
      Adafruit_BMP280::FILTER_X16,    // Фильтр показаний ×16
      Adafruit_BMP280::STANDBY_MS_500 // Пауза между измерениями ~500 мс
    );
  }

  scrollText("   READY   ", speed);   // Все датчики инициализированы – готовы к работе
}

// --------------------------------------------------
// loop() – главный цикл программы
// --------------------------------------------------
void loop() {
  // ---------- ЧТЕНИЕ MPU6050 ----------
  if (mpu_ok) {
    uint8_t data[14];
    mpuRead(0x3B, data, 14);          // Читаем весь пакет данных MPU

    int16_t ax_raw  = (int16_t)(data[0] << 8 | data[1]);
    int16_t ay_raw  = (int16_t)(data[2] << 8 | data[3]);
    int16_t az_raw  = (int16_t)(data[4] << 8 | data[5]);
    int16_t tempRaw = (int16_t)(data[6] << 8 | data[7]);
    int16_t gx_raw  = (int16_t)(data[8] << 8 | data[9]);
    int16_t gy_raw  = (int16_t)(data[10] << 8 | data[11]);
    int16_t gz_raw  = (int16_t)(data[12] << 8 | data[13]);

    // Применяем смещения (калибровку) и переводим в физические единицы
    float ax = (ax_raw - ax_offset) / 16384.0f;
    float ay = (ay_raw - ay_offset) / 16384.0f;
    float az = (az_raw - az_offset) / 16384.0f;

    float gx = (gx_raw - gx_offset) / 131.0f;
    float gy = (gy_raw - gy_offset) / 131.0f;
    float gz = (gz_raw - gz_offset) / 131.0f;

    float tempC = (tempRaw / 340.0f) + 36.53f + temp_offset;

    // Экспоненциальное сглаживание – уменьшаем шум и рывки показаний
    ax_f = ax_f + alpha * (ax - ax_f);
    ay_f = ay_f + alpha * (ay - ay_f);
    az_f = az_f + alpha * (az - az_f);

    gx_f = gx_f + alpha * (gx - gx_f);
    gy_f = gy_f + alpha * (gy - gy_f);
    gz_f = gz_f + alpha * (gz - gz_f);

    temp_f = temp_f + alpha * (tempC - temp_f);
  }

  // ---------- ЧТЕНИЕ AHT20 + BMP280 ----------
  float aht_tC = 0, aht_hR = 0;      // Температура и влажность с AHT20
  float pPa = 0, p_hPa = 0, p_mmHg = 0;   // Давление в разных единицах

  if (aht_ok) {
    sensors_event_t humidity, temp;
    if (aht.getEvent(&humidity, &temp)) {   // Успешно получены новые данные AHT20
      aht_tC = temp.temperature;
      aht_hR = humidity.relative_humidity;

      if (bmp_ok) {
        pPa  = bmp.readPressure();        // Давление в Паскалях
        p_hPa = pPa / 100.0;              // В гектопаскалях
        p_mmHg = p_hPa * 0.75006;         // Перевод в мм рт. ст.
      }
    } else if (!poterya_svyazi) {
      // Ошибка чтения AHT20 – считаем, что датчики временно недоступны
      aht_ok = false;
      bmp_ok = false;
      poterya_svyazi = true;
      lastAhtRetry = millis();            // Запоминаем время, с которого начнём пытаться восстановить связь
    }
  }

  // Периодические попытки заново инициализировать AHT20 и BMP280 после сбоя
  if (poterya_svyazi &&
      (millis() - lastAhtRetry) > AHT_RETRY_INTERVAL) {
    Serial.println("Trying to re-init AHT20 & BMP280...");
    if (aht.begin() && (bmp.begin(0x76) || bmp.begin(0x77))) {
      aht_ok = true;
      bmp_ok = true;
      poterya_svyazi = false;
      Serial.println("AHT20 & BMP280 re-init OK");
      scrollText(" AHT OK ", speed);
      scrollText(" BMP OK ", speed);
    } else {
      Serial.println("Still no AHT/BMP");
    }
  }

  // ---------- Вывод в Serial для отладки ----------
  if (mpu_ok) {
    Serial.print("ACC[g] X=");
    Serial.print(ax_f, 2);
    Serial.print(" Y=");
    Serial.print(ay_f, 2);
    Serial.print(" Z=");
    Serial.println(az_f, 2);

    Serial.print("GYRO[dps] X=");
    Serial.print(gx_f, 1);
    Serial.print(" Y=");
    Serial.print(gy_f, 1);
    Serial.print(" Z=");
    Serial.println(gz_f, 1);

    Serial.print("T_MPU=");
    Serial.print(temp_f, 1);
    Serial.println("C");
  } else {
    Serial.println("MPU ERR");
  }

  if (aht_ok) {
    Serial.print("T_AHT=");
    Serial.print(aht_tC, 1);
    Serial.println("C");
    Serial.print("H=");
    Serial.print(aht_hR, 1);
    Serial.println("%");
    Serial.print("P=");
    Serial.print(p_mmHg, 0);
    Serial.println("mmHg");
  } else {
    Serial.println("AHT/BMP ERR");
  }
  Serial.println();

  // ---------- Переключение режима отображения на матрице по времени ----------
  unsigned long now = millis();
  if (now - lastModeChange > MODE_INTERVAL) {
    lastModeChange = now;
    displayMode = (displayMode + 1) % 2;   // Цикл: 0 -> 1 -> 0 -> ...
  }

  // ---------- Матрица: режимы отображения ----------
  if (displayMode == 0) {
    // ----- Режим 0: выводим данные MPU (аксель, гироскоп, температура MPU) -----
    if (mpu_ok) {
      snprintf(buffer, sizeof(buffer), " AX=%.2fg ", ax_f);
      scrollText(buffer, speed);

      snprintf(buffer, sizeof(buffer), " AY=%.2fg ", ay_f);
      scrollText(buffer, speed);

      snprintf(buffer, sizeof(buffer), " AZ=%.2fg ", az_f);
      scrollText(buffer, speed);

      snprintf(buffer, sizeof(buffer), " GX=%.1fd ", gx_f);
      scrollText(buffer, speed);

      snprintf(buffer, sizeof(buffer), " GY=%.1fd ", gy_f);
      scrollText(buffer, speed);

      snprintf(buffer, sizeof(buffer), " GZ=%.1fd ", gz_f);
      scrollText(buffer, speed);

      snprintf(buffer, sizeof(buffer), " TMPU=%.1fC ", temp_f);
      scrollText(buffer, speed);
    } else {
      scrollText(" MPU ERR ", speed);
    }

  } else {
    // ----- Режим 1: выводим данные AHT/BMP (температура воздуха, влажность, давление) -----
    if (aht_ok) {
      snprintf(buffer, sizeof(buffer), " TAHT=%.1fC ", aht_tC);
      scrollText(buffer, speed);

      snprintf(buffer, sizeof(buffer), " H=%.0f%% ", aht_hR);
      scrollText(buffer, speed);

      if (bmp_ok) {
        snprintf(buffer, sizeof(buffer), " P=%.0fmmHg ", p_mmHg);
        scrollText(buffer, speed);
      }
    } else {
      scrollText(" AHT20 ERR ", speed);
      if (!bmp_ok) {
        scrollText(" BMP280 ERR ", speed);
      }
    }
  }

  delay(200);      // Небольшая пауза перед следующим циклом
}
/*
#include <Wire.h>

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }
  Serial.println("I2C scanner...");

  Wire.begin();
}

void loop() {
  byte error, address;
  int nDevices = 0;

  Serial.println("Scanning...");

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.println();
      nDevices++;
    }
  }

  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  } else {
    Serial.println("done\n");
  }

  delay(2000);
}
*/