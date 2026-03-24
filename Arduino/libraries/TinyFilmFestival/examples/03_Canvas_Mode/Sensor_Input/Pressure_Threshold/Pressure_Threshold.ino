/*
 * TinyFilmFestival V2 - Pressure Threshold (Canvas)
 * 03_Canvas_Mode/Pressure_Threshold
 * 
 * Switch between graphic styles based on pressure levels.
 * No press, light press, and hard press show different patterns.
 * 
 * Hardware:
 * - Arduino UNO R4 WiFi (built-in 12×8 LED Matrix)
 * - FSR (Force Sensitive Resistor) on analog pin A0
 * - 10K pull-down resistor
 * 
 * Concept: THRESHOLDS
 * Different visual states provide clear feedback for distinct pressure levels.
 * Graphics are always drawn — never blank.
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

int pressurePin = A0;

int LIGHT_THRESHOLD = 200;
int HARD_THRESHOLD = 600;

void setup()
{
    Serial.begin(9600);
    screen.begin();
    Serial.println("Canvas Pressure Threshold Demo");
    Serial.println("Press to change graphics:");
    Serial.println("  Light: horizontal line");
    Serial.println("  Medium: cross pattern");
    Serial.println("  Hard: X + cross pattern");
}

void loop()
{
    int pressure = analogRead(pressurePin);
    
    screen.beginDraw();
    screen.background(OFF);
    screen.stroke(ON);
    
    if (pressure < LIGHT_THRESHOLD)
    {
        // No/light pressure: horizontal line
        screen.line(0, 3, 11, 3);
    } else if (pressure < HARD_THRESHOLD)
    {
        // Medium pressure: cross pattern
        screen.line(0, 3, 11, 3);
        screen.line(5, 0, 5, 7);
    }
    else
    {
        // Hard pressure: X pattern fills screen
        screen.line(0, 0, 11, 7);
        screen.line(11, 0, 0, 7);
        screen.line(0, 3, 11, 3);
        screen.line(5, 0, 5, 7);
    }
    
    screen.endDraw();
}
