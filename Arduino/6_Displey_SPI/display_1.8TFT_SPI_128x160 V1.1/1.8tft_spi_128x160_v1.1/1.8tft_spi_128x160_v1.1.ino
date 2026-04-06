#include <Wire.h>        // Библиотека для работы по шине I2C (MPU6050, AHT20, BMP280)
#include <SPI.h>         // Библиотека для работы по шине SPI (дисплей)

// Библиотеки Adafruit для графики и конкретных устройств
#include <Adafruit_GFX.h>         // Графическая библиотека (текст, линии и т.п.)
#include <Adafruit_ST7735.h>      // Драйвер TFT-дисплея ST7735 (1.8" 128x160)
#include <Adafruit_AHTX0.h>       // Драйвер датчика AHT20 (температура + влажность)
#include <Adafruit_BMP280.h>      // Драйвер датчика BMP280 (давление + температура)

#define SET_ROTAC 2              //Поворот экрана от 0 до 3
#define ZAGOLOVOK 2              //размер заголовка
#define OSN_TEXT  1              //размер текста
#define DATCHIK   1              //размер названия датчика

#define SMESH_L1  2              //смещение от левого края экрана, начинаем печатать название коорд датч MPU6050
#define SMESH_L2  50             //смещ от лев края экр, начин печа сами коорд дат MPU6050
#define SMESH_L3  2              //смещ от лев края экр, начин печа название коорд дат ENV (AHT20/BMP280)
#define SMESH_L4  50             //смещ от лев края экр, начин печа сами коорд дат ENV (AHT20/BMP280)
//-------------------------------------------------------------------
//Цвета фона и текста
//---------------------------------------------------------------------
// дополнительные цвета в формате RGB565
#define ST77XX_NAVY      0x000F
#define ST77XX_DARKGREEN 0x03E0
#define ST77XX_DARKGREY  0x7BEF
#define ST77XX_LIGHTGREY 0xC618
#define ST77XX_MAROON    0x7800
#define ST77XX_PURPLE    0x780F
#define ST77XX_OLIVE     0x7BE0
#define ST77XX_BROWN     0x79E0
#define ST77XX_PINK      0xF81F  // или другой оттенок по вкусу
//Цвета заставки
//-----------------------------------------------------------------
//"INIT SENS"
#define COL_TEXT1   ST77XX_WHITE  
#define COL_FON1    ST77XX_PURPLE    
//"MPU OK"
#define COL_TEXT2   ST77XX_WHITE 
#define COL_FON2    ST77XX_PURPLE  
//"MPU ERR"
#define COL_TEXT3   ST77XX_RED     
#define COL_FON3    ST77XX_PURPLE   
//"AHT20 ERR"
#define COL_TEXT4   ST77XX_RED    
#define COL_FON4    ST77XX_PURPLE          
//"AHT20 OK"
#define COL_TEXT5   ST77XX_WHITE           
#define COL_FON5    ST77XX_PURPLE    
//"BMP280 ERR"
#define COL_TEXT6   ST77XX_RED    
#define COL_FON6    ST77XX_PURPLE
//"BMP280 OK"
#define COL_TEXT7   ST77XX_WHITE    
#define COL_FON7    ST77XX_PURPLE 
//---------------------------------------------------------------------
//Цвета данных
//-----------------------------------------------------------------
// Рисуем «каркас» экрана MPU: подписи без чисел
#define COL_TEXT8   ST77XX_GREEN  //цвет текста каркаса  
#define COL_FON8    ST77XX_BLACK 
#define COL_TEXT9   ST77XX_YELLOW  //цвет теста данных
// Рисуем «каркас» экрана ENV (AHT20/BMP280): подписи без чисел
#define COL_TEXT10   ST77XX_GREEN  //цвет текста каркаса  
#define COL_FON10    ST77XX_BLACK 
#define COL_TEXT11   ST77XX_YELLOW  //цвет теста данных
/*
Библиотека Adafruit_ST77xx (ST7789) уже содержит набор готовых цветовых констант в формате RGB565.
Вот основные встроенные цвета:

ST77XX_BLACK      // чёрный
ST77XX_WHITE      // белый
ST77XX_RED        // красный
ST77XX_GREEN      // зелёный
ST77XX_BLUE       // синий
ST77XX_CYAN       // циан (бирюзовый)
ST77XX_MAGENTA    // маджента
ST77XX_YELLOW     // жёлтый
ST77XX_ORANGE     // оранжевый
-------цвета ниже определены в define -------------
ST77XX_PURPLE     // фиолетовый       
ST77XX_PINK       // розовый          
ST77XX_BROWN      // коричневый       
ST77XX_NAVY       // тёмно‑синий     
ST77XX_DARKGREEN  // тёмно‑зелёный    
ST77XX_MAROON     // тёмно‑красный   
ST77XX_OLIVE      // оливковый        
ST77XX_DARKGREY   // тёмно‑серый      
ST77XX_LIGHTGREY  // светло‑серый     
*/

/*
// -----------------------------------------------------------------------------
Подключение 1.8" TFT 128×160
По фото/типовой плате (с SD‑слотом):

VCC → 5V
GND → GND
LED/BL → через 22 Ом → 5V (или на PWM‑пин через транзистор, если нужен ШИМ)
SCK → D13
SDA/MOSI → D11
A0/DC → D9
RESET → D8
CS → D10
SD‑пины (SD_CS, SD_MOSI и т.п.) можно не подключать.
*/
//   НАСТРОЙКИ TFT ST7735 (1.8" 128x160)
//   Подключение:
//     D10 -> CS   (SS/CS дисплея)
//     D9  -> DC   (A0 / D-C)
//     D8  -> RST  (RESET)
//     D11 -> MOSI (SDA)
//     D13 -> SCK  (SCL)
// -----------------------------------------------------------------------------
#define TFT_CS   10      // Пин выбора устройства (CS) дисплея
#define TFT_DC    9      // Пин DC (Data/Command)
#define TFT_RST   8      // Пин аппаратного сброса дисплея

// Создаём объект дисплея ST7735. Пины SCK и MOSI берутся из аппаратного SPI (13,11).
Adafruit_ST7735 tft(TFT_CS, TFT_DC, TFT_RST);

// -----------------------------------------------------------------------------
//   НАСТРОЙКИ MPU6050 (акселерометр + гироскоп + температура кристалла)
// -----------------------------------------------------------------------------
const uint8_t MPU_ADDR = 0x68;   // Адрес MPU6050 на шине I2C (AD0 = GND)
bool mpu_ok = false;             // Флаг: MPU6050 успешно инициализирован?

// Смещения нулей (офсеты), вычисленные при калибровке
float ax_offset = 0, ay_offset = 0, az_offset = 0;  // Оффсеты акселерометра
float gx_offset = 0, gy_offset = 0, gz_offset = 0;  // Оффсеты гироскопа
float temp_offset = 0;                              // Поправка к температуре кристалла (пока 0)

// Сглаженные значения (экспоненциальное сглаживание)
float ax_f = 0, ay_f = 0, az_f = 0;   // Ускорения по X/Y/Z в g
float gx_f = 0, gy_f = 0, gz_f = 0;   // Угловые скорости по X/Y/Z в град/с
float temp_f = 0;                     // Температура MPU, °C
const float alpha = 0.1f;             // Коэффициент сглаживания (0..1), меньше — плавнее изменения

// -----------------------------------------------------------------------------
//   НАСТРОЙКИ AHT20 + BMP280 (температура/влажность/давление)
// -----------------------------------------------------------------------------
Adafruit_AHTX0 aht;          // Объект датчика AHT20
Adafruit_BMP280 bmp;         // Объект датчика BMP280

bool aht_ok = false;         // Флаг: AHT20 инициализирован?
bool bmp_ok = false;         // Флаг: BMP280 инициализирован?
bool poterya_svyazi = false; // Во время работы была потеря связи с AHT/BMP?
unsigned long lastAhtRetry = 0;                 // Время последней попытки восстановить связь
const unsigned long AHT_RETRY_INTERVAL = 5000;  // Интервал между попытками (мс)

// Текущие значения ENV
float aht_tC = 0;      // Температура воздуха, °C
float aht_hR = 0;      // Влажность воздуха, %
float p_mmHg = 0;      // Давление, мм рт. ст.

// -----------------------------------------------------------------------------
//   РЕЖИМЫ ОТОБРАЖЕНИЯ (0 – MPU, 1 – ENV) И ТАЙМЕР ЭКРАНА
// -----------------------------------------------------------------------------
int displayMode = 0;                         // Текущий режим: 0 — MPU, 1 — AHT/BMP
unsigned long lastScreenUpdate = 0;          // Время последнего обновления чисел на TFT
const unsigned long SCREEN_UPDATE_INTERVAL = 2000; // Интервал обновления чисел (мс)

// -----------------------------------------------------------------------------
//   КНОПКА ПЕРЕКЛЮЧЕНИЯ РЕЖИМОВ (D2 -> кнопка -> GND, INPUT_PULLUP)
// -----------------------------------------------------------------------------
const int buttonPin = 2;              // Пин для кнопки
bool lastButtonState = HIGH;          // Предыдущее состояние (HIGH = не нажата)
unsigned long lastButtonChange = 0;   // Время последнего изменения
const unsigned long DEBOUNCE_MS = 50; // Интервал антидребезга (мс)

// -----------------------------------------------------------------------------
//   ВЫВОД В SERIAL С НАСТРАИВАЕМЫМ ИНТЕРВАЛОМ
// -----------------------------------------------------------------------------
unsigned long lastSerialPrint = 0;           // Время последнего вывода в Serial
unsigned long serialPrintInterval = 60000;   // Интервал вывода (мс). 60000 = 1 минута.

// ============================================================================
//   ФУНКЦИИ ДЛЯ MPU6050
// ============================================================================

// Записать байт data в регистр reg MPU6050
void mpuWrite(uint8_t reg, uint8_t data) {
  Wire.beginTransmission(MPU_ADDR);  // Начинаем передачу к MPU6050
  Wire.write(reg);                   // Отправляем номер регистра
  Wire.write(data);                  // Отправляем значение
  Wire.endTransmission();            // Завершаем передачу
}

// Прочитать len байт из MPU6050, начиная с регистра reg, в буфер buf
void mpuRead(uint8_t reg, uint8_t *buf, uint8_t len) {
  Wire.beginTransmission(MPU_ADDR);  // Начинаем передачу
  Wire.write(reg);                   // Пишем начальный регистр
  Wire.endTransmission(false);       // Завершаем с повторным стартом (без STOP)
  Wire.requestFrom((int)MPU_ADDR, (int)len); // Запрашиваем len байт у MPU

  for (uint8_t i = 0; i < len && Wire.available(); i++) {
    buf[i] = Wire.read();            // Читаем каждый доступный байт
  }
}

// Калибровка MPU6050: усредняем несколько сотен измерений в покое
void calibrateMPU(unsigned int samples = 200) {
  Serial.println("Calibrating MPU... Keep sensor still.");

  long ax_sum = 0, ay_sum = 0, az_sum = 0;   // Суммы для акселерометра
  long gx_sum = 0, gy_sum = 0, gz_sum = 0;   // Суммы для гироскопа
  long temp_sum = 0;                         // Сумма температур (пока не используем)

  for (unsigned int i = 0; i < samples; i++) {
    uint8_t data[14];                        // 14 байт: AX,AY,AZ,T,GX,GY,GZ
    mpuRead(0x3B, data, 14);                 // Читаем блок регистров начиная с 0x3B

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

    delay(5);                                // Короткая пауза между измерениями
  }

  // Средние значения сырых данных
  float ax_avg = (float)ax_sum / samples;
  float ay_avg = (float)ay_sum / samples;
  float az_avg = (float)az_sum / samples;
  float gx_avg = (float)gx_sum / samples;
  float gy_avg = (float)gy_sum / samples;
  float gz_avg = (float)gz_sum / samples;
  (void)temp_sum;                            // temp_sum пока не используем

  // Для акселя: хотим X≈0g, Y≈0g, Z≈+1g (16384 сырых единиц при ±2g)
  ax_offset = ax_avg;
  ay_offset = ay_avg;
  az_offset = az_avg - 16384;

  // Для гироскопа: хотим в покое 0°/с по всем осям
  gx_offset = gx_avg;
  gy_offset = gy_avg;
  gz_offset = gz_avg;

  temp_offset = 0.0f;                        // Поправка по температуре пока не нужна

  Serial.println("MPU calibration done.");
}

// ============================================================================
//   ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ДЛЯ TFT
// ============================================================================
//Пример вызова - showMessageTFT("INIT SENS", ZAGOLOVOK, COL_TEXT1, COL_FON1);  // Показываем «INIT SENS»
// Вывести короткое сообщение text по центру экрана TFT
void showMessageTFT(const char *text, uint8_t size, uint16_t color_text, uint16_t color_fon) {
  tft.fillScreen(color_fon);           // Очистить экран установить фон
  tft.setTextWrap(false);                 // Не переносить текст автоматически
  tft.setTextColor(color_text, color_fon);  // Цвет текста, цвет фона
  tft.setTextSize(size);                  // Масштаб шрифта

  int16_t x1, y1;        // сюда библиотека вернёт координаты левого верхнего угла
                         // прямоугольника, в котором «лежит» текст (может быть не 0,0)
  uint16_t w, h;         // сюда вернутся ширина (w) и высота (h) этого прямоугольника

  // Запрашиваем у библиотеки размеры и положение текста "text",
  // если бы он был нарисован с начальной точкой (0,0).
  // В результате в x1, y1, w, h будут лежать реальные границы текста.
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

  // Вычисляем координату X так, чтобы текст оказался по центру экрана по горизонтали:
  // берём ширину экрана и вычитаем ширину текста, затем делим пополам.
  int16_t x = (tft.width()  - w) / 2;

  // Аналогично вычисляем координату Y для вертикального центрирования текста.
  int16_t y = (tft.height() - h) / 2;

  tft.setCursor(x, y);                    // Устанавливаем курсор
  tft.print(text);                        // Печатаем строку
}

// Рисуем «каркас» экрана MPU: подписи без чисел
void drawMPUFrame() {
  tft.fillScreen(COL_FON8);                   // Очищаем экран
  tft.setTextWrap(false);
  tft.setTextColor(COL_TEXT8, COL_FON8);   // Подписи зелёным
  tft.setTextSize(DATCHIK);                             // Шрифт маленький (экран 128x160)

  //SMESH_L1    - Отступ слева
  tft.setCursor(SMESH_L1, 2);  tft.print("MPU6050");    // Заголовок
  tft.setTextSize(OSN_TEXT);

  tft.setCursor(SMESH_L1, 18); tft.print("AX:");        // Подписываем оси акселя
  tft.setCursor(SMESH_L1, 30); tft.print("AY:");
  tft.setCursor(SMESH_L1, 42); tft.print("AZ:");

  tft.setCursor(SMESH_L1, 60); tft.print("GX:");        // Подписываем оси гироскопа
  tft.setCursor(SMESH_L1, 72); tft.print("GY:");
  tft.setCursor(SMESH_L1, 84); tft.print("GZ:");

  tft.setCursor(SMESH_L1, 102);tft.print("T-MPU:");         // Подписываем температуру MPU
}

// Обновляем числовые значения MPU, не трогая весь фон (чтобы не мигало)
void updateMPUValues() {
  tft.setTextWrap(false);
  tft.setTextColor(COL_TEXT9, COL_FON8);   // Значения белым
  tft.setTextSize(OSN_TEXT);

  // SMESH_L2 - Колонка для чисел

  // Для каждого поля сначала затираем старое значение пробелами, затем печатаем новое
  tft.setCursor(SMESH_L2, 18); tft.print("        "); tft.setCursor(SMESH_L2, 18); tft.print(ax_f, 2);
  tft.setCursor(SMESH_L2, 30); tft.print("        "); tft.setCursor(SMESH_L2, 30); tft.print(ay_f, 2);
  tft.setCursor(SMESH_L2, 42); tft.print("        "); tft.setCursor(SMESH_L2, 42); tft.print(az_f, 2);

  tft.setCursor(SMESH_L2, 60); tft.print("        "); tft.setCursor(SMESH_L2, 60); tft.print(gx_f, 1);
  tft.setCursor(SMESH_L2, 72); tft.print("        "); tft.setCursor(SMESH_L2, 72); tft.print(gy_f, 1);
  tft.setCursor(SMESH_L2, 84); tft.print("        "); tft.setCursor(SMESH_L2, 84); tft.print(gz_f, 1);

  tft.setCursor(SMESH_L2, 102); tft.print("        "); tft.setCursor(SMESH_L2, 102); tft.print(temp_f, 1);
}

// Рисуем «каркас» экрана ENV (AHT20/BMP280)
void drawEnvFrame() {
  tft.fillScreen(COL_FON10);                   // Очищаем экран
  tft.setTextWrap(false);
  tft.setTextColor(COL_TEXT10, COL_FON10);   // Подписи зелёным
  tft.setTextSize(DATCHIK);

  //SMESH_L3    - Отступ слева
  tft.setCursor(SMESH_L3, 2);  tft.print("AHT20/BMP280"); // Заголовок
  tft.setTextSize(OSN_TEXT);

  tft.setCursor(SMESH_L3, 22); tft.print("Temp:");          // Температура воздуха
  tft.setCursor(SMESH_L3, 42); tft.print("Hydro:");          // Влажность
  tft.setCursor(SMESH_L3, 62); tft.print("Psc:");          // Давление
}

// Обновляем значения температуры, влажности и давления на экране ENV
void updateEnvValues() {
  tft.setTextWrap(false);
  tft.setTextColor(COL_TEXT11, COL_FON10);   // Значения белым
  tft.setTextSize(OSN_TEXT);

  // SMESH_L4 - Колонка для чисел

  // Температура
  tft.setCursor(SMESH_L4, 22);
  tft.print("           ");                       // Стираем старое значение
  tft.setCursor(SMESH_L4, 22);
  if (aht_ok) {
    tft.print(aht_tC, 1);                         // Печатаем T воздуха
    tft.print(" C");
  } else {
    tft.print("ERR");
  }

  // Влажность
  tft.setCursor(SMESH_L4, 42);
  tft.print("           ");
  tft.setCursor(SMESH_L4, 42);
  if (aht_ok) {
    tft.print(aht_hR, 0);                         // Влажность без дробной части
    tft.print(" %");
  } else {
    tft.print("ERR");
  }

  // Давление
  tft.setCursor(SMESH_L4, 62);
  tft.print("           ");
  tft.setCursor(SMESH_L4, 62);
  if (bmp_ok) {
    tft.print(p_mmHg, 0);                         // Давление в мм рт. ст.
    tft.print(" p_mmHg");
  } else {
    tft.print("ERR");
  }
}

// ============================================================================
//   SETUP() – инициализация всех устройств
// ============================================================================

void setup() {
  Serial.begin(115200);            // Запускаем Serial на 115200 бод
  while (!Serial) { delay(10); }   // Ждём, пока USB‑Serial станет доступен (для UNO R4)

  Wire.begin();                    // Стартуем шину I2C
  pinMode(buttonPin, INPUT_PULLUP);// Настраиваем пин кнопки с внутренней подтяжкой к +V

  // Инициализация TFT ST7735 для 1.8" 128x160
  tft.initR(INITR_BLACKTAB);      // INITR_BLACKTAB – типичный вариант платы
  tft.setRotation(SET_ROTAC);     // Поворот экрана (0..3) – здесь 2, чтобы удобно читать
  tft.fillScreen(COL_FON1);   // Заливаем экран чёрным
  tft.setTextWrap(false);         // Не переносим текст автоматически

  showMessageTFT("INIT SENS", ZAGOLOVOK, COL_TEXT1, COL_FON1);  // Показываем «INIT SENS»
  delay(500);                                   // Даём время прочитать

  // ---------- Инициализация MPU6050 ----------
  Wire.beginTransmission(MPU_ADDR);
  uint8_t err = Wire.endTransmission();
  if (err == 0) {                                // Если устройство отвечает
    uint8_t whoami = 0;
    mpuRead(0x75, &whoami, 1);                   // Читаем регистр WHO_AM_I
    Serial.print("MPU WHO_AM_I = 0x");
    Serial.println(whoami, HEX);

    mpuWrite(0x6B, 0x00);                        // Будим чип: PWR_MGMT_1 = 0
    delay(100);
    mpuWrite(0x1C, 0x00);                        // ACCEL_CONFIG: ±2g
    mpuWrite(0x1B, 0x00);                        // GYRO_CONFIG: ±250°/с

    mpu_ok = true;
    Serial.println("MPU initialized.");
    showMessageTFT("MPU OK", ZAGOLOVOK,  COL_TEXT2, COL_FON2);
    delay(500);

    calibrateMPU(200);                           // Калибруем нули

    // Первое чтение для инициализации сглаженных значений
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

    ax_f = ax; ay_f = ay; az_f = az;            // Инициализируем сглаженные значения
    gx_f = gx; gy_f = gy; gz_f = gz;
    temp_f = tempC;
  } else {
    mpu_ok = false;
    Serial.println("MPU not found at 0x68");
    showMessageTFT("MPU ERR", ZAGOLOVOK, COL_TEXT3, COL_FON3);
    delay(500);
  }

  // ---------- Инициализация AHT20 ----------
  if (!aht.begin()) {                           // Если begin() вернул false – датчик не найден
    aht_ok = false;
    Serial.println("AHT20 not found");
    showMessageTFT("AHT20 ERR", ZAGOLOVOK, COL_TEXT4, COL_FON4);
    delay(500);
  } else {
    aht_ok = true;
    Serial.println("AHT20 OK");
    showMessageTFT("AHT20 OK", ZAGOLOVOK, COL_TEXT5, COL_FON5);
    delay(500);
  }

  // ---------- Инициализация BMP280 ----------
  if (!bmp.begin(0x76) && !bmp.begin(0x77)) {   // Пробуем оба стандартных I2C-адреса
    bmp_ok = false;
    Serial.println("BMP280 not found");
    showMessageTFT("BMP280 ERR", ZAGOLOVOK, COL_TEXT6, COL_FON6);
    delay(500);
  } else {
    bmp_ok = true;
    Serial.println("BMP280 OK");
    showMessageTFT("BMP280 OK", ZAGOLOVOK, COL_TEXT7, COL_FON7);
    delay(500);

    // Настройка режима измерений BMP280
    bmp.setSampling(
      Adafruit_BMP280::MODE_NORMAL,
      Adafruit_BMP280::SAMPLING_X2,
      Adafruit_BMP280::SAMPLING_X16,
      Adafruit_BMP280::FILTER_X16,
      Adafruit_BMP280::STANDBY_MS_500
    );
  }

  // Стартуем с экрана MPU
  displayMode = 0;
  drawMPUFrame();                 // Рисуем рамку для MPU

  lastScreenUpdate = millis();    // Время последнего обновления значений на экране
  lastSerialPrint  = millis();    // Время последнего вывода в Serial
}

// ============================================================================
//   LOOP() – основной цикл
// ============================================================================

void loop() {
  unsigned long now = millis();   // Текущее время (мс с момента включения)

  // ----- ЧТЕНИЕ MPU6050 -----
  if (mpu_ok) {
    uint8_t data[14];
    mpuRead(0x3B, data, 14);      // Читаем 14 байт с данными акселя, температуры и гироскопа

    int16_t ax_raw  = (int16_t)(data[0] << 8 | data[1]);
    int16_t ay_raw  = (int16_t)(data[2] << 8 | data[3]);
    int16_t az_raw  = (int16_t)(data[4] << 8 | data[5]);
    int16_t tempRaw = (int16_t)(data[6] << 8 | data[7]);
    int16_t gx_raw  = (int16_t)(data[8] << 8 | data[9]);
    int16_t gy_raw  = (int16_t)(data[10] << 8 | data[11]);
    int16_t gz_raw  = (int16_t)(data[12] << 8 | data[13]);

    // Переводим сырые значения в физические единицы и вычитаем оффсеты
    float ax = (ax_raw - ax_offset) / 16384.0f;
    float ay = (ay_raw - ay_offset) / 16384.0f;
    float az = (az_raw - az_offset) / 16384.0f;

    float gx = (gx_raw - gx_offset) / 131.0f;
    float gy = (gy_raw - gy_offset) / 131.0f;
    float gz = (gz_raw - gz_offset) / 131.0f;

    float tempC = (tempRaw / 340.0f) + 36.53f + temp_offset;

    // Сглаживание (экспоненциальное)
    ax_f = ax_f + alpha * (ax - ax_f);
    ay_f = ay_f + alpha * (ay - ay_f);
    az_f = az_f + alpha * (az - az_f);

    gx_f = gx_f + alpha * (gx - gx_f);
    gy_f = gy_f + alpha * (gy - gy_f);
    gz_f = gz_f + alpha * (gz - gz_f);

    temp_f = temp_f + alpha * (tempC - temp_f);
  }

  // ----- ЧТЕНИЕ AHT20/BMP280 -----
  if (aht_ok) {
    sensors_event_t humidity, temp;        // Структуры для получения данных
    if (aht.getEvent(&humidity, &temp)) {  // Успешное чтение с AHT20
      aht_tC = temp.temperature;           // Температура воздуха
      aht_hR = humidity.relative_humidity; // Влажность

      if (bmp_ok) {                        // Если BMP280 тоже доступен
        float pPa  = bmp.readPressure();   // Давление в Паскалях
        float p_hPa = pPa / 100.0;         // Перевод в гПа
        p_mmHg = p_hPa * 0.75006;          // Перевод в мм рт. ст.
      }
    } else if (!poterya_svyazi) {          // Ошибка чтения, и это первая ошибка
      aht_ok = false;                      // Считаем, что AHT20 недоступен
      bmp_ok = false;                      // И BMP280 тоже
      poterya_svyazi = true;               // Фиксируем потерю связи
      lastAhtRetry = now;                  // Запоминаем время сбоя
    }
  }

  // Периодические попытки переинициализировать AHT/BMP после сбоя
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

  // ----- ОБРАБОТКА КНОПКИ ПЕРЕКЛЮЧЕНИЯ РЕЖИМОВ -----
  bool buttonState = digitalRead(buttonPin);  // Читаем текущее состояние кнопки (HIGH/LOW)
  if (buttonState != lastButtonState &&       // Состояние изменилось
      (now - lastButtonChange) > DEBOUNCE_MS) { // Прошёл интервал антидребезга

    lastButtonChange = now;                   // Обновляем время последнего изменения
    lastButtonState = buttonState;            // Запоминаем новое состояние

    if (buttonState == LOW) {                 // Фиксируем нажатие (фронт HIGH -> LOW)
      displayMode = (displayMode + 1) % 2;    // Переключаем 0 -> 1 -> 0 -> ...

      if (displayMode == 0) {
        drawMPUFrame();                       // При режиме 0 рисуем каркас MPU
      } else {
        drawEnvFrame();                       // При режиме 1 рисуем каркас ENV
      }
    }
  }

  // ----- ПЕРИОДИЧЕСКИЙ ВЫВОД В SERIAL -----
  if (now - lastSerialPrint > serialPrintInterval) {  // Прошёл заданный интервал?
    lastSerialPrint = now;                            // Обновляем метку времени

    if (mpu_ok) {
      Serial.print("MPU ACC[g] X=");
      Serial.print(ax_f, 2);
      Serial.print(" Y=");
      Serial.print(ay_f, 2);
      Serial.print(" Z=");
      Serial.print(az_f, 2);

      Serial.println("  GYRO[dps] X=");
      Serial.print(gx_f, 1);
      Serial.print(" Y=");
      Serial.print(gy_f, 1);
      Serial.print(" Z=");
      Serial.print(gz_f, 1);

      Serial.println("  T_MPU=");
      Serial.print(temp_f, 1);
      Serial.print("C");
    } else {
      Serial.print("MPU ERR");
    }

    Serial.print("  |  ");

    if (aht_ok) {
      Serial.println("T_AHT=");
      Serial.print(aht_tC, 1);
      Serial.print("C  H=");
      Serial.print(aht_hR, 0);
      Serial.print("%  ");

      if (bmp_ok) {
        Serial.println("P=");
        Serial.print(p_mmHg, 0);
        Serial.print("mmHg");
      } else {
        Serial.print("P=N/A");
      }
    } else {
      Serial.print("AHT/BMP ERR");
    }
    Serial.println();                             // Переход на новую строку
  }

  // ----- ОБНОВЛЕНИЕ ЗНАЧЕНИЙ НА ЭКРАНЕ TFT (БЕЗ МЕРЦАНИЯ) -----
  if (now - lastScreenUpdate > SCREEN_UPDATE_INTERVAL) {
    lastScreenUpdate = now;                       // Обновляем время последнего обновления
    if (displayMode == 0) {                       // Если показываем MPU
      if (mpu_ok) updateMPUValues();              // Обновляем только числа MPU
    } else {                                      // Иначе показываем ENV
      updateEnvValues();                          // Обновляем только показания среды
    }
  }
}