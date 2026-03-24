/*
 * BlinkLED Example
 *
 * Demonstrates non-blocking blink alongside direct LED control.
 * Each LED can blink at its own independent rate while other LEDs
 * are controlled normally with ledWrite() or ledToggle().
 *
 * Functions:
 *   ledBlink(x, y, rateMs)  - Blink LED at (x,y) every rateMs milliseconds
 *   ledBlink(index, rateMs) - Blink LED by linear index
 *   ledNoBlink(x, y)        - Stop blinking LED at (x,y), turns it OFF
 *   ledNoBlink(index)       - Stop blinking by index
 *   ledNoBlink()            - Stop ALL blinking
 *   ledUpdate()             - Process blink timers (call every loop!)
 *
 * Blink coexists with ledWrite/ledToggle:
 *   - Calling ledWrite() or ledToggle() on a blinking LED stops its blink
 *   - Calling ledBlink() on a static LED starts it blinking
 *   - ledClear() stops all blinking and turns everything off
 */

#include "TinyFilmFestival.h"

void setup()
{
    ledBegin();

    // Three LEDs blinking at different rates
    ledBlink(0, 0, 200);     // Top-left: fast blink (200ms)
    ledBlink(5, 3, 500);     // Center:   medium blink (500ms)
    ledBlink(11, 7, 1000);   // Bottom-right: slow blink (1 second)

    // These LEDs stay solid — no blinking
    ledWrite(6, 0, HIGH);    // Steady ON
    ledWrite(7, 0, HIGH);    // Steady ON
}

void loop()
{
    // IMPORTANT: call ledUpdate() every loop to drive blink timers
    ledUpdate();
}
