/*
  CBTimer.cpp - Callback Timer using FSP Timer for Arduino UNO R4

  Copyright (c) 2024 embedded-kiddie All Rights Reserved.

  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
*/
#include "Arduino.h"

#if 0
#define debug_begin(...)  { Serial.begin(__VA_ARGS__); while(!Serial); }
#define debug_print(...)    Serial.print(__VA_ARGS__)
#define debug_println(...)  Serial.println(__VA_ARGS__)
#endif

#if 0
#define CBTIMER_FORCE_AGT
#endif

#include "CBTimer.h"

// Initialize static data menbers
int CBTimer::period_max = LIMIT_PERIOD_GPT;
int volatile CBTimer::period_ms = 0;
int volatile CBTimer::remain_ms = 0;
uint32_t volatile CBTimer::start_ms = 0;
void (*CBTimer::user_callback)(void) = nullptr;
FspTimer CBTimer::fsp_timer;

// Constructor / Destructor
CBTimer::CBTimer() {}
CBTimer::~CBTimer() {
  end();
}

// TIMER_MODE_PERIODIC or TIMER_MODE_ONE_SHOT (variants/MINIMA/includes/ra/fsp/inc/api/r_timer_api.h)
timer_mode_t CBTimer::timer_mode = TIMER_MODE_PERIODIC;

bool CBTimer::begin(timer_mode_t mode, int period, void (*callback)(), bool start) {
  timer_mode = mode;
  period_ms = remain_ms = period;
  CBTimer::user_callback = callback;
  return timer_config(timer_mode, period_ms, start);
}

bool CBTimer::begin(int period, void (*callback)(), bool start) {
  return begin(TIMER_MODE_PERIODIC, period, callback, start);
}

void CBTimer::cbtimer_callback(timer_callback_args_t __attribute__((unused)) * args) {
  debug_println("cbtimer_callback = " + String(args->event));

#ifndef CBTIMER_NO_SPLIT

  if (period_ms <= period_max) {
    // execute user callback immediately
    user_callback();
  }

  else {
    // actual elapsed time
    int t = (int)(millis() - start_ms);
    start_ms += t;

    if (remain_ms > period_max) {
      // still needs to be split
      timer_config(timer_mode, remain_ms -= t);
    } 

    else {
      if (timer_mode == TIMER_MODE_PERIODIC) {
        // rerun periodic timer before executing user callback
        timer_config(timer_mode, remain_ms = period_ms);
      }

      // execute user callback
      user_callback();
    }
  }

#else

  // execute user callback immediately
  user_callback();

#endif // CBTIMER_NO_SPLIT
}
