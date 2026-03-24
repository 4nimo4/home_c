/*
 * TinyFilmFestival V2 - Pressure Map (Hybrid)
 * 04_Hybrid_Mode/Pressure_Map
 * 
 * Map pressure to the number of overlay dots.
 * More pressure = more dots appearing over the animation.
 * 
 * Hardware:
 * - Arduino UNO R4 WiFi (built-in 12×8 LED Matrix)
 * - FSR (Force Sensitive Resistor) on analog pin A0
 * - 10K pull-down resistor
 * 
 * Concept: MAP
 * Continuous mapping creates responsive visual feedback 
 * layered on animation.
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
#include "idle.h"

TinyScreen screen;
Animation idleAnim = idle;

int pressurePin = A0;

void setup()
{
    Serial.begin(9600);
    screen.begin();
    screen.play(idleAnim, LOOP);
    Serial.println("Hybrid Pressure Map Demo");
    Serial.println("Press harder = more dots on top row");
}

void loop()
{
    screen.update();
    
    int pressure = analogRead(pressurePin);
    
    // Map pressure to number of dots (0-12)
    int numDots = map(pressure, 0, 1023, 0, 12);
    
    screen.beginOverlay();
    
    // Draw dots across top row
    for (int x = 0; x < numDots; x++)
    {
        screen.point(x, 0);
    }
    
    screen.endOverlay();
}
