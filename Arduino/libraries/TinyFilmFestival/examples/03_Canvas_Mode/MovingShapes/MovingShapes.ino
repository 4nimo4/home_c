/*
 * TinyFilmFestival - Canvas Mode - MovingShapes
 * 
 * Code-generated animations with different motion behaviors.
 * This mirrors the Animation Mode PlayModes (ONCE, LOOP, BOOMERANG)
 * but creates the motion entirely with code.
 * 
 * This example demonstrates:
 *   - Three motion types: ONCE, LOOP, BOOMERANG
 *   - Drawing filled shapes (circle)
 *   - Using Serial to switch between modes
 *   - State machine for animation control
 * 
 * Mode: Canvas
 * Mirrors: 02_Animation_Mode/PlaybackControl
 * 
 * Hardware Required:
 *   - Arduino UNO R4 WiFi
 * 
 * Usage:
 *   Open Serial Monitor and send:
 *   '1' = ONCE mode (shape crosses screen and stops)
 *   '2' = LOOP mode (shape wraps around continuously)
 *   '3' = BOOMERANG mode (shape bounces back and forth)
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

// Motion modes (mirrors Animation PlayModes)
enum MotionMode {
    MODE_ONCE,
    MODE_LOOP,
    MODE_BOOMERANG
};

// Animation state
int x = 0;
int direction = 1;
MotionMode currentMode = MODE_LOOP;
bool completed = false;

// Timing
unsigned long lastMove = 0;
int speed = 80;  // milliseconds per frame

void setup()
{
    Serial.begin(9600);
    screen.begin();
    
    Serial.println("=== MovingShapes ===");
    Serial.println("Send: 1=ONCE, 2=LOOP, 3=BOOMERANG");
    Serial.println("Starting in LOOP mode");
}

void loop()
{
    // Check for mode changes via Serial
    if (Serial.available())
    {
        char c = Serial.read();
        switch(c)
        {
            case '1':
                currentMode = MODE_ONCE;
                resetAnimation();
                Serial.println("Mode: ONCE");
                break;
            case '2':
                currentMode = MODE_LOOP;
                resetAnimation();
                Serial.println("Mode: LOOP");
                break;
            case '3':
                currentMode = MODE_BOOMERANG;
                resetAnimation();
                Serial.println("Mode: BOOMERANG");
                break;
        }
    }
    
    // Update position at specified speed
    if (millis() - lastMove >= speed && !completed)
    {
        updatePosition();
        lastMove = millis();
    }
    
    // Draw the shape
    screen.beginDraw();
    screen.background(OFF);
    screen.fill(ON);
    screen.stroke(ON);
    screen.circle(x, 4, 4);   // Circle at current x position
    screen.endDraw();
}

void updatePosition()
{
    x += direction;
    
    switch(currentMode)
    {
        case MODE_ONCE:
            // Stop when reaching the far edge
            if (x >= 10)
            {
                x = 10;
                completed = true;
                Serial.println("Animation complete");
            }
            break;
            
        case MODE_LOOP:
            // Wrap around when exiting screen
            if (x > 13)
            {
                x = -2;  // Start just off the left edge
            }
            break;
            
        case MODE_BOOMERANG:
            // Bounce at edges
            if (x >= 10 || x <= 1)
            {
                direction *= -1;
            }
            break;
    }
}

void resetAnimation()
{
    x = 1;
    direction = 1;
    completed = false;
}
