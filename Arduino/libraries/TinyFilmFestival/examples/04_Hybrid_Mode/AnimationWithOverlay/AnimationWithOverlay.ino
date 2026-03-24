/*
 * TinyFilmFestival V2 - Animation With Overlay
 * 04_Hybrid_Mode/AnimationWithOverlay
 * 
 * Combines a pre-made animation with code-drawn graphics.
 * The animation plays normally while a live indicator bar
 * animates independently on top.
 * 
 * This example demonstrates:
 *   - Playing a pre-made animation with play()
 *   - Drawing graphics over the animation with beginOverlay()/endOverlay()
 *   - Independent timing for overlay animation
 *   - The hybrid approach: best of both worlds
 * 
 * Hardware: Arduino UNO R4 WiFi (built-in 12×8 LED Matrix)
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
Animation backgroundAnim = idle;

// Overlay: animated indicator bar on the right edge
int indicatorLevel = 0;
int indicatorDirection = 1;
unsigned long lastIndicatorUpdate = 0;
int INDICATOR_SPEED = 150;

void setup()
{
    screen.begin();
    screen.play(backgroundAnim, LOOP);
    screen.setSpeed(100);
}

void loop()
{
    // Update overlay indicator independently
    if (millis() - lastIndicatorUpdate >= INDICATOR_SPEED)
    {
        indicatorLevel += indicatorDirection;
        if (indicatorLevel >= 7 || indicatorLevel <= 0)
        {
            indicatorDirection *= -1;
        }
        lastIndicatorUpdate = millis();
    }
    
    // beginOverlay() updates animation AND starts drawing mode
    screen.beginOverlay();
    
    // Draw indicator bar on the right edge (column 11)
    for (int y = 7; y > 7 - indicatorLevel; y--)
    {
        screen.point(11, y);
    }
    
    // Draw corner brackets as a frame
    // Top-left
    screen.point(0, 0);
    screen.point(1, 0);
    screen.point(0, 1);
    
    // Top-right (leave room for indicator)
    screen.point(9, 0);
    screen.point(10, 0);
    
    // Bottom-left
    screen.point(0, 7);
    screen.point(1, 7);
    screen.point(0, 6);
    
    // Bottom-right
    screen.point(9, 7);
    screen.point(10, 7);
    
    screen.endOverlay();
}
