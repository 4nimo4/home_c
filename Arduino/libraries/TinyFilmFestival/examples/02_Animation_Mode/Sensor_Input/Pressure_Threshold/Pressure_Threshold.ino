/*
 * TinyFilmFestival V2 - Pressure Threshold
 * 02_Animation_Mode/Pressure_Threshold
 * 
 * Switch between animations based on pressure levels.
 * Light touch, medium press, and hard press trigger different animations.
 * 
 * Hardware:
 * - Arduino UNO R4 WiFi (built-in 12×8 LED Matrix)
 * - FSR (Force Sensitive Resistor) on analog pin A0
 * - 10K pull-down resistor
 * 
 * Concept: THRESHOLDS
 * Multiple thresholds create multiple interaction zones from a single 
 * pressure sensor. The animation is always playing — never off.
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
#include "go.h"

TinyScreen screen;
Animation idleAnim = idle;
Animation goAnim = go;

int pressurePin = A0;

// Define pressure thresholds (0-1023 range)
int LIGHT_THRESHOLD = 200;   // Below = idle slow
int MEDIUM_THRESHOLD = 500;  // Between = idle fast
                                   // Above = go

int currentMode = 0;

void setup()
{
    Serial.begin(9600);
    screen.begin();
    screen.play(idleAnim, LOOP);
    Serial.println("Pressure Threshold Demo");
    Serial.println("Press the FSR to change animations:");
    Serial.println("  Light (<200): idle slow");
    Serial.println("  Medium (200-500): idle fast");
    Serial.println("  Hard (>500): go");
}

void loop()
{
    int pressure = analogRead(pressurePin);
    int newMode;
    
    // Determine mode based on pressure thresholds
    if (pressure < LIGHT_THRESHOLD)
    {
        newMode = 0;  // Light/no pressure = idle slow
    } else if (pressure < MEDIUM_THRESHOLD)
    {
        newMode = 1;  // Medium pressure = idle fast
    }
    else
    {
        newMode = 2;  // High pressure = go
    }
    
    // Only switch when mode changes
    if (newMode != currentMode)
    {
        currentMode = newMode;
        
        switch (currentMode)
        {
            case 0: 
                screen.play(idleAnim, LOOP);
                screen.setSpeed(1.0);
                Serial.println("LIGHT - idle (slow)");
                break;
            case 1: 
                screen.play(idleAnim, LOOP);
                screen.setSpeed(2.5);
                Serial.println("MEDIUM - idle (fast)");
                break;
            case 2: 
                screen.play(goAnim, LOOP);
                screen.setSpeed(1.0);
                Serial.println("HARD - go");
                break;
        }
    }
    
    screen.update();
}
