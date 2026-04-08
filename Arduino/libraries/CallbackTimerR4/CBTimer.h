/*
  CBTimer.h - Callback Timer using FSP Timer for Arduino UNO R4

  Copyright (c) 2024 embedded-kiddie All Rights Reserved.

  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
*/
#ifndef CBTIMER_H
#define CBTIMER_H

#if !defined(ARDUINO_UNOR4_MINIMA) && !defined(ARDUINO_UNOR4_WIFI)
#error This library is only supported on the UNO-R4 (renesas_uno) boards.
#endif

#include "FspTimer.h"

#ifndef debug_begin
#define debug_begin(...)
#endif
#ifndef debug_print
#define debug_print(...)
#endif
#ifndef debug_println
#define debug_println(...)
#endif

// To start playing music immediately, you must specify "CBTIMER_START_NOW" as "period_ms" instead of "0".
#define CBTIMER_START_NOW (1)

// We can not get 32bit timer because they are already used.
// The limit of duration for GPT and AGT 16bit timer:
// - GPT: 1/(48MHz ÷ 1024) × 65535 = 1398.08[msec]
// - AGT: 1/(24MHz ÷ 8   ) × 65535 ≒   21.85[msec]
#define DIVISION_RATIO_GPT  1024
#define DIVISION_RATIO_AGT  8
#define LIMIT_PERIOD_GPT    1398
#define LIMIT_PERIOD_AGT    21

class CBTimer {
private:
  static int period_max;
  static volatile int period_ms;
  static volatile int remain_ms;
  static volatile uint32_t start_ms;
  static timer_mode_t timer_mode; // TIMER_MODE_PERIODIC or TIMER_MODE_ONE_SHOT (variants/MINIMA/includes/ra/fsp/inc/api/r_timer_api.h)
  static FspTimer fsp_timer;

  static void (*user_callback)(void);
  static void cbtimer_callback(timer_callback_args_t __attribute((unused)) * p_args);

public:
  CBTimer();
  ~CBTimer();
  bool begin(int period_ms, void (*callback)(void), bool start = true);
  bool begin(timer_mode_t timer_mode, int period_ms, void (*callback)(void), bool start = true);

  void end(void) {
    fsp_timer.end();
  }

  bool start(void) {
    start_ms = millis();
    return fsp_timer.start();
  }

  bool stop(void) {
    return fsp_timer.stop();
  }

  static bool timer_config(timer_mode_t mode, int period, bool start = true) {
    // set limit of duration
    period = min(period, period_max);

    if (fsp_timer.is_opened()) {
      fsp_timer.set_period_ms((double)period);

      // show FSP Timer infomation
      debug_println("is_opened = "      + String(period));
      debug_println("get_period_raw = " + String(fsp_timer.get_period_raw()));
      debug_println("get_counter    = " + String(fsp_timer.get_counter()));
      debug_println("get_freq_hz    = " + String(fsp_timer.get_freq_hz()));

      return true;
    }

    else {

#ifndef CBTIMER_FORCE_AGT

      // type is determined by get_available_timer(&type) as GPT_TIMER or AGT_TIMER
      uint8_t type = GPT_TIMER;
      int channel = FspTimer::get_available_timer(type);

#else

      // "FORCE_AGT" should not be defined. This is for testing purposes only.
      uint8_t type = AGT_TIMER;
      int channel = 1;  // chnnel 0 is already used for millis() and micros().

#endif  // CBTIMER_FORCE_AGT

      debug_println("type    = " + String(type) + (type == GPT_TIMER ? " (GPT)" : " (AGT)"));
      debug_println("channel = " + String(channel));
      debug_println("period  = " + String(period));

      if (channel != -1) {
        // calculate maximum limit of duration for each type of timer
        uint32_t freq_hz;
        if (type == GPT_TIMER) {
          freq_hz = R_FSP_SystemClockHzGet(FSP_PRIV_CLOCK_PCLKD);
          period_max = (int)(65535000.0 * DIVISION_RATIO_GPT / freq_hz); 
        } else {
          freq_hz = R_FSP_SystemClockHzGet(FSP_PRIV_CLOCK_PCLKB);
          period_max = (int)(65535000.0 * DIVISION_RATIO_AGT / freq_hz); 
        }

        debug_println("freq_hz = "    + String(freq_hz));
        debug_println("period_max = " + String(period_max));

        fsp_timer.begin(timer_mode, type, channel, 1000.0 / period, 100.0, cbtimer_callback, nullptr);
        fsp_timer.setup_overflow_irq();
        fsp_timer.open();
        if (start) {
          fsp_timer.start();
        }

        // show FSP Timer infomation
        debug_println("get_period_raw = " + String(fsp_timer.get_period_raw()));
        debug_println("get_counter    = " + String(fsp_timer.get_counter()));
        debug_println("get_freq_hz    = " + String(fsp_timer.get_freq_hz()));

        return true;
      }
    }
    return false;
  }
};

#endif /* CBTIMER_H */
