/*
 * Smooth Animation Example
 * Demonstrates oscillate() and Ease for smooth motion
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

// Ease objects for smooth position control
Ease xPos(0);    // Horizontal position, starts at 0
Ease yPos(3);    // Vertical position, starts at middle

void setup()
{
    screen.begin();
    
    // Start first animation: move to right side over 2 seconds
    xPos.to(11, 2000);
}

void loop()
{
    // When horizontal movement finishes, start a new target
    if (xPos.done())
    {
        // Ping-pong between left and right
        if (xPos.target() > 5)
        {
            xPos.to(0, 2000);   // Move left over 2 seconds
        }
        else
        {
            xPos.to(11, 2000);  // Move right over 2 seconds
        }
    }
    
    // Use oscillate() for continuous vertical wave motion
    // Smoothly cycles between y=1 and y=6 over 1.5 seconds
    int y = oscillateInt(1, 6, 1500);
    
    // Draw the moving dot
    screen.beginDraw();
    screen.background(OFF);
    screen.stroke(ON);
    screen.point(xPos.intValue(), y);
    screen.endDraw();
}
