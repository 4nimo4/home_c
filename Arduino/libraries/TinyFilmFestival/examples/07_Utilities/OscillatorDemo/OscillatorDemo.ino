/*
 * Oscillator Demo
 * Shows different uses of oscillate() for smooth cyclic motion
 * 
 * LED Matrix Layout (12x8):
 *
 *       x=0  x=1  x=2  x=3  x=4  x=5  x=6  x=7  x=8  x=9  x=10 x=11
 *      +----+----+----+----+----+----+----+----+----+----+----+----+
 *  y=0 |  0 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |  8 |  9 | 10 | 11 |
 *      +----+----+----+----+----+----+----+----+----+----+----+----+
 *  y=1 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 20 | 21 | 22 | 23 |
 *      +----+----+----+----+----+----+----+----+----+----+----+----+
 *  y=2 | 24 | 25 | 26 | 27 | 28 | 29 | 30 | 31 | 32 | 33 | 34 | 35 |
 *      +----+----+----+----+----+----+----+----+----+----+----+----+
 *  y=3 | 36 | 37 | 38 | 39 | 40 | 41 | 42 | 43 | 44 | 45 | 46 | 47 |
 *      +----+----+----+----+----+----+----+----+----+----+----+----+
 *  y=4 | 48 | 49 | 50 | 51 | 52 | 53 | 54 | 55 | 56 | 57 | 58 | 59 |
 *      +----+----+----+----+----+----+----+----+----+----+----+----+
 *  y=5 | 60 | 61 | 62 | 63 | 64 | 65 | 66 | 67 | 68 | 69 | 70 | 71 |
 *      +----+----+----+----+----+----+----+----+----+----+----+----+
 *  y=6 | 72 | 73 | 74 | 75 | 76 | 77 | 78 | 79 | 80 | 81 | 82 | 83 |
 *      +----+----+----+----+----+----+----+----+----+----+----+----+
 *  y=7 | 84 | 85 | 86 | 87 | 88 | 89 | 90 | 91 | 92 | 93 | 94 | 95 |
 *      +----+----+----+----+----+----+----+----+----+----+----+----+
 */

#include "TinyFilmFestival.h"

TinyScreen screen;

void setup()
{
    screen.begin();
}

void loop()
{
    screen.beginDraw();
    screen.background(OFF);
    screen.stroke(ON);
    
    // Three dots at different oscillation speeds
    // Each creates a smooth sine wave motion
    
    // Slow oscillation (3 second period) - left column
    int y1 = oscillateInt(0, 7, 3000);
    screen.point(2, y1);
    
    // Medium oscillation (1.5 second period) - middle column
    int y2 = oscillateInt(0, 7, 1500);
    screen.point(5, y2);
    
    // Fast oscillation (0.75 second period) - right column
    int y3 = oscillateInt(0, 7, 750);
    screen.point(9, y3);
    
    screen.endDraw();
}
