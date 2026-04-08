# Arduino UNO R4 timer library using FspTimer class.

This library allows you to generate as interrupt at specified cycle and call your function when timer event fires.

## Usage

The following codes modulate LED (`LED_BUILTIN`) in `loop()` and also blink LEDs (`LED_TX` and `LED_RX`) in `callback_func()`.

```C++
#include "Arduino.h"
#include "CBTimer.h"

void callback_func(void) {
  static int n = 0;

  if (n++ % 2) {
    digitalWrite(LED_TX, HIGH);
    digitalWrite(LED_RX,  LOW);
  } else {
    digitalWrite(LED_TX,  LOW);
    digitalWrite(LED_RX, HIGH);
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_TX, OUTPUT);
  pinMode(LED_RX, OUTPUT);

  static CBTimer timer;
  timer.begin(100, callback_func);
}

void loop() {
  // put your main code here, to run repeatedly:
  int i = 0;

  while (i < 256) {
    analogWrite(LED_BUILTIN, i++);
    delay(2);
  }

  while (i >= 0) {
    analogWrite(LED_BUILTIN, i--);
    delay(2);
  }
}
```

## API

### bool begin(int period_ms, void (*callback)(void), bool start = true)

- int period_ms ... The time in milli seconds between starting the timer and calling your callback function. Or the interval between calling your callback function. You should specify `CBTIMER_START_NOW` instead of `0` when you start the timer immediately. It Returns false if timer allocation fails.

- void (*callback)(void) ... Your callback function.

- bool start = true ... Start the timer when true or pend it when false.

### bool begin(timer_mode_t timer_mode, int period_ms, void (*callback)(void), bool start = true);

- timer_mode_t timer_mode ... `TIMER_MODE_PERIODIC` (default) or `TIMER_MODE_ONE_SHOT`.

Ohters are same as the previous method.

### bool start(void)
Start the timer.

### void stop(void)
Pend the timer.

### void end(void)
Calling this method followed by `stop()` will release the timer resource.

### bool timer_config(timer_mode_t mode, int period_ms, bool start = true)

You can change the timer mode and period in your callback function.
