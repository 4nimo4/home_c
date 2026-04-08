/*
  Example of CallbackTimerR4

  Copyright (c) 2024 embedded-kiddie All Rights Reserved.

  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
*/
#include "Arduino.h"
#include "CBTimer.h"

#if 1
#define TIME_FUNCTION millis  // Check in milliseconds
#define TIME_SCALE    1
#else
#define TIME_FUNCTION micros  // Check in microseconds
#define TIME_SCALE    1000
#endif

#define TIME_PERIOD_MS    10 // 10, 50, 100, 500, 1000, 1500, 2000, 3000, ...[msec]
#define TIME_MEASUREMENT  (60000 * TIME_SCALE)  // measurement for 1 minute

static CBTimer timer;
static volatile uint32_t interrupts = 0;
static volatile uint32_t start_time;
static volatile uint32_t begin, end, lap;
static volatile uint32_t elapsed, period;

void user_callback(void) {
  end = TIME_FUNCTION();

  elapsed = end - begin;
  period  = end - lap;
  
  lap = end;
  interrupts++;

  // Blink LED
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  // Initializing a periodic timer
  timer.begin(TIMER_MODE_PERIODIC, TIME_PERIOD_MS, user_callback, false);

  // Start the timer
  start_time = begin = lap = TIME_FUNCTION();
  timer.start();
}

void loop() {
  // Capture interrupt 
  if (end) {
    end = 0;
    Serial.println(String(elapsed) + " (" + String(period) + ")");
  }

  // Stop measurement
  if (TIME_FUNCTION() - start_time > TIME_MEASUREMENT) {
    timer.stop();
    Serial.println("Number of interrupts = " + String(interrupts));
    while (1);
  }
}
