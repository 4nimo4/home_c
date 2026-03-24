/*
 * BlinkFrame Example
 * 
 * Inner 8x4 rectangle stays solid ON.
 * Outer 2-layer frame LEDs each blink at a random rate.
 *
 * Demonstrates:
 *   - ledBlink() with per-LED random rates
 *   - ledWrite() for static LEDs alongside blinking ones
 *   - ledUpdate() to drive all blink timers each loop
 *
 * LED Matrix Layout (12 columns x 8 rows):
 * 
 *   B = blinking (random rate)    S = solid ON
 * 
 *   B  B  B  B  B  B  B  B  B  B  B  B
 *   B  B  B  B  B  B  B  B  B  B  B  B
 *   B  B  S  S  S  S  S  S  S  S  B  B
 *   B  B  S  S  S  S  S  S  S  S  B  B
 *   B  B  S  S  S  S  S  S  S  S  B  B
 *   B  B  S  S  S  S  S  S  S  S  B  B
 *   B  B  B  B  B  B  B  B  B  B  B  B
 *   B  B  B  B  B  B  B  B  B  B  B  B
 */

#include "TinyFilmFestival.h"

void setup()
{
    ledBegin();
    randomSeed(analogRead(A0));   // Seed RNG for varied rates

    for (int x = 0; x < 12; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            // Inner rectangle: x 2-9, y 2-5  (solid ON)
            if (x >= 2 && x <= 9 && y >= 2 && y <= 5)
            {
                ledWrite(x, y, HIGH);
            }
            else
            {
                // Outer frame: blink at a random rate (100-800 ms)
                ledBlink(x, y, random(100, 800));
            }
        }
    }
}

void loop()
{
    ledUpdate();   // Process all blink timers — call every loop!
}
