/*
 * TinyFilmFestival V2 - Playback Control
 * 02_Animation_Mode/PlaybackControl
 * 
 * Demonstrates all Animation Mode playback options:
 * - Playback modes: ONCE, LOOP, BOOMERANG
 * - Speed control: setSpeed(), restoreOriginalSpeed()
 * - Partial playback: play specific frame ranges
 * 
 * Animation files (.h) are created using the Arduino LED Matrix Editor:
 * https://ledmatrix-editor.arduino.cc/
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
#include "animation.h"

TinyScreen screen;
Animation myAnim = animation;

// Demo cycling
int demoMode = 0;
unsigned long lastChange = 0;
int demoDuration = 4000;

void setup()
{
    Serial.begin(9600);
    screen.begin();
    
    Serial.print("Animation has ");
    Serial.print(myAnim.getFrameCount());
    Serial.println(" frames");
    
    startDemo(0);
}

void loop()
{
    // Cycle through demos every few seconds
    if (millis() - lastChange > demoDuration)
    {
        demoMode = (demoMode + 1) % 5;
        startDemo(demoMode);
        lastChange = millis();
    }
    
    screen.update();
}

void startDemo(int mode)
{
    switch (mode)
    {
        case 0:
            // LOOP: continuous forward playback
            screen.play(myAnim, LOOP);
            screen.restoreOriginalSpeed();
            Serial.println("Mode: LOOP (normal speed)");
            break;
            
        case 1:
            // BOOMERANG: forward then backward
            screen.play(myAnim, BOOMERANG);
            Serial.println("Mode: BOOMERANG");
            break;
            
        case 2:
            // Fast speed
            screen.play(myAnim, LOOP);
            screen.setSpeed(30);
            Serial.println("Mode: LOOP (fast - 30ms)");
            break;
            
        case 3:
            // Slow speed
            screen.play(myAnim, LOOP);
            screen.setSpeed(200);
            Serial.println("Mode: LOOP (slow - 200ms)");
            break;
            
        case 4:
            // Partial playback: frames 2-5 only
            screen.play(myAnim, LOOP, 2, 5);
            screen.restoreOriginalSpeed();
            Serial.println("Mode: LOOP (frames 2-5 only)");
            break;
    }
}
