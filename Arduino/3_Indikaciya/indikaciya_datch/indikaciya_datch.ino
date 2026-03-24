#include <Wire.h>                  // Библиотека для работы с шиной I2C
#include <Adafruit_AHTX0.h>        // Библиотека для датчика температуры/влажности AHT20
#include <Adafruit_BMP280.h>       // Библиотека для датчика давления/температуры BMP280

// ВАЖНО: сначала подключаем ArduinoGraphics, затем Arduino_LED_Matrix,
// иначе текстовые функции (textFont, beginText и т.д.) могут быть недоступны.
#include "ArduinoGraphics.h"       // Графическая библиотека Arduino (шрифты, текст, примитивы)
#include "Arduino_LED_Matrix.h"    // Библиотека для управления встроенной LED‑матрицей UNO R4 WiFi

// Создаём объект матрицы
ArduinoLEDMatrix matrix;

// Создаём объекты датчиков
Adafruit_AHTX0 aht;                // AHT20 – температура и влажность
Adafruit_BMP280 bmp;               // BMP280 – давление и температура

// Буфер для формирования текстовых строк, которые будем прокручивать
char buffer[32];
uint16_t speed = 60;
bool aht_ok = false;   // есть ли рабочий AHT20 
bool bmp_ok = false;   // есть ли рабочий BMP280
bool poterya_svyazi = false; //во время работы обнаружена потеря связи с датчиками
unsigned long lastAhtRetry = 0;
const unsigned long AHT_RETRY_INTERVAL = 5000;// после аварии ждем 5 секунд и пробуем восстановить связь с датчиками  
// -------------------------------------------------------------
// Универсальная функция: прокрутить текстовую строку text
// по матрице слева направо с заданной скоростью speed (мс/шаг)
// -------------------------------------------------------------
void scrollText(const char *text, uint16_t speed) {
  matrix.beginDraw();                 //сообщаем библиотеке, что мы начинаем группу графических операций:
   //настройка цвета,установка шрифта,вывод текста.
  //внутри библиотеки может очищаться внутренний буфер кадра,
  //включаться режим «рисование в память, а не сразу на экран».

  matrix.stroke(0xFFFFFFFF);          // Устанавливаем цвет «кисти» (для матрицы просто «включено»)
  //stroke – «цвет пера/кисти».На обычных дисплеях это цвет линий/текста (ARGB: AARRGGBB).
  //На матрице UNO R4 WiFi светодиоды просто «вкл/выкл», 
  //так что любой ненулевой цвет означает «светодиод горит»
  //0xFFFFFFFF – полностью белый с полной непрозрачностью (255,255,255,255).

  matrix.textScrollSpeed(speed);      // Задаём скорость прокрутки текста (мс на один шаг)
  //Устанавливаем скорость скроллинга текста-меньше число → быстрее прокрутка,больше число → медленнее.
  //Speed измеряется в миллисекундах на один шаг сдвига текста.

  matrix.clear();                     // Очищаем содержимое матрицы
  //Очищает экран/кадр-гасит все светодиоды,или очищает внутренний буфер, в который будет рисоваться текст.

  matrix.textFont(Font_4x6);          // Выбираем шрифт 5x7 (высота 7 пикселей, ширина 5)
  //Файл с этим шрифтом лежит в библиотеке ArduinoGraphics (там описаны битовые маски для каждого символа).

  // Начинаем выводить текст:
  // координаты x=0 - стартовая колонка
  //y=1 (чтобы текст не упирался в самый верхний край),
  // цвет белый (0xFFFFFF)
  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.println(text);               // Выводим строку текста в буфер графики
  matrix.endText(SCROLL_LEFT);        // Завершаем вывод и запускаем прокрутку влево

  // Завершаем «рисование» на матрице – команда отправляется на экран
  matrix.endDraw();
}

// -------------------------------------------------------------
// Функция setup() – выполняется один раз при старте/ресете
// -------------------------------------------------------------
void setup() {
  Serial.begin(115200);               // Запускаем последовательный порт на 115200 бод
  while (!Serial){
     delay(10); 
    }      // Ждём, пока порт инициализируется (актуально для некоторых плат)

  // Инициализация матрицы
  matrix.begin();

  // Инициализация I2C (SDA/SCL на плате UNO R4 WiFi)
  Wire.begin();
  scrollText(" INICIAL", speed/2);
  // -------- Инициализация AHT20 --------
  if (!aht.begin()) {                 // Пытаемся инициализировать датчик
    aht_ok = false;                   // датчик НЕ работает
    Serial.println(" AHT20 not found");
    scrollText(" AHT20 ERR", speed/2);  // Однократное сообщение об ошибке
  } else {
    aht_ok = true;                    // датчик успешно инициализирован
    Serial.println(" AHT20 OK");
    scrollText("  AHT20 OK", speed/2);
  }

  // -------- Инициализация BMP280 --------
  // Датчик BMP280 чаще всего имеет I2C‑адрес 0x76 или 0x77,
  // пробуем оба варианта.
  if (!bmp.begin(0x76) && !bmp.begin(0x77)) {
    bmp_ok = false;                     // датчик НЕ работает
    Serial.println(" BMP280 not found");
    scrollText(" BMP280 ERR", speed/2);   // Однократное сообщение об ошибке
    } else {
      bmp_ok = true;                      // датчик успешно инициализирован
      Serial.println(" BMP280 OK");
      scrollText(" BMP280 OK", speed/2);
    }
  

  // Настраиваем режим работы BMP280:
  bmp.setSampling(
    Adafruit_BMP280::MODE_NORMAL,     // Нормальный непрерывный режим измерения
    Adafruit_BMP280::SAMPLING_X2,     // Пересэмплинг температуры ×2 (средняя точность, меньше шум)
    Adafruit_BMP280::SAMPLING_X16,    // Пересэмплинг давления ×16 (более точное давление)
    Adafruit_BMP280::FILTER_X16,      // Фильтрация показаний ×16 (сглаживание)
    Adafruit_BMP280::STANDBY_MS_500   // Интервал ожидания между измерениями ~500 мс
  );

  // При успешной инициализации выводим на матрицу «READY»
  scrollText("    HELLO!!! ", speed);
}

// -------------------------------------------------------------
// Функция loop() – главный цикл, выполняется бесконечно
// -------------------------------------------------------------
void loop() {

  float tC = 0;                             // Температура в градусах Цельсия
  float hR = 0;                             // Относительная влажность, %

  float pPa  = 0;                           // Давление в Паскалях (Па)
  float p_hPa = 0;                          // Переводим Паскали в гектопаскали (делим на 100)
  float p_mmHg = 0;                         // Перевод в мм рт. ст. (1 hPa ≈ 0.75006 мм рт. ст.)


  if(aht_ok){
    // --- Чтение AHT20: температура и влажность ---
    sensors_event_t humidity, temp;   // Структуры, куда библиотека запишет измерения
    if(aht.getEvent(&humidity, &temp)){   // Запрашиваем новые данные у датчика AHT20
      tC = temp.temperature;
      hR = humidity.relative_humidity;
      // --- Чтение BMP280: давление ---
      pPa  = bmp.readPressure();          // Давление в Паскалях (Па)
      p_hPa = pPa / 100.0;                // Переводим Паскали в гектопаскали (делим на 100)
      p_mmHg = p_hPa * 0.75006;           // Перевод в мм рт. ст. (1 hPa ≈ 0.75006 мм рт. ст.)
    } else if(poterya_svyazi == false){
        // На всякий случай: если во время работы что-то пошло не так
        // Можно временно считать, что датчика нет:
        aht_ok = false;
        bmp_ok = false;
        poterya_svyazi = true;
        lastAhtRetry = millis();
      }
  }

  if(poterya_svyazi == true && (millis() - lastAhtRetry) > AHT_RETRY_INTERVAL){
    Serial.println("Trying to re-init AHT20...");
    Serial.println("Trying to re-init BMP280...");
    if (aht.begin() && (bmp.begin(0x76) || bmp.begin(0x77))) {
      aht_ok = true;
      bmp_ok = true;
      poterya_svyazi = false;
      Serial.println("AHT20 re-initialized OK");
      Serial.println("BMP280 re-initialized OK");
      scrollText("AHT OK", speed);
      scrollText("BMP OK", speed);
    } else {
      Serial.println("AHT20 still not found");
      Serial.println("BMP280 still not found");
    }
  }
  // --- Вывод в Serial Monitor для отладки ---
  if(aht_ok){
    Serial.print("T=");
    Serial.print(tC);
    Serial.print("C  H=");
    Serial.print(hR);
    Serial.print(" P=");
    Serial.print(p_mmHg);
    Serial.println("mmHg");
  } else {
    Serial.println(" ERROR: AHT20 read error");
    Serial.println(" ERROR: BMP280 read error");
  }

  
  
  if(aht_ok){
    // --- 1) Прокрутка температуры на матрице ---
    // Формируем строку " T=23.4C "
    snprintf(buffer, sizeof(buffer), " T=%.1fC ", tC);
    scrollText(buffer, speed);               // Прокручиваем эту строку по матрице
                                            //чем значение меньше, тем выше скорость прокрутки
    // --- 3) Прокрутка влажности на матрице ---
    // Формируем строку " H=45% " 
    snprintf(buffer, sizeof(buffer), " H=%.0f%% ", hR);
    scrollText(buffer, speed);
    // --- 2) Прокрутка давления на матрице ---
    // Вариант в hPa (закомментирован на случай, если понадобится):
    // snprintf(buffer, sizeof(buffer), " P=%.0fhPa ", p_hPa);
    // scrollText(buffer, 70);

    // Текущий вариант: давление в мм рт. ст. " P=760mmHg "
    snprintf(buffer, sizeof(buffer), " P=%.0fmmHg ", p_mmHg);
    scrollText(buffer, speed);
  } else {
    // Вместо показаний выводим сообщение о том, что датчика нет
    scrollText(" AHT20 ERR ", speed);
    scrollText(" BMP280 ERR ", speed);
  }

 
  // Небольшая пауза перед следующим циклом измерений
  delay(500);
}