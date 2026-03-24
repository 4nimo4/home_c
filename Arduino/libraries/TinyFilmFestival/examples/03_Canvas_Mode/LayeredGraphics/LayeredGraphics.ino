/*
 * TinyFilmFestival - Canvas Mode - LayeredGraphics
 * 
 * Multiple independent animated elements on screen,
 * each moving at its own speed. This mirrors LayeredAnimations
 * from Animation Mode but creates everything with code.
 * 
 * This example demonstrates:
 *   - Multiple animation states (position, direction, timing)
 *   - Independent speed control per element
 *   - Combining different shapes in one scene
 *   - Non-blocking timing with millis()
 * 
 * Mode: Canvas
 * Mirrors: 02_Animation_Mode/LayeredAnimations
 * 
 * Hardware Required:
 *   - Arduino UNO R4 WiFi
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

// === Layer 1: Slow horizontal scan line (background) ===
int scanY = 0;
int scanDirection = 1;
unsigned long lastScanUpdate = 0;
int SCAN_SPEED = 200;  // Slow: 200ms per step

// === Layer 2: Fast bouncing dot (foreground) ===
int dotX = 6;
int dotDirection = 1;
unsigned long lastDotUpdate = 0;
int DOT_SPEED = 50;  // Fast: 50ms per step

// === Layer 3: Pulsing center ring (medium speed) ===
int ringSize = 2;
int ringDirection = 1;
unsigned long lastRingUpdate = 0;
int RING_SPEED = 120;  // Medium: 120ms per step

void setup()
{
    screen.begin();
}

void loop()
{
    unsigned long currentTime = millis();
    
    // Update Layer 1: Scan line (slow)
    if (currentTime - lastScanUpdate >= SCAN_SPEED)
    {
        scanY += scanDirection;
        if (scanY >= 7 || scanY <= 0)
        {
            scanDirection *= -1;
        }
        lastScanUpdate = currentTime;
    }
    
    // Update Layer 2: Bouncing dot (fast)
    if (currentTime - lastDotUpdate >= DOT_SPEED)
    {
        dotX += dotDirection;
        if (dotX >= 11 || dotX <= 0)
        {
            dotDirection *= -1;
        }
        lastDotUpdate = currentTime;
    }
    
    // Update Layer 3: Pulsing ring (medium)
    if (currentTime - lastRingUpdate >= RING_SPEED)
    {
        ringSize += ringDirection;
        if (ringSize >= 4 || ringSize <= 1)
        {
            ringDirection *= -1;
        }
        lastRingUpdate = currentTime;
    }
    
    // Draw all layers
    screen.beginDraw();
    screen.background(OFF);
    screen.stroke(ON);
    
    // Layer 1: Horizontal scan line
    screen.line(0, scanY, 11, scanY);
    
    // Layer 2: Bouncing dot
    screen.point(dotX, 4);
    
    // Layer 3: Pulsing ring (outline only)
    screen.noFill();
    screen.circle(6, 4, ringSize * 2);
    
    screen.endDraw();
}
