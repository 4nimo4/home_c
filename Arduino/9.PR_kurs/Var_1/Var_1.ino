// ============================================================================
//   ДОМАШНЯЯ МЕТЕОСТАНЦИЯ ДЛЯ UNO R4 WIFI
//   AHT20 (T/H), BMP280 (P), MPU6050 (T_MPU), TFT ST7735 1.8"
//   Автоперезапуск всех датчиков при любой ошибке ENV/MPU
//   Виртуальный курс по интегралу крена
// ============================================================================

#include <Arduino.h>
#include <Wire.h>        // I2C (MPU6050, AHT20, BMP280)
#include <SPI.h>         // SPI (TFT-дисплей)

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>

#include <CBTimer.h>     // Таймер для мигания ERR / служебных задач

// ============================================================================
//  [CFG] ОБЩИЕ НАСТРОЙКИ И КОНСТАНТЫ
// ============================================================================

// --- TFT / текст ---
#define SET_ROTAC 2      // Поворот экрана от 0 до 3
#define ZAGOLOVOK 2      // Размер шрифта заголовка
#define OSN_TEXT  1      // Размер основного текста
#define DATCHIK   1      // Размер названия датчика

// Смещения по X для подписей и значений
#define SMESH_L1  2      // Подписи MPU (названия полей)
#define SMESH_L2  50     // Значения MPU
#define SMESH_L3  2      // Подписи ENV
#define SMESH_L4  42     // Значения ENV

// --- Дополнительные цвета RGB565 ---
#define ST77XX_NAVY      0x000F
#define ST77XX_DARKGREEN 0x03E0
#define ST77XX_DARKGREY  0x7BEF
#define ST77XX_LIGHTGREY 0xC618
#define ST77XX_MAROON    0x7800
#define ST77XX_PURPLE    0x780F
#define ST77XX_OLIVE     0x7BE0
#define ST77XX_BROWN     0x79E0
#define ST77XX_PINK      0xF81F

// --- Цвета сообщений ---
#define COL_TEXT1   ST77XX_WHITE
#define COL_FON1    ST77XX_PURPLE
#define COL_TEXT2   ST77XX_WHITE
#define COL_FON2    ST77XX_PURPLE
#define COL_TEXT3   ST77XX_RED
#define COL_FON3    ST77XX_PURPLE
#define COL_TEXT4   ST77XX_RED
#define COL_FON4    ST77XX_PURPLE
#define COL_TEXT5   ST77XX_WHITE
#define COL_FON5    ST77XX_PURPLE
#define COL_TEXT6   ST77XX_RED
#define COL_FON6    ST77XX_PURPLE
#define COL_TEXT7   ST77XX_WHITE
#define COL_FON7    ST77XX_PURPLE

// --- Цвета основных экранов ---
#define COL_TEXT8   ST77XX_GREEN   // текст каркаса MPU
#define COL_FON8    ST77XX_BLACK   // фон MPU
#define COL_TEXT9   ST77XX_YELLOW  // данные MPU

#define COL_TEXT10  ST77XX_GREEN   // текст ENV
#define COL_FON10   ST77XX_BLACK   // фон ENV
#define COL_TEXT11  ST77XX_YELLOW
#define COL_TEXT12  ST77XX_WHITE

// --- TFT ST7735 1.8" 128x160 ---
#define TFT_CS   10
#define TFT_DC    9
#define TFT_RST   8

// --- Питание датчиков ---
#define AHT_BMP_PWR_PIN   3   // D3 - транзистор питания PNP BC327 (AHT20/BMP280)
#define SV1_INDIC_PWR_PIN 4   // D4 - светодиод питания AHT20/BMP280
// --- Питание MPU6050 ---
#define MPU_PWR_PIN       5   // D5 - BC327 питания 5V на MPU6050 (LOW=вкл, HIGH=выкл)
#define SV2_MPU_INDIC_PIN 6   // D6 - светодиод питания MPU6050 (HIGH=вкл, LOW=выкл)

// --- Кнопка и тайминги UI ---
const int buttonPin = 2;
const unsigned long DEBOUNCE_MS = 50;

// Длинное нажатие для ручного "сброса нуля"
const unsigned long LONG_PRESS_MS = 1500;   // 1.5 секунды

bool   buttonPressed = false;
unsigned long buttonPressStart = 0;
bool   longPressHandled = false;

// --- Частоты обновления экранов ---
const unsigned long SCREEN_UPDATE_INTERVAL_METEO = 2000;
const unsigned long SCREEN_UPDATE_INTERVAL_HUD   = 200;

// --- Опрос ENV-датчиков ---
const unsigned long SENSOR_POLL_INTERVAL = 500;
unsigned long lastSensorPoll = 0;

// Отдельный интервал для ENV в HUD-режиме
const unsigned long HUD_SENSOR_POLL_INTERVAL = 2000; // 2 секунды
unsigned long lastHudEnvPoll = 0;

// --- Автовосстановление AHT/BMP (фактически через reinitAllSensors) ---
const unsigned long AHT_RETRY_INTERVAL = 5000;
unsigned long lastAhtRetry = 0;

// --- Попытки восстановления MPU ---
const unsigned long MPU_RETRY_INTERVAL = 3000;
unsigned long lastMpuRetry = 0;

// --- Вывод в Serial ---
unsigned long serialPrintInterval = 60000;

// --- Оценка погоды ---
const float P_LOW  = 710.0;
const float P_HIGH = 730.0;
const unsigned long TREND_INTERVAL = 10UL * 60UL * 1000UL;

// --- Фильтр MPU ---
const float alpha = 0.1f;

// --- Единый масштаб тангажа ---
const float PITCH_PX_PER_DEG = 2.0f;

// --- Опорное давление для высоты (hPa) ---
const float SEA_LEVEL_HPA = 1013.25f;

// ============================================================================
//   ГЛОБАЛЬНЫЕ ОБЪЕКТЫ
// ============================================================================
Adafruit_ST7735 tft(TFT_CS, TFT_DC, TFT_RST);
Adafruit_AHTX0   aht;
Adafruit_BMP280  bmp;
CBTimer          uiTimer;

// ============================================================================
//   ПЕРЕМЕННЫЕ СОСТОЯНИЯ
// ============================================================================

// --- MPU6050 ---
float roll_deg  = 0.0f;
float pitch_deg = 0.0f;
float heading_deg = 0.0f;      // виртуальный курс
float altZeroRef      = 0.0f;
bool  altZeroRefSet   = false;
float roll_for_heading_deg = 0.0f;
const float HEADING_ROLL_ALPHA = 0.2f;  // сглаживание крена для курса

// --- "виртуальный курс" по интегралу крена ---
float last_roll_for_heading = 0.0f;
bool  headingInitDone = false;

// --- Оффсеты горизонта (запоминаемый ноль при первом старте) ---
float roll_zero_deg  = 0.0f;
float pitch_zero_deg = 0.0f;
bool  horizonZeroSet = false;

// --- Флаг, что ноль курса уже зафиксирован (по первому старту) ---
bool  headingZeroSet = false;

// Последние нормированные значения (для логов при ошибках)
float ax_last = 0, ay_last = 0, az_last = 0;
float gx_last = 0, gy_last = 0, gz_last = 0;
float temp_last = 0;

unsigned long mpuLastInitTime = 0;
const unsigned long MPU_GRACE_MS = 1000;  // 1 секунда после init

const uint8_t MPU_ADDR = 0x68;
bool mpu_ok = false;

// Контроль ошибок MPU
bool mpu_error = false;
unsigned long mpuErrCount = 0;
unsigned long mpuNotFoundCount = 0;
uint8_t mpuReadErrorStreak = 0;
bool mpuHwWarnShown = false;

// Статистика ошибок MPU относительно движения
unsigned long mpuErrWhileStill  = 0;
unsigned long mpuErrWhileMoving = 0;

// Оффсеты
float ax_offset = 0, ay_offset = 0, az_offset = 0;
float gx_offset = 0, gy_offset = 0, gz_offset = 0;
float temp_offset = 0;

// Сглаженные значения
float ax_f = 0, ay_f = 0, az_f = 0;
float gx_f = 0, gy_f = 0, gz_f = 0;
float temp_f = 0;

// --- AHT20 + BMP280 ---
bool aht_ok = false;
bool bmp_ok = false;
bool poterya_svyazi = false;

// Текущие ENV-значения
float aht_tC = 0;
float aht_hR = 0;
float p_mmHg = 0;
float altitude_m = 0;
float altitudeOffset_m = 0;
float altitude_m_f = 0;
unsigned long lastBmpReadMs = 0;

// --- Режим стабилизации высоты после старта ---
unsigned long startTime = 0;
const unsigned long ALT_STAB_MS = 10000;

// Счётчики отказов
unsigned long ahtErrCount = 0;
unsigned long bmpErrCount = 0;

// --- Режим восстановления после общего сброса датчиков ---
bool recovering = false;
unsigned long recoverUntilMs = 0;
const unsigned long RECOVER_MS = 2000;   // 2 секунды «тиша» после reinitAllSensors

// --- Режимы экранов и кнопка ---
int displayMode = 0;
unsigned long lastScreenUpdate = 0;

bool lastButtonState = HIGH;
unsigned long lastButtonChange = 0;

// --- Вывод в SERIAL ---
unsigned long lastSerialPrint = 0;

// --- TREND давления ---
float p_trend_ref = 0;
unsigned long lastTrendTime = 0;

enum TrendState {
  T_STATE_FALL,
  T_STATE_STEADY,
  T_STATE_RISE,
  T_STATE_TUNKNOWN
};

TrendState pressureTrend = T_STATE_TUNKNOWN;

// --- CBTimer / мигание ---
volatile bool blinkErr = false;

enum AltState {
  ALT_IDLE,      // стоим, нет движения
  ALT_MOVING_UP,
  ALT_MOVING_DOWN
};

AltState altState = ALT_IDLE;

// "дискретная" высота, которую показываем на HUD
float altStep_m = 0.0f;

// последняя "зафиксированная" высота при остановке
float altLock_m = 0.0f;

// пороги для определения направления по баро
const float ALT_DIR_THRESH = 0.02f;   // 2 см

// ============================================================================
//   ПРОТОТИПЫ
// ============================================================================
enum PressureState {
  P_STATE_LOW,
  P_STATE_NORMAL,
  P_STATE_HIGH,
  P_STATE_UNKNOWN
};

enum ComfortState {
  C_STATE_DRY,
  C_STATE_COMFORT,
  C_STATE_HUMID,
  C_STATE_UNKNOWN
};

PressureState getPressureState(float p);
ComfortState getComfortState(float tC, float hR);
float         calcDewPoint(float tC, float hR);

void   mpuWrite(uint8_t reg, uint8_t data);
uint8_t mpuRead(uint8_t reg, uint8_t *buf, uint8_t len);
bool   calibrateMPU(unsigned int samples = 200);
bool   initMPU();

void showMessageTFT(const char *text, uint8_t size,
                    uint16_t color_text, uint16_t color_fon);
void drawMPUFrame();
void drawEnvFrame();
void drawHud();
void updateEnvValues();

void reinitMPUOnly(const char* reason);
void reinitEnvOnly(const char* reason);   // сейчас не используется
void reinitAllSensors(const char* reason);
void manualZeroReset();
void pollEnvOnce();   // один опрос AHT/BMP + обработка высоты

// ============================================================================
//   [F01] uiTimerCallback
// ============================================================================
void uiTimerCallback() {
  blinkErr = !blinkErr;
}

// ============================================================================
//   [F02] getPressureState
// ============================================================================
PressureState getPressureState(float p) {
  if (!bmp_ok) return P_STATE_UNKNOWN;
  if (p < P_LOW)  return P_STATE_LOW;
  if (p > P_HIGH) return P_STATE_HIGH;
  return P_STATE_NORMAL;
}

// ============================================================================
//   [F03] getComfortState
// ============================================================================
ComfortState getComfortState(float tC, float hR) {
  if (!aht_ok) return C_STATE_UNKNOWN;
  if (hR < 35.0) return C_STATE_DRY;
  if (hR > 65.0 && tC > 24.0) return C_STATE_HUMID;
  return C_STATE_COMFORT;
}

// ============================================================================
//   [F04] calcDewPoint
// ============================================================================
float calcDewPoint(float tC, float hR) {
  double a = 17.62;
  double b = 243.12;
  double gamma = (a * tC / (b + tC)) + log(hR / 100.0);
  double dp = (b * gamma) / (a - gamma);
  return (float)dp;
}

// ============================================================================
//   [F05] mpuWrite
// ============================================================================
void mpuWrite(uint8_t reg, uint8_t data) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  Wire.endTransmission();
}

// ============================================================================
//   [F06] mpuRead – с учётом I2C ошибок
// ============================================================================
uint8_t mpuRead(uint8_t reg, uint8_t *buf, uint8_t len) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  uint8_t err = Wire.endTransmission(false);
  if (err != 0) {
    unsigned long now = millis();
    Serial.print("MPU I2C endTransmission error=");
    Serial.print(err);
    Serial.print(" at ms=");
    Serial.println(now);

    // считаем это как ошибку чтения
    mpuReadErrorStreak++;
    mpuErrCount++;

    return 0;
  }

  uint8_t readCount = Wire.requestFrom((int)MPU_ADDR, (int)len);
  if (readCount != len) {
    unsigned long now = millis();
    Serial.print("MPU I2C short requestFrom: got ");
    Serial.print(readCount);
    Serial.print(" of ");
    Serial.print(len);
    Serial.print(" bytes at ms=");
    Serial.println(now);
  }

  for (uint8_t i = 0; i < readCount && i < len; i++) {
    buf[i] = Wire.read();
  }
  return readCount;
}

// ============================================================================
//   [F07] calibrateMPU – мягкая калибровка с проверкой чтений
// ============================================================================
bool calibrateMPU(unsigned int samples) {
  Serial.println("Calibrating MPU... Keep sensor still.");

  long ax_sum = 0, ay_sum = 0, az_sum = 0;
  long gx_sum = 0, gy_sum = 0, gz_sum = 0;
  long temp_sum = 0;
  unsigned int goodSamples = 0;

  for (unsigned int i = 0; i < samples; i++) {
    uint8_t data[14];
    uint8_t rc = mpuRead(0x3B, data, 14);
    if (rc < 14) {
      // плохое чтение – пропускаем
      delay(5);
      continue;
    }

    int16_t ax_raw  = (int16_t)(data[0] << 8 | data[1]);
    int16_t ay_raw  = (int16_t)(data[2] << 8 | data[3]);
    int16_t az_raw  = (int16_t)(data[4] << 8 | data[5]);
    int16_t tempRaw = (int16_t)(data[6] << 8 | data[7]);
    int16_t gx_raw  = (int16_t)(data[8] << 8 | data[9]);
    int16_t gy_raw  = (int16_t)(data[10] << 8 | data[11]);
    int16_t gz_raw  = (int16_t)(data[12] << 8 | data[13]);

    ax_sum   += ax_raw;
    ay_sum   += ay_raw;
    az_sum   += az_raw;
    gx_sum   += gx_raw;
    gy_sum   += gy_raw;
    gz_sum   += gz_raw;
    temp_sum += tempRaw;
    goodSamples++;

    delay(5);
  }

  if (goodSamples == 0) {
    Serial.println("MPU calibration FAILED: no valid samples");
    return false;
  }

  float ax_avg = (float)ax_sum / goodSamples;
  float ay_avg = (float)ay_sum / goodSamples;
  float az_avg = (float)az_sum / goodSamples;
  float gx_avg = (float)gx_sum / goodSamples;
  float gy_avg = (float)gy_sum / goodSamples;
  float gz_avg = (float)gz_sum / goodSamples;
  (void)temp_sum;

  ax_offset   = ax_avg;
  ay_offset   = ay_avg;
  az_offset   = az_avg - 16384;
  gx_offset   = gx_avg;
  gy_offset   = gy_avg;
  gz_offset   = gz_avg;
  temp_offset = 0.0f;

  Serial.print("MPU calibration done. goodSamples=");
  Serial.println(goodSamples);
  return true;
}

// ============================================================================
//   [F08] initMPU – с проверкой WHO_AM_I и калибровкой
// ============================================================================
bool initMPU() {
  // [F08.1] Проверяем наличие устройства на шине
  Wire.beginTransmission(MPU_ADDR);
  uint8_t err = Wire.endTransmission();
  if (err != 0) {
    mpu_ok = false;
    mpuNotFoundCount++;
    Serial.print("MPU not found at 0x68, I2C err=");
    Serial.print(err);
    Serial.print("  mpuNotFoundCount=");
    Serial.println(mpuNotFoundCount);

    if (!mpuHwWarnShown) {
      mpuHwWarnShown = true;
      Serial.println("MPU_HW_WARN: device not responding on I2C. Check VCC/GND/SDA/SCL and power transistor.");
    }
    return false;
  }

  // [F08.2] WHO_AM_I
  uint8_t whoami = 0;
  uint8_t rcWho = mpuRead(0x75, &whoami, 1);
  if (rcWho < 1) {
    mpu_ok = false;
    Serial.println("MPU WHO_AM_I read failed");

    if (!mpuHwWarnShown) {
      mpuHwWarnShown = true;
      Serial.println("MPU_HW_WARN: WHO_AM_I read failed. Check wiring, pull-ups, and MPU power (BC327).");
    }
    return false;
  }

  Serial.print("MPU WHO_AM_I = 0x");
  Serial.println(whoami, HEX);

  if (whoami != 0x68 && whoami != 0x70) {
    Serial.println("MPU WHO_AM_I unexpected, но продолжаем init");
  }

  // [F08.3] Настройка диапазонов и будим
  mpuWrite(0x6B, 0x00);   // будим
  delay(100);
  mpuWrite(0x1C, 0x00);   // ±2g
  mpuWrite(0x1B, 0x00);   // ±250 dps
  delay(50);

  // Сбрасываем оффсеты
  ax_offset = ay_offset = az_offset = 0;
  gx_offset = gy_offset = gz_offset = 0;
  temp_offset = 0.0f;

  // [F08.4] Калибровка
  if (!calibrateMPU(80)) {
    mpu_ok = false;
    Serial.println("initMPU: calibration failed");
    return false;
  }

  // [F08.5] Контрольное чтение
  uint8_t data[14];
  uint8_t rc = mpuRead(0x3B, data, 14);
  if (rc < 14) {
    mpu_ok = false;
    Serial.println("MPU initial read failed");
    return false;
  }

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

  // Сохраняем как "последние измерения" для логов
  ax_last   = ax;
  ay_last   = ay;
  az_last   = az;
  gx_last   = gx;
  gy_last   = gy;
  gz_last   = gz;
  temp_last = tempC;

  // --- [F08.6] Вычисляем "сырые" углы для фиксации нуля горизонта ---
  float roll_rad_init  = atan2(ay_f, az_f);
  float pitch_rad_init = atan2(-ax_f, sqrt(ay_f * ay_f + az_f * az_f));

  float roll_raw_deg_init  = roll_rad_init  * 180.0f / PI;
  float pitch_raw_deg_init = pitch_rad_init * 180.0f / PI;

  // Если ноль горизонта ещё не зафиксирован – фиксируем ТЕКУЩЕЕ положение как ноль
  if (!horizonZeroSet) {
    roll_zero_deg  = roll_raw_deg_init;
    pitch_zero_deg = pitch_raw_deg_init;
    horizonZeroSet = true;
    Serial.print("HORIZON zero captured: roll_zero=");
    Serial.print(roll_zero_deg, 1);
    Serial.print(" pitch_zero=");
    Serial.println(pitch_zero_deg, 1);
  }

  // Выставляем текущие углы с учётом нуля горизонта
  if (horizonZeroSet) {
    roll_deg  = roll_raw_deg_init  - roll_zero_deg;
    pitch_deg = pitch_raw_deg_init - pitch_zero_deg;
  } else {
    roll_deg  = roll_raw_deg_init;
    pitch_deg = pitch_raw_deg_init;
  }

  mpu_ok    = true;
  mpu_error = false;

  Serial.println("MPU initialized & calibrated.");
  mpuLastInitTime = millis();
  mpuHwWarnShown  = false;

  // --- [F08.7] Фиксация нуля курса: ТОЛЬКО при ПЕРВОМ успешном init ---
  if (!headingZeroSet) {
    heading_deg           = 0.0f;        // курс = 0
    last_roll_for_heading = roll_deg;    // текущий roll (уже относительно нуля горизонта)
    headingInitDone       = true;
    headingZeroSet        = true;
    Serial.print("HEADING zero captured at roll=");
    Serial.println(roll_deg, 1);
  } else {
    // при последующих init'ах НЕ переопределяем ноль, только разрешаем интеграцию
    headingInitDone = true;
    // heading_deg и last_roll_for_heading НЕ трогаем, чтобы курс продолжал считаться от первичного нуля
  }

  return true;
}

// ============================================================================
//   [F09] showMessageTFT
// ============================================================================
void showMessageTFT(const char *text, uint8_t size,
                    uint16_t color_text, uint16_t color_fon) {
  tft.fillScreen(color_fon);
  tft.setTextWrap(false);
  tft.setTextColor(color_text, color_fon);
  tft.setTextSize(size);

  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  int16_t x = (tft.width()  - w) / 2;
  int16_t y = (tft.height() - h) / 2;

  tft.setCursor(x, y);
  tft.print(text);
}

// ============================================================================
//   [F10] drawMPUFrame
// ============================================================================
void drawMPUFrame() {
  tft.fillScreen(COL_FON1);
  tft.setTextWrap(false);
}

// ============================================================================
//   [F11] drawEnvFrame
// ============================================================================
void drawEnvFrame() {
  tft.fillScreen(COL_FON1);
  tft.setTextWrap(false);

  tft.setTextSize(1);
  tft.setTextColor(COL_TEXT12, COL_FON1);

  const char *title = "WEATHER STATION";
  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds(title, 0, 0, &x1, &y1, &w, &h);
  int16_t x = (tft.width()  - w) / 2 - 10;
  int16_t y = 4;
  tft.setCursor(x, y);
  tft.print(title);

  tft.setTextColor(COL_TEXT10, COL_FON1);
  tft.setTextSize(OSN_TEXT);

  tft.setCursor(SMESH_L3, 20);  tft.print("Temp:");
  tft.setCursor(SMESH_L3, 35);  tft.print("Hydro:");
  tft.setCursor(SMESH_L3, 50);  tft.print("Dew:");
  tft.setCursor(SMESH_L3, 65);  tft.print("Psc:");
  tft.setCursor(SMESH_L3, 80);  tft.print("Trend:");
  tft.setCursor(SMESH_L3, 95);  tft.print("T_MPU:");

  tft.setCursor(SMESH_L3, 120); tft.print("AHT20:");
  tft.setCursor(SMESH_L3, 132); tft.print("BMP280:");
  tft.setCursor(SMESH_L3, 144); tft.print("MPU:");
}

// ============================================================================
//   [F12] drawHud – основной HUD по MPU6050
// ============================================================================
void drawHud() {
  const int16_t W = tft.width();
  const int16_t H = tft.height();

  tft.fillScreen(COL_FON1);
  uint16_t hudColor = ST77XX_GREEN;

  int16_t cx = W / 2 - 16;
  int16_t cy = H / 2 + 4;

  float pitch_vis    = -pitch_deg;
  float roll_rad     = roll_deg * PI / 180.0f;

  // [F12.1] Основная линия горизонта
  int16_t pitchOffsetY = (int16_t)(pitch_vis * PITCH_PX_PER_DEG);
  int16_t hy = cy + pitchOffsetY;

  const int16_t horizMarginLeft   = 6;
  const int16_t horizToPitchScale = 10;
  const int16_t scaleX = W - 20;

  int16_t hx1 = horizMarginLeft;
  int16_t hx2 = scaleX - horizToPitchScale;
  tft.drawLine(hx1, hy, hx2, hy, hudColor);

  // [F12.2] Дуга крена
  const int16_t br_r  = 35;
  const int16_t br_cx = cx;
  const int16_t br_cy = cy + 0;

  for (int a = -90; a <= 90; a += 30) {
    float ang = (a + 90.0f) * PI / 180.0f;

    float cx1 = br_cx + br_r * cos(ang);
    float cy1 = br_cy + br_r * sin(ang);

    float vx = cos(ang);
    float vy = sin(ang);

    if (a == -90 || a == 90) {
      const float tipLen  = 2.0f;
      const float wingLen = 6.0f;
      const float wingAng = 25.0f * PI / 180.0f;

      float bx = cx1;
      float by = cy1;

      float ix = -vx;
      float iy = -vy;

      float tx = bx + ix * tipLen;
      float ty = by + iy * tipLen;

      float cosW = cos(wingAng);
      float sinW = sin(wingAng);

      float lxv = vx * cosW - vy * sinW;
      float lyv = vx * sinW + vy * cosW;

      float rxv = vx * cosW + vy * sinW;
      float ryv = -vx * sinW + vy * cosW;

      int16_t tipX = (int16_t)tx;
      int16_t tipY = (int16_t)ty;

      int16_t lX = (int16_t)(tx + lxv * wingLen);
      int16_t lY = (int16_t)(ty + lyv * wingLen);
      int16_t rX = (int16_t)(tx + rxv * wingLen);
      int16_t rY = (int16_t)(ty + ryv * wingLen);

      tft.drawLine(tipX, tipY, lX, lY, hudColor);
      tft.drawLine(tipX, tipY, rX, rY, hudColor);

    } else {
      int len = (a == 0) ? 7 : 5;

      int16_t x1 = (int16_t)cx1;
      int16_t y1 = (int16_t)cy1;
      int16_t x2 = br_cx + (int16_t)((br_r + len) * cos(ang));
      int16_t y2 = br_cy + (int16_t)((br_r + len) * sin(ang));

      tft.drawLine(x1, y1, x2, y2, hudColor);
    }
  }

  // [F12.3] Вторая линия горизонта
  const int16_t horizOffset = -10;
  int16_t hy2 = hy + horizOffset;

  float cx2 = W / 2.0f;
  float cy2 = (float)hy2;

  float roll2 = -roll_rad;

  float lineLen2 = W;
  float dx2 = cos(roll2) * lineLen2;
  float dy2 = sin(roll2) * lineLen2;

  int16_t x1_2 = (int16_t)(cx2 - dx2);
  int16_t y1_2 = (int16_t)(cy2 - dy2);
  int16_t x2_2 = (int16_t)(cx2 + dx2);
  int16_t y2_2 = (int16_t)(cy2 + dy2);

  tft.drawLine(x1_2, y1_2, x2_2, y2_2, hudColor);

  // [F12.4] Самолёт
  auto rotX = [&](float x, float y) -> int16_t {
    return (int16_t)(x * cos(roll_rad) - y * sin(roll_rad)) + cx;
  };
  auto rotY = [&](float x, float y) -> int16_t {
    return (int16_t)(x * sin(roll_rad) + y * cos(roll_rad)) + cy;
  };

  const float wingHalfLen = 26.0f;
  const float centerGap   = 6.0f;
  const float toothDepth  = 4.0f;

  float leftMid = (-wingHalfLen + (-centerGap)) * 0.5f;

  tft.drawLine(
    rotX(-wingHalfLen, 0), rotY(-wingHalfLen, 0),
    rotX(leftMid - 2,  0), rotY(leftMid - 2,  0),
    hudColor
  );
  tft.drawLine(
    rotX(leftMid - 2,  0),         rotY(leftMid - 2,  0),
    rotX(leftMid,     toothDepth), rotY(leftMid,     toothDepth),
    hudColor
  );
  tft.drawLine(
    rotX(leftMid,     toothDepth), rotY(leftMid,     toothDepth),
    rotX(leftMid + 2, 0),          rotY(leftMid + 2, 0),
    hudColor
  );
  tft.drawLine(
    rotX(leftMid + 2, 0), rotY(leftMid + 2, 0),
    rotX(-centerGap,  0), rotY(-centerGap,  0),
    hudColor
  );

  float rightMid = (wingHalfLen + centerGap) * 0.5f;

  tft.drawLine(
    rotX( wingHalfLen, 0), rotY( wingHalfLen, 0),
    rotX( rightMid + 2, 0), rotY( rightMid + 2, 0),
    hudColor
  );
  tft.drawLine(
    rotX( rightMid + 2, 0), rotY( rightMid + 2, 0),
    rotX( rightMid,     toothDepth), rotY( rightMid,     toothDepth),
    hudColor
  );
  tft.drawLine(
    rotX( rightMid,     toothDepth), rotY( rightMid,     toothDepth),
    rotX( rightMid - 2, 0),          rotY( rightMid - 2, 0),
    hudColor
  );
  tft.drawLine(
    rotX( rightMid - 2, 0), rotY( rightMid - 2, 0),
    rotX( centerGap,    0), rotY( centerGap,    0),
    hudColor
  );

  const float keelGap   = centerGap;
  float keelTopY    = -20.0f;
  float keelBottomY = -keelGap;

  tft.drawLine(
    rotX(0, keelTopY),    rotY(0, keelTopY),
    rotX(0, keelBottomY), rotY(0, keelBottomY),
    hudColor
  );

  // [F12.5] Правая шкала тангажа
  const float  pixelsPerDeg = 2.8f;
  const int    majorLen     = 6;
  const int    minorLen     = 3;
  const int    maxDeg       = 180;

  const int16_t winCenterY = cy;
  const int16_t winHalfH   = 40;
  const int16_t winTopY    = winCenterY - winHalfH;
  const int16_t winBotY    = winCenterY + winHalfH;

  tft.setTextColor(hudColor, COL_FON1);
  tft.setTextSize(1);

  for (int mark = -maxDeg; mark <= maxDeg; mark += 10) {
    float dy = (pitch_vis - (float)mark) * pixelsPerDeg;
    int16_t my = (int16_t)(winCenterY + dy);

    if (my < winTopY || my > winBotY) continue;

    int16_t mx1 = scaleX - majorLen / 2;
    int16_t mx2 = scaleX + majorLen / 2;
    tft.drawLine(mx1, my, mx2, my, hudColor);

    const int16_t rightMargin   = 4;
    const int16_t gapFromStick  = 4;

    int16_t tx = mx2 + gapFromStick;
    int16_t maxTx = W - rightMargin - 10;
    if (tx > maxTx) tx = maxTx;

    int16_t ty = my - 3;
    if (ty < winTopY)     ty = winTopY;
    if (ty > winBotY - 8) ty = winBotY - 8;

    tft.setCursor(tx, ty);

    int val = (mark == 0) ? 0 : abs(mark);
    tft.print(val);
  }

  for (int mark = -maxDeg; mark <= maxDeg; mark += 5) {
    if (mark % 10 == 0) continue;

    float dy = (pitch_vis - (float)mark) * pixelsPerDeg;
    int16_t my = (int16_t)(winCenterY + dy);

    if (my < winTopY || my > winBotY) continue;

    int16_t mx1 = scaleX - minorLen / 2;
    int16_t mx2 = scaleX + minorLen / 2;
    tft.drawLine(mx1, my, mx2, my, hudColor);
  }

  // [F12.6] Окно высоты (используем altStep_m)
  {
    const int16_t altBoxWidth  = 34;
    const int16_t altBoxHeight = 16;

    const int16_t rightMargin = 2;
    int16_t altBoxX2 = W - rightMargin;
    int16_t altBoxX1 = altBoxX2 - altBoxWidth;

    int16_t altBoxY = 4;
    if (altBoxY + altBoxHeight > H) {
      altBoxY = H - altBoxHeight - 1;
    }

    tft.drawRect(altBoxX1, altBoxY, altBoxWidth, altBoxHeight, hudColor);

    tft.setTextColor(hudColor, COL_FON1);
    tft.setTextSize(1);

    float altPrint_m = altStep_m;

    char valBuf[8];
    snprintf(valBuf, sizeof(valBuf), "%.1f", altPrint_m);

    int16_t bx, by;
    uint16_t bw, bh;
    tft.getTextBounds(valBuf, 0, 0, &bx, &by, &bw, &bh);

    int16_t valTextX = altBoxX2 - 2 - bw;
    int16_t valTextY = altBoxY + (altBoxHeight - bh) / 2;

    if (valTextX < altBoxX1 + 1) valTextX = altBoxX1 + 1;

    tft.setCursor(valTextX, valTextY);
    tft.print(valBuf);

    const char *labelAlt = "Alt";

    int16_t l1x, l1y;
    uint16_t l1w, l1h;
    tft.getTextBounds(labelAlt, 0, 0, &l1x, &l1y, &l1w, &l1h);

    int16_t labelAltX = altBoxX1 + (altBoxWidth - l1w) / 2;
    int16_t labelAltY = altBoxY + altBoxHeight + 3;
    if (labelAltY > H - l1h - 1) labelAltY = H - l1h - 1;

    tft.setCursor(labelAltX, labelAltY);
    tft.print(labelAlt);
  }

  // [F12.7] Верхняя шкала курса: диапазон -180..+180 вокруг 0
  float hdg = heading_deg;

  // нормализуем в -180..+180
  while (hdg >  180.0f) hdg -= 360.0f;
  while (hdg <= -180.0f) hdg += 360.0f;

  int16_t hs_y  = 16;          // ЧУТЬ НИЖЕ, чем было (было 14)
  int16_t hs_cx = cx;
  int16_t halfLenH2 = 28;
  int16_t hs_left  = hs_cx - halfLenH2;
  int16_t hs_right = hs_cx + halfLenH2;
  if (hs_left < 4) hs_left = 4;
  if (hs_right > W - 4) hs_right = W - 4;

  // Линия шкалы
  tft.drawLine(hs_left, hs_y, hs_right, hs_y, hudColor);

  // Треугольный указатель курса – немного ниже шкалы, строго по центру
  int16_t tri_h = 6;
  int16_t tri_w = 8;

  int16_t vx0 = hs_cx;
  int16_t vy0 = hs_y + 2;      // было на самой линии, сдвинули на 2px ниже
  int16_t bx_y = vy0 + tri_h;
  int16_t bx_l = vx0 - tri_w / 2;
  int16_t bx_r = vx0 + tri_w / 2;

  tft.drawLine(vx0, vy0, bx_l, bx_y, hudColor);
  tft.drawLine(vx0, vy0, bx_r, bx_y, hudColor);
  tft.drawLine(bx_l, bx_y, bx_r, bx_y, hudColor);

  // Параметры шкалы
  int   stepDeg   = 5;
  float pxPerDegH = 2.3f;
  int   rangeDeg  = 20;        // ±20° вокруг текущего курса

  // Центр шкалы – округляем hdg, чтобы сетка не уплывала
  int centerDeg = (int)round(hdg);

  int startDeg  = centerDeg - rangeDeg;
  int endDeg    = centerDeg + rangeDeg;

  tft.setTextSize(1);
  tft.setTextColor(hudColor, COL_FON1);

  // Сетка делений и подписей (как было, но относительно centerDeg)
  for (int mark = startDeg; mark <= endDeg; mark += stepDeg) {
    int d = mark - centerDeg;
    int16_t x = hs_cx + (int16_t)(d * pxPerDegH);
    if (x < hs_left || x > hs_right) continue;

    int16_t len = 4;
    tft.drawLine(x, hs_y, x, hs_y - len, hudColor);

    // подписи через 10 градусов
    if (((mark / stepDeg) % 2) == 0) {
      int val = mark;

      // отображаем в диапазоне -180..+180
      while (val >  180) val -= 360;
      while (val <= -180) val += 360;

      // пропустим 0 здесь, чтобы нарисовать его отдельно строго по центру
      if (val == 0) continue;

      int16_t tx = x - 8;
      int16_t ty = hs_y - len - 8;
      if (tx < 0) tx = 0;
      if (tx > W - 16) tx = W - 16;
      if (ty < 0) ty = 0;

      tft.setCursor(tx, ty);
      tft.print(val);
    }
  }

  // Специально рисуем "0" РОВНО под вершиной треугольника,
  // независимо от сетки делений, чтобы он всегда был по центру
  {
    int16_t len = 4;
    int16_t ty0 = hs_y - len - 8;
    if (ty0 < 0) ty0 = 0;

    // лёгкая подгонка по X под ширину шрифта "0"
    int16_t tx0 = hs_cx - 3;   // если будет чуть влево/вправо – меняем -3 на -2/-4

    if (tx0 < 0) tx0 = 0;
    if (tx0 > W - 8) tx0 = W - 8;

    tft.setCursor(tx0, ty0);
    tft.print(0);
  }

  // [F12.8] Численный крен – левый нижний
  tft.setTextSize(1);
  tft.setTextColor(hudColor, COL_FON1);

  const int16_t rollTextMarginX = 20;
  const int16_t rollTextMarginY = 20;

  int16_t rollTextX = rollTextMarginX;
  int16_t rollTextY = H - rollTextMarginY;

  tft.setCursor(rollTextX, rollTextY);
  tft.print(roll_deg, 1);

  // [F12.9] Численный тангаж – правый нижний
  float pitch_for_print = pitch_vis;

  int16_t w = W;
  int16_t h = H;

  const int16_t pitchTextMarginX = 20;
  const int16_t pitchTextMarginY = 20;

  int16_t pitchTextX = w - pitchTextMarginX - 26;
  int16_t pitchTextY = h - pitchTextMarginY;
  if (pitchTextX < 0) pitchTextX = 0;

  tft.setCursor(pitchTextX, pitchTextY);

  if (pitch_for_print > 0.05f) {
    tft.print("+");
    tft.print(pitch_for_print, 1);
  } else if (pitch_for_print < -0.05f) {
    tft.print(pitch_for_print, 1);
  } else {
    tft.print("0.0");
  }
}

// ============================================================================
//   [F13] updateEnvValues – обновление ENV-значений на экране
// ============================================================================
void updateEnvValues() {
  tft.setTextWrap(false);
  tft.setTextColor(COL_TEXT11, COL_FON1);
  tft.setTextSize(OSN_TEXT);

  tft.setCursor(SMESH_L4, 20);
  tft.print("           ");
  tft.setCursor(SMESH_L4, 20);
  if (aht_ok) {
    tft.print(aht_tC, 1); tft.print(" C");
  } else {
    tft.print("ERR");
  }

  tft.setCursor(SMESH_L4, 35);
  tft.print("           ");
  tft.setCursor(SMESH_L4, 35);
  if (aht_ok) {
    tft.print(aht_hR, 0); tft.print(" %");
  } else {
    tft.print("ERR");
  }

  tft.setCursor(SMESH_L4, 50);
  tft.print("           ");
  tft.setCursor(SMESH_L4, 50);
  if (aht_ok) {
    float dew = calcDewPoint(aht_tC, aht_hR);
    tft.print(dew, 1); tft.print(" C");
  } else {
    tft.print("ERR");
  }

  tft.setCursor(SMESH_L3 + 90, 35);
  tft.print("        ");
  tft.setCursor(SMESH_L3 + 90, 35);
  if (aht_ok) {
    ComfortState cs = getComfortState(aht_tC, aht_hR);
    if (cs == C_STATE_DRY)         tft.print("DRY");
    else if (cs == C_STATE_HUMID)  tft.print("HUMID");
    else                           tft.print("OK");
  } else {
    tft.print("N/A");
  }

  tft.setCursor(SMESH_L4, 65);
  tft.print("           ");
  tft.setCursor(SMESH_L4, 65);
  if (bmp_ok) {
    tft.print(p_mmHg, 0); tft.print("mmH");
  } else {
    tft.print("ERR");
  }

  tft.setCursor(SMESH_L3 + 90, 65);
  tft.print("        ");
  tft.setCursor(SMESH_L3 + 90, 65);
  if (bmp_ok) {
    PressureState ps = getPressureState(p_mmHg);
    if (ps == P_STATE_LOW)       tft.print("LOW");
    else if (ps == P_STATE_HIGH) tft.print("HIGH");
    else                         tft.print("NORM");
  } else {
    tft.print("N/A");
  }

  tft.setCursor(SMESH_L4, 80);
  tft.print("           ");
  tft.setCursor(SMESH_L4, 80);
  if (bmp_ok) {
    if (pressureTrend == T_STATE_RISE)        tft.print("UP");
    else if (pressureTrend == T_STATE_FALL)   tft.print("DOWN");
    else if (pressureTrend == T_STATE_STEADY) tft.print("STEADY");
    else                                      tft.print("N/A");
  } else {
    tft.print("N/A");
  }

  tft.setCursor(SMESH_L4, 95);
  tft.print("           ");
  tft.setCursor(SMESH_L4, 95);
  if (mpu_ok) {
    tft.print(temp_f, 1); tft.print(" C");
  } else {
    tft.print("ERR");
  }

  tft.setCursor(SMESH_L3 + 40, 120);
  tft.print("                    ");
  tft.setCursor(SMESH_L3 + 40, 120);
  if (aht_ok) tft.print("OK ");
  else        tft.print("ERR");
  tft.print(" (mERR="); tft.print(ahtErrCount); tft.print(")");

  tft.setCursor(SMESH_L3 + 40, 132);
  tft.print("                    ");
  tft.setCursor(SMESH_L3 + 40, 132);
  if (bmp_ok) tft.print("OK ");
  else        tft.print("ERR");
  tft.print(" (mERR="); tft.print(bmpErrCount); tft.print(")");

  // --- Статус MPU6050 ---
  tft.setCursor(SMESH_L3 + 40, 144);
  tft.print("                    ");
  tft.setCursor(SMESH_L3 + 40, 144);
  if (mpu_ok) tft.print("OK ");
  else        tft.print("ERR");
  tft.print(" (mERR="); tft.print(mpuErrCount); tft.print(")");
}

// ============================================================================
//   [F13.1] pollEnvOnce – один опрос AHT/BMP + обработка высоты
// ============================================================================
void pollEnvOnce() {
  sensors_event_t humidity, temp;
  if (aht.getEvent(&humidity, &temp)) {
    aht_tC = temp.temperature;
    aht_hR = humidity.relative_humidity;

    if (bmp_ok) {
      unsigned long t_bmp = millis();
      lastBmpReadMs = t_bmp;
      Serial.print("BMP read at ms=");
      Serial.println(t_bmp);

      float pPa   = bmp.readPressure();
      float p_hPa = pPa / 100.0f;
      p_mmHg      = p_hPa * 0.75006f;

      float alt_abs = bmp.readAltitude(SEA_LEVEL_HPA);
      altitude_m    = alt_abs - altitudeOffset_m;

      // [F13.1.1] Сглаживание баро
      bool inStab = (millis() - startTime) < ALT_STAB_MS;
      const float alpha_bar = 0.5f;   // или твоё новое значение, если будешь менять

      if (inStab) {
        altitude_m_f  = 0.0f;
        altZeroRefSet = false;
        altStep_m     = 0.0f;
        altLock_m     = 0.0f;
        altState      = ALT_IDLE;
      } else {
        if (!altZeroRefSet) {
          altZeroRef    = altitude_m;  // ноль берём один раз
          altZeroRefSet = true;
          altitude_m_f  = altitude_m;
          altStep_m     = 0.0f;
          altLock_m     = 0.0f;
          altState      = ALT_IDLE;
          Serial.print("Alt zero ref captured: ");
          Serial.println(altZeroRef);
        }

        altitude_m_f = altitude_m_f + alpha_bar * (altitude_m - altitude_m_f);
      }

      // [F13.1.2] "ЛЕСТНИЦА" ВЫСОТЫ ТОЛЬКО ПО БАРОМЕТРУ
      if (altZeroRefSet && !inStab) {
        float alt_rel = altitude_m_f - altZeroRef;

        static float last_alt_rel = 0.0f;
        float dAlt = alt_rel - last_alt_rel;
        last_alt_rel = alt_rel;

        bool baroUp   = (dAlt >  ALT_DIR_THRESH);
        bool baroDown = (dAlt < -ALT_DIR_THRESH);

        switch (altState) {
          case ALT_IDLE:
            if (baroUp) {
              altState = ALT_MOVING_UP;
            } else if (baroDown) {
              altState = ALT_MOVING_DOWN;
            }
            break;

          case ALT_MOVING_UP:
            if (baroUp) {
              altStep_m += dAlt;
            } else if (!baroUp && !baroDown) {
              altLock_m = altStep_m;
              altState  = ALT_IDLE;
            } else if (baroDown) {
              altState = ALT_MOVING_DOWN;
            }
            break;

          case ALT_MOVING_DOWN:
            if (baroDown) {
              altStep_m += dAlt;
              if (altStep_m < 0.0f) altStep_m = 0.0f;
            } else if (!baroUp && !baroDown) {
              if (altStep_m < 0.1f) altStep_m = 0.0f;
              altLock_m = altStep_m;
              altState  = ALT_IDLE;
            } else if (baroUp) {
              altState = ALT_MOVING_UP;
            }
            break;
        }

        if (altStep_m < 0.0f) altStep_m = 0.0f;
        if (altStep_m > 5.0f) altStep_m = 5.0f;

        float alt_rel_now = altitude_m_f - altZeroRef;
        if (fabs(alt_rel_now) < 0.30f && altState == ALT_IDLE) {
          altStep_m = 0.0f;
        }
      }

      // [F13.1.3] Отладка высоты (локальная)
      static unsigned long lastAltDebug = 0;
      if (millis() - lastAltDebug > 500) {
        lastAltDebug = millis();
        Serial.print("ALT raw=");
        Serial.print(altitude_m, 2);
        Serial.print("  alt_f=");
        Serial.print(altitude_m_f, 2);
        Serial.print("  altZero=");
        Serial.print(altZeroRef, 2);
        Serial.print("  altState=");
        Serial.print((int)altState);
        Serial.print("  altStep=");
        Serial.println(altStep_m, 2);
      }

    } else {
      bmp_ok = false;
      bmpErrCount++;
      Serial.println("BMP suddenly ERR -> reinitAllSensors()");
      reinitAllSensors("BMP sudden ERR");
    }

  } else {
    // Ошибка AHT – сразу перезапуск всех датчиков
    aht_ok = false;
    ahtErrCount++;
    Serial.println("AHT getEvent() FAILED -> reinitAllSensors()");
    reinitAllSensors("AHT error");
  }
} 

// ============================================================================
//   [F14] reinitMPUOnly – теперь просто вызывает общий сброс
// ============================================================================
void reinitMPUOnly(const char* reason) {
  Serial.print("reinitMPUOnly() -> reinitAllSensors(), reason: ");
  Serial.println(reason);
  reinitAllSensors(reason);
}

// ============================================================================
//   [F15] reinitEnvOnly – сейчас НЕ используется (оставляем задел)
// ============================================================================
void reinitEnvOnly(const char* reason) {
  Serial.print("Reinit ENV only (AHT/BMP), reason: ");
  Serial.println(reason);

  digitalWrite(AHT_BMP_PWR_PIN, HIGH);
  digitalWrite(SV1_INDIC_PWR_PIN, LOW);
  delay(300);
  digitalWrite(AHT_BMP_PWR_PIN, LOW);
  digitalWrite(SV1_INDIC_PWR_PIN, HIGH);
  delay(200);

  bool aht_ok_new = aht.begin();
  aht_ok = aht_ok_new;
  if (!aht_ok_new) ahtErrCount++;

  bool bmp_ok_new = (bmp.begin(0x76) || bmp.begin(0x77));
  bmp_ok = bmp_ok_new;
  if (!bmp_ok_new) bmpErrCount++;

  Serial.print("ENV re-init results: AHT=");
  Serial.print(aht_ok_new ? "OK" : "ERR");
  Serial.print(" BMP=");
  Serial.println(bmp_ok_new ? "OK" : "ERR");
}

// ============================================================================
//   [F16] reinitAllSensors – общий ПОЛНЫЙ сброс AHT/BMP + MPU
// ============================================================================
void reinitAllSensors(const char* reason) {
  Serial.print("Reinit ALL sensors (ENV + MPU), reason: ");
  Serial.println(reason);

  // [F16.1] Выключаем ВСЁ питание датчиков одновременным действием
  digitalWrite(AHT_BMP_PWR_PIN, HIGH);    // OFF AHT/BMP
  digitalWrite(MPU_PWR_PIN,     HIGH);    // OFF MPU
  digitalWrite(SV1_INDIC_PWR_PIN, LOW);   // LED ENV OFF
  digitalWrite(SV2_MPU_INDIC_PIN, LOW);   // LED MPU OFF
  delay(250);

  // [F16.2] Включаем ВСЁ питание датчиков одновременным действием
  digitalWrite(AHT_BMP_PWR_PIN, LOW);     // ON AHT/BMP
  digitalWrite(MPU_PWR_PIN,     LOW);     // ON MPU
  digitalWrite(SV1_INDIC_PWR_PIN, HIGH);  // LED ENV ON
  digitalWrite(SV2_MPU_INDIC_PIN, HIGH);  // LED MPU ON
  delay(300);

  // [F16.3] Инициализация ENV
  Serial.println("Reinit ALL: calling aht.begin() / bmp.begin()");

  bool aht_ok_new = aht.begin();
  aht_ok = aht_ok_new;
  if (!aht_ok_new) ahtErrCount++;

  bool bmp_ok_new = (bmp.begin(0x76) || bmp.begin(0x77));
  bmp_ok = bmp_ok_new;
  if (!bmp_ok_new) bmpErrCount++;

  Serial.print("Reinit ALL: AHT=");
  Serial.print(aht_ok_new ? "OK" : "ERR");
  Serial.print(" BMP=");
  Serial.println(bmp_ok_new ? "OK" : "ERR");

  // [F16.4] Инициализация MPU
  Serial.println("Reinit ALL: calling initMPU()");
  bool mpu_ok_new = initMPU();
  mpu_ok    = mpu_ok_new;
  mpu_error = !mpu_ok_new;

  Serial.print("Reinit ALL: MPU=");
  Serial.println(mpu_ok_new ? "OK" : "ERR");

  // [F16.5] Служебные флаги и «тихий» период
  mpuReadErrorStreak = 0;
  recovering = true;
  recoverUntilMs = millis() + RECOVER_MS;
}

// ============================================================================
//   [F16.1] manualZeroReset – ручной сброс нуля горизонта и курса
// ============================================================================
void manualZeroReset() {
  // Используем текущие сглаженные ax_f/ay_f/az_f для вычисления "сырых" углов
  float roll_rad  = atan2(ay_f, az_f);
  float pitch_rad = atan2(-ax_f, sqrt(ay_f * ay_f + az_f * az_f));

  float roll_raw_deg  = roll_rad  * 180.0f / PI;
  float pitch_raw_deg = pitch_rad * 180.0f / PI;

  // Ставим новое "физическое" положение как ноль горизонта
  roll_zero_deg  = roll_raw_deg;
  pitch_zero_deg = pitch_raw_deg;
  horizonZeroSet = true;

  // Приводим текущие углы к нулю относительно нового нуля
  roll_deg  = roll_raw_deg  - roll_zero_deg;   // будет ~0
  pitch_deg = pitch_raw_deg - pitch_zero_deg;  // будет ~0

  // Сбрасываем виртуальный курс относительно нового нуля
  heading_deg           = 0.0f;
  last_roll_for_heading = roll_deg;            // ~0
  headingInitDone       = true;
  headingZeroSet        = true;

  Serial.println("MANUAL ZERO RESET:");
  Serial.print("  roll_zero_deg=");
  Serial.print(roll_zero_deg, 1);
  Serial.print("  pitch_zero_deg=");
  Serial.println(pitch_zero_deg, 1);

  // Краткое сообщение на TFT
  tft.fillScreen(COL_FON1);
  tft.setTextWrap(false);
  tft.setTextColor(ST77XX_YELLOW, COL_FON1);
  tft.setTextSize(1);
  tft.setCursor(10, tft.height() / 2 - 10);
  tft.print("ZERO RESET");
  tft.setCursor(10, tft.height() / 2);
  tft.print("HORIZON & HDG");
  delay(800);   // короткая пауза чтобы увидеть
  // Вернём текущий экранный фрейм
  if (displayMode == 0) {
    drawEnvFrame();
    lastScreenUpdate = 0;
  } else {
    drawMPUFrame();
    lastScreenUpdate = 0;
  }
}

// ============================================================================
//   [F17] setup – инициализация устройств
// ============================================================================
void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }

  Serial.println("=== SETUP START ===");

  Wire.begin();
  Wire.setClock(100000);   // I2C 100 кГц

  pinMode(buttonPin,         INPUT_PULLUP);
  pinMode(AHT_BMP_PWR_PIN,   OUTPUT);
  pinMode(SV1_INDIC_PWR_PIN, OUTPUT);
  pinMode(MPU_PWR_PIN,       OUTPUT);
  pinMode(SV2_MPU_INDIC_PIN, OUTPUT);

  // [F17.1] Питание AHT/BMP (3.3V через BC327)
  digitalWrite(AHT_BMP_PWR_PIN, HIGH);   // HIGH = выкл питания
  digitalWrite(SV1_INDIC_PWR_PIN, LOW);  // LED AHT/BMP выкл
  delay(50);

  digitalWrite(AHT_BMP_PWR_PIN, LOW);    // LOW = вкл питания AHT/BMP
  digitalWrite(SV1_INDIC_PWR_PIN, HIGH); // LED AHT/BMP вкл
  delay(100);

  // [F17.2] Питание MPU6050 (5V через BC327)
  digitalWrite(MPU_PWR_PIN, HIGH);       // пока выкл
  digitalWrite(SV2_MPU_INDIC_PIN, LOW);  // LED MPU выкл
  delay(50);

  digitalWrite(MPU_PWR_PIN, LOW);        // LOW = вкл 5V на MPU
  digitalWrite(SV2_MPU_INDIC_PIN, HIGH); // LED MPU вкл
  delay(300);

  // [F17.3] Инициализация TFT
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(SET_ROTAC);
  tft.fillScreen(COL_FON1);
  tft.setTextWrap(false);

  showMessageTFT("INIT SENS", ZAGOLOVOK, COL_TEXT1, COL_FON1);
  delay(500);

  // [F17.4] Инициализация AHT20
  if (!aht.begin()) {
    aht_ok = false;
    Serial.println("AHT20 not found");
    showMessageTFT("AHT20 ERR", ZAGOLOVOK, COL_TEXT4, COL_FON4);
    delay(500);
    ahtErrCount++;
    poterya_svyazi = true;
    lastAhtRetry   = millis();
  } else {
    aht_ok = true;
    Serial.println("AHT20 OK");
    showMessageTFT("AHT20 OK", ZAGOLOVOK, COL_TEXT5, COL_FON5);
    delay(500);
  }

  // [F17.5] Инициализация BMP280
  if (!bmp.begin(0x76) && !bmp.begin(0x77)) {
    bmp_ok = false;
    Serial.println("BMP280 not found");
    showMessageTFT("BMP280 ERR", ZAGOLOVOK, COL_TEXT6, COL_FON6);
    delay(500);
    bmpErrCount++;
    if (!poterya_svyazi) {
      poterya_svyazi = true;
      lastAhtRetry   = millis();
    }
  } else {
    bmp_ok = true;
    Serial.println("BMP280 OK");
    showMessageTFT("BMP280 OK", ZAGOLOVOK, COL_TEXT7, COL_FON7);
    delay(500);

    bmp.setSampling(
      Adafruit_BMP280::MODE_NORMAL,
      Adafruit_BMP280::SAMPLING_X2,
      Adafruit_BMP280::SAMPLING_X16,
      Adafruit_BMP280::FILTER_X16,
      Adafruit_BMP280::STANDBY_MS_500
    );

    // [F17.5.1] Калибровка высоты
    const int ALT_CAL_SAMPLES = 20;
    float sumAlt = 0.0f;

    for (int i = 0; i < ALT_CAL_SAMPLES; i++) {
      sumAlt += bmp.readAltitude(SEA_LEVEL_HPA);
      delay(50);
    }

    float a0 = sumAlt / ALT_CAL_SAMPLES;
    altitudeOffset_m = a0;   // текущее место считаем 0 м
    altitude_m       = 0.0f;
    altitude_m_f     = 0.0f;

    Serial.print("Alt0 (calibrated avg) = ");
    Serial.println(a0);
  }

  // [F17.6] Инициализация MPU6050 через initMPU (после ENV)
  Serial.println("=== MPU init from setup (after ENV) ===");
  bool mpu_init_ok = initMPU();
  Serial.print("MPU init in setup = ");
  Serial.println(mpu_init_ok ? "OK" : "ERR");

  if (mpu_init_ok) {
    showMessageTFT("MPU OK", ZAGOLOVOK, COL_TEXT2, COL_FON2);
  } else {
    showMessageTFT("MPU ERR", ZAGOLOVOK, COL_TEXT3, COL_FON3);
  }
  delay(500);

  displayMode = 0;
  drawEnvFrame();

  unsigned long now = millis();
  lastScreenUpdate = now;
  lastSerialPrint  = now;
  startTime        = now;

  uiTimer.begin(1000, uiTimerCallback, true);

  Serial.print("START: ALT_zero=");
  Serial.print(altZeroRefSet ? altZeroRef : altitudeOffset_m, 2);
  Serial.print("  ALT_offset_m=");
  Serial.print(altitudeOffset_m, 2);
  Serial.print("  I2C_speed=");
  Serial.print(100000);
  Serial.println("Hz");

  Serial.println("=== SETUP DONE ===");
}

// ============================================================================
//   [F18] loop – основной цикл
//   Любая ошибка MPU/ENV приводит к reinitAllSensors().
// ============================================================================
void loop() {
  unsigned long now = millis();

  // [F18.1] Режим восстановления после общего сброса
  if (recovering && (long)(recoverUntilMs - now) <= 0) {
    recovering = false;
    Serial.println("RECOVER window finished");
  }

  // [F18.2] ЧТЕНИЕ MPU + автоперезапуск ВСЕХ датчиков (HUD)
  if (displayMode == 1) {
    if (!mpu_ok) {
      // [F18.2.1] MPU уже в ошибке – периодически пытаемся поднять всё вместе
      unsigned long now2 = millis();
      const unsigned long MPU_RETRY_INTERVAL_LOCAL = 5000;
      if (now2 - lastMpuRetry > MPU_RETRY_INTERVAL_LOCAL) {
        lastMpuRetry = now2;
        Serial.println("MPU in ERR state on HUD -> reinitAllSensors()");
        reinitAllSensors("MPU in ERR on HUD");
      }
    } else {
      // [F18.2.2] mpu_ok == true: нормальное чтение + отслеживание ошибок
      uint8_t data[14];
      uint8_t readCount = mpuRead(0x3B, data, 14);

      if (readCount < 14) {
        unsigned long now2 = millis();
        unsigned long dt   = now2 - mpuLastInitTime;
        long diffBmp       = (lastBmpReadMs == 0) ? -1 : (long)now2 - (long)lastBmpReadMs;

        Serial.print("MPU short read at ms=");
        Serial.print(now2);
        Serial.print(" dt since init=");
        Serial.print(dt);
        Serial.print("ms  displayMode=");
        Serial.print(displayMode);
        Serial.print("  diffToLastBMP=");
        Serial.print(diffBmp);
        Serial.println("ms");

        // Лог состояния MPU
        Serial.print("MPU state at error: roll=");
        Serial.print(roll_deg, 1);
        Serial.print(" pitch=");
        Serial.print(pitch_deg, 1);
        Serial.print("  ax=");
        Serial.print(ax_last, 2);
        Serial.print(" ay=");
        Serial.print(ay_last, 2);
        Serial.print(" az=");
        Serial.print(az_last, 2);
        Serial.print("  gx=");
        Serial.print(gx_last, 1);
        Serial.print(" gy=");
        Serial.print(gy_last, 1);
        Serial.print(" gz=");
        Serial.print(gz_last, 1);
        Serial.print("  temp=");
        Serial.print(temp_last, 1);
        Serial.println("C");

        const float ACC_STILL_THRESH  = 0.05f;
        const float GYRO_STILL_THRESH = 5.0f;

        bool accMoving  = (fabs(ax_last) > ACC_STILL_THRESH) ||
                          (fabs(ay_last) > ACC_STILL_THRESH) ||
                          (fabs(az_last - 1.0f) > ACC_STILL_THRESH);
        bool gyroMoving = (fabs(gx_last) > GYRO_STILL_THRESH) ||
                          (fabs(gy_last) > GYRO_STILL_THRESH) ||
                          (fabs(gz_last) > GYRO_STILL_THRESH);

        bool movingNow = accMoving || gyroMoving;

        if (movingNow) mpuErrWhileMoving++;
        else           mpuErrWhileStill++;

        Serial.print("MPU_ERR_STATS: movingNow=");
        Serial.print(movingNow ? "YES" : "NO");
        Serial.print("  errStill=");
        Serial.print(mpuErrWhileStill);
        Serial.print("  errMove=");
        Serial.println(mpuErrWhileMoving);

        if (dt < MPU_GRACE_MS) {
          Serial.println("MPU read error in grace period, ignored");
        } else {
          mpuReadErrorStreak++;
          Serial.print("MPU read error (");
          Serial.print(mpuReadErrorStreak);
          Serial.println(")");

          if (mpuReadErrorStreak >= 5) {
            unsigned long now2b = millis();
            mpuErrCount++;
            mpuReadErrorStreak = 0;

            const unsigned long MPU_RETRY_INTERVAL_LOCAL = 5000;
            if (now2b - lastMpuRetry > MPU_RETRY_INTERVAL_LOCAL) {
              lastMpuRetry = now2b;
              Serial.println("MPU read error streak -> reinitAllSensors()");
              reinitAllSensors("MPU read errors streak");
            } else {
              Serial.println("MPU read error streak, reinitAllSensors() suppressed (too frequent)");
            }
          }
        }

      } else {
        // [F18.2.3] Успешное чтение – обнуляем streak и считаем углы
        mpuReadErrorStreak = 0;

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

        ax_last   = ax;  ay_last   = ay;  az_last   = az;
        gx_last   = gx;  gy_last   = gy;  gz_last   = gz;
        temp_last = tempC;

        ax_f = ax_f + alpha * (ax - ax_f);
        ay_f = ay_f + alpha * (ay - ay_f);
        az_f = az_f + alpha * (az - az_f);
        gx_f = gx_f + alpha * (gx - gx_f);
        gy_f = gy_f + alpha * (gy - gy_f);
        gz_f = gz_f + alpha * (gz - gz_f);
        temp_f = temp_f + alpha * (tempC - temp_f);

        float roll_rad_calc  = atan2(ay_f, az_f);
        float pitch_rad_calc = atan2(-ax_f, sqrt(ay_f * ay_f + az_f * az_f));

        float roll_raw_deg  = roll_rad_calc  * 180.0f / PI;
        float pitch_raw_deg = pitch_rad_calc * 180.0f / PI;

        if (horizonZeroSet) {
          roll_deg  = roll_raw_deg  - roll_zero_deg;
          pitch_deg = pitch_raw_deg - pitch_zero_deg;
        } else {
          roll_deg  = roll_raw_deg;
          pitch_deg = pitch_raw_deg;
        }

        // Дополнительное сглаживание крена только для расчёта курса
        roll_for_heading_deg = roll_for_heading_deg + HEADING_ROLL_ALPHA * (roll_deg - roll_for_heading_deg);

        // [F18.2.3.1] Виртуальный курс по интегралу крена
        // [F18.2.3.1] Виртуальный курс по интегралу крена
        if (!headingInitDone) {
          heading_deg           = 0.0f;
          last_roll_for_heading = roll_for_heading_deg;
          headingInitDone       = true;
        } else {
          float dRoll = roll_for_heading_deg - last_roll_for_heading;

          if (dRoll > 180.0f)  dRoll -= 360.0f;
          if (dRoll < -180.0f) dRoll += 360.0f;

          heading_deg += dRoll;

          while (heading_deg >= 360.0f) heading_deg -= 360.0f;
          while (heading_deg <   0.0f) heading_deg += 360.0f;

          last_roll_for_heading = roll_for_heading_deg;
        }
      }
    }
  }

  // [F18.3] ЧТЕНИЕ AHT/BMP
  //   - в METEO-режиме (displayMode == 0) – как раньше, каждые SENSOR_POLL_INTERVAL
  //   - в HUD-режиме (displayMode == 1)   – реже, раз в HUD_SENSOR_POLL_INTERVAL
  if (displayMode == 0 && (now - lastSensorPoll >= SENSOR_POLL_INTERVAL)) {
    lastSensorPoll = now;

    if (recovering) {
      Serial.println("ENV poll skipped: recovering after reinitAllSensors()");
    } else if (!aht_ok || !bmp_ok) {
      // ENV уже в ошибке – пытаемся перезапустить, но не слишком часто
      static unsigned long lastEnvReinitTry = 0;
      const unsigned long ENV_RETRY_INTERVAL = 3000; // не чаще 1 раза в 3 секунды

      if (now - lastEnvReinitTry > ENV_RETRY_INTERVAL) {
        lastEnvReinitTry = now;
        Serial.println("ENV in ERR state on METEO -> reinitAllSensors()");
        reinitAllSensors("ENV errors on METEO");
      } else {
        Serial.println("ENV in ERR, but reinitAllSensors() suppressed (too frequent)");
      }

    } else {
      // Нормальная работа ENV в METEO
      pollEnvOnce();
    }
  }

  // [F18.3b] ЧТЕНИЕ AHT/BMP в HUD-режиме – реже, раз в 2 секунды
  if (displayMode == 1 && (now - lastHudEnvPoll >= HUD_SENSOR_POLL_INTERVAL)) {
    lastHudEnvPoll = now;

    if (recovering) {
      Serial.println("ENV poll (HUD) skipped: recovering after reinitAllSensors()");
    } else if (!aht_ok || !bmp_ok) {
      // В HUD тоже пробуем восстановить, но так же не слишком часто
      static unsigned long lastEnvReinitTryHud = 0;
      const unsigned long ENV_RETRY_INTERVAL_HUD = 5000; // можно чуть реже, чем в METEO

      if (now - lastEnvReinitTryHud > ENV_RETRY_INTERVAL_HUD) {
        lastEnvReinitTryHud = now;
        Serial.println("ENV in ERR state on HUD -> reinitAllSensors()");
        reinitAllSensors("ENV errors on HUD");
      } else {
        Serial.println("ENV in ERR (HUD), but reinitAllSensors() suppressed (too frequent)");
      }

    } else {
      // Нормальная работа ENV в HUD – один опрос
      pollEnvOnce();
    }
  }

  // [F18.4] ТЕНДЕНЦИЯ ДАВЛЕНИЯ
  if (displayMode == 0 && bmp_ok) {
    if (lastTrendTime == 0) {
      lastTrendTime = now;
      p_trend_ref   = p_mmHg;
    } else if (now - lastTrendTime > TREND_INTERVAL) {
      float diff = p_mmHg - p_trend_ref;
      if (diff > 2.0f)       pressureTrend = T_STATE_RISE;
      else if (diff < -2.0f) pressureTrend = T_STATE_FALL;
      else                   pressureTrend = T_STATE_STEADY;
      p_trend_ref   = p_mmHg;
      lastTrendTime = now;
    }
  }

   // [F18.5] КНОПКА: короткое нажатие – смена экрана,
  //               длинное нажатие – ручной сброс нуля горизонта/курса
  bool buttonState = digitalRead(buttonPin);

  // Фронты и антидребезг
  if (buttonState != lastButtonState &&
      (now - lastButtonChange) > DEBOUNCE_MS) {
    lastButtonChange = now;
    lastButtonState  = buttonState;

    if (buttonState == LOW) {
      // Нажатие началось
      buttonPressed     = true;
      buttonPressStart  = now;
      longPressHandled  = false;
    } else {
      // Кнопка отпущена
      if (buttonPressed) {
        unsigned long pressDur = now - buttonPressStart;

        if (!longPressHandled && pressDur < LONG_PRESS_MS) {
          // КОРОТКОЕ нажатие -> переключение экранов
          int oldMode = displayMode;
          displayMode = (displayMode + 1) % 2;

          if (displayMode == 0) {
            drawEnvFrame();
            lastScreenUpdate = 0;
          } else {
            drawMPUFrame();
            lastScreenUpdate = 0;
          }

          Serial.print("MODE_SWITCH: ");
          Serial.print(oldMode == 0 ? "METEO" : "HUD");
          Serial.print(" -> ");
          Serial.print(displayMode == 0 ? "METEO" : "HUD");
          Serial.print(" at ms=");
          Serial.print(now);

          if (bmp_ok && altZeroRefSet) {
            float alt_rel_now = altitude_m_f - altZeroRef;
            Serial.print("  ALT_rel=");
            Serial.print(alt_rel_now, 2);
            Serial.print("  ALT_f=");
            Serial.print(altitude_m_f, 2);
          }
          if (mpu_ok) {
            Serial.print("  MPU_T=");
            Serial.print(temp_f, 1);
          }
          Serial.println();
        }
      }

      buttonPressed = false;
    }
  }

  // Если кнопка удерживается – проверяем на длинное нажатие
  if (buttonPressed && !longPressHandled) {
    unsigned long pressDur = now - buttonPressStart;
    if (pressDur >= LONG_PRESS_MS) {
      longPressHandled = true;

      // Ручной сброс нуля – только если MPU доступен и мы в HUD‑режиме
      if (mpu_ok && displayMode == 1) {
        manualZeroReset();
      } else {
        Serial.println("MANUAL ZERO RESET ignored (MPU not OK or not in HUD mode)");
      }
    }
  }

  // [F18.6] ПЕРИОДИЧЕСКИЙ ВЫВОД В SERIAL (METEO)
  if (displayMode == 0 && now - lastSerialPrint > serialPrintInterval) {
    lastSerialPrint = now;

    Serial.print("MPU: ");
    if (mpu_ok) {
      Serial.print("T="); Serial.print(temp_f, 1); Serial.print("C  ");
    } else {
      Serial.print("ERR  ");
    }

    Serial.print("ENV: ");
    if (aht_ok) {
      Serial.print("T="); Serial.print(aht_tC, 1); Serial.print("C  H=");
      Serial.print(aht_hR, 0); Serial.print("%  ");
      float dew = calcDewPoint(aht_tC, aht_hR);
      Serial.print("Dew="); Serial.print(dew, 1); Serial.print("C  ");
    } else {
      Serial.print("AHT_ERR  ");
    }

    if (bmp_ok) {
      Serial.print("P="); Serial.print(p_mmHg, 0); Serial.print("mmHg  pressureTrend=");
      if (pressureTrend == T_STATE_RISE)        Serial.print("UP");
      else if (pressureTrend == T_STATE_FALL)   Serial.print("DOWN");
      else if (pressureTrend == T_STATE_STEADY) Serial.print("STEADY");
      else                                      Serial.print("N/A");
    } else {
      Serial.print("BMP_ERR");
    }

    Serial.print("  memERR_AHT="); Serial.print(ahtErrCount);
    Serial.print("  memERR_BMP="); Serial.print(bmpErrCount);
    Serial.print("  memERR_MPU="); Serial.print(mpuErrCount);
    Serial.print("  mpuErrStill="); Serial.print(mpuErrWhileStill);
    Serial.print("  mpuErrMove=");  Serial.print(mpuErrWhileMoving);
    Serial.println();
  }

  // [F18.7] ОБЩИЙ ЛОГ ВЫСОТЫ
  static unsigned long lastAltDebugGlobal = 0;
  if (millis() - lastAltDebugGlobal > 1000) {
    lastAltDebugGlobal = millis();

    if (bmp_ok && altZeroRefSet) {
      float alt_rel_now = altitude_m_f - altZeroRef;

      Serial.print("ALTLOG mode=");
      Serial.print(displayMode == 0 ? "METEO" : "HUD");
      Serial.print(" ms=");
      Serial.print(now);
      Serial.print(" raw=");
      Serial.print(altitude_m, 2);
      Serial.print(" f=");
      Serial.print(altitude_m_f, 2);
      Serial.print(" zero=");
      Serial.print(altZeroRef, 2);
      Serial.print(" rel=");
      Serial.print(alt_rel_now, 2);
      Serial.print(" state=");
      Serial.print((int)altState);
      Serial.print(" step=");
      Serial.println(altStep_m, 2);
    }
  }

  // [F18.8] ОБНОВЛЕНИЕ ЭКРАНА
  unsigned long screenInt = (displayMode == 0)
                            ? SCREEN_UPDATE_INTERVAL_METEO
                            : SCREEN_UPDATE_INTERVAL_HUD;

  if (now - lastScreenUpdate > screenInt) {
    lastScreenUpdate = now;

    if (displayMode == 0) {
      updateEnvValues();
    } else {
      if (mpu_ok) {
        drawHud();
      } else {
        tft.fillScreen(COL_FON1);
        tft.setTextColor(ST77XX_RED, COL_FON1);
        tft.setTextSize(1);
        int16_t h = tft.height();
        tft.setCursor(10, h / 2 - 4);
        tft.print("MPU ERR");
      }
    }
  }
}