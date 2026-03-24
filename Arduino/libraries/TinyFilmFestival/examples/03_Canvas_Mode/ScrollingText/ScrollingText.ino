/*
 * ScrollingText Example
 * TinyFilmFestival V2 - Canvas Mode
 * 
 * Demonstrates text display and scrolling on the LED matrix.
 * Shows static text, then scrolling text in different directions.
 * 
 * LED Matrix Layout (12 columns x 8 rows):
 * 
 *      x=0  x=1  x=2  x=3  x=4  x=5  x=6  x=7  x=8  x=9  x=10 x=11
 *     +----+----+----+----+----+----+----+----+----+----+----+----+
 * y=0 |  0 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |  8 |  9 | 10 | 11 |
 *     +----+----+----+----+----+----+----+----+----+----+----+----+
 * y=1 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 20 | 21 | 22 | 23 |
 *     +----+----+----+----+----+----+----+----+----+----+----+----+
 * y=2 | 24 | 25 | 26 | 27 | 28 | 29 | 30 | 31 | 32 | 33 | 34 | 35 |
 *     +----+----+----+----+----+----+----+----+----+----+----+----+
 * y=3 | 36 | 37 | 38 | 39 | 40 | 41 | 42 | 43 | 44 | 45 | 46 | 47 |
 *     +----+----+----+----+----+----+----+----+----+----+----+----+
 * y=4 | 48 | 49 | 50 | 51 | 52 | 53 | 54 | 55 | 56 | 57 | 58 | 59 |
 *     +----+----+----+----+----+----+----+----+----+----+----+----+
 * y=5 | 60 | 61 | 62 | 63 | 64 | 65 | 66 | 67 | 68 | 69 | 70 | 71 |
 *     +----+----+----+----+----+----+----+----+----+----+----+----+
 * y=6 | 72 | 73 | 74 | 75 | 76 | 77 | 78 | 79 | 80 | 81 | 82 | 83 |
 *     +----+----+----+----+----+----+----+----+----+----+----+----+
 * y=7 | 84 | 85 | 86 | 87 | 88 | 89 | 90 | 91 | 92 | 93 | 94 | 95 |
 *     +----+----+----+----+----+----+----+----+----+----+----+----+
 */

#include "TinyFilmFestival.h"

TinyScreen screen;

int phase = 0;
unsigned long phaseStart;

void setup()
{
    screen.begin();
    phaseStart = millis();
}

void loop()
{
    unsigned long elapsed = millis() - phaseStart;
    
    switch (phase)
    {
        case 0:
            // Static text for 3 seconds
            screen.beginDraw();
            screen.stroke(ON);
            screen.text("Hi!", 1, 2);
            screen.endDraw();
            
            if (elapsed > 3000)
            {
                phase = 1;
                phaseStart = millis();
                screen.resetScroll();
                screen.setScrollSpeed(80);  // 80ms per pixel
            }
            break;
            
        case 1:
            // Scroll left for 8 seconds
            screen.beginDraw();
            screen.stroke(ON);
            screen.scrollText("HELLO WORLD", 2, SCROLL_LEFT);
            screen.endDraw();
            
            if (elapsed > 8000)
            {
                phase = 2;
                phaseStart = millis();
                screen.resetScroll();
            }
            break;
            
        case 2:
            // Scroll right for 8 seconds  
            screen.beginDraw();
            screen.stroke(ON);
            screen.scrollText("ARDUINO", 2, SCROLL_RIGHT);
            screen.endDraw();
            
            if (elapsed > 8000)
            {
                phase = 0;
                phaseStart = millis();
            }
            break;
    }
}
