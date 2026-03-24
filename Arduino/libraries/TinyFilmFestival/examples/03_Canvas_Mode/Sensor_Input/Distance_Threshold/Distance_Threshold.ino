/*
 * TinyFilmFestival V2 - Distance Threshold (Canvas)
 * 03_Canvas_Mode/Distance_Threshold
 * 
 * Switch between graphic patterns based on distance zones.
 * The display is always active — distance determines what's drawn.
 * 
 * Hardware:
 * - Arduino UNO R4 WiFi (built-in 12×8 LED Matrix)
 * - HC-SR04 ultrasonic sensor (Trigger: A0, Echo: A1)
 * 
 * Library: EasyUltrasonic by George Spulber
 * 
 * Concept: THRESHOLDS
 * Different visual modes create distinct feedback for each interaction zone.
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
#include <EasyUltrasonic.h>

TinyScreen screen;

// Distance sensor
int trigPin = A0;
int echoPin = A1;
EasyUltrasonic ultrasonic;

const float FAR_THRESHOLD = 60.0;
const float MID_THRESHOLD = 30.0;

unsigned long lastRead = 0;
int readInterval = 50;

void setup()
{
    Serial.begin(9600);
    ultrasonic.attach(trigPin, echoPin);
    screen.begin();
    Serial.println("Canvas Distance Threshold Demo");
    Serial.println("Move hand to change graphics:");
    Serial.println("  Far: single dot");
    Serial.println("  Mid: circle outline");
    Serial.println("  Close: filled rectangle");
}

void loop()
{
    float dist = 0;
    
    if (millis() - lastRead > readInterval)
    {
        dist = ultrasonic.getDistanceCM();
        lastRead = millis();
    }
    
    screen.beginDraw();
    screen.background(OFF);
    screen.stroke(ON);
    
    if (dist <= 0)
    {
        // No reading - show error indicator
        screen.point(0, 0);
        screen.point(11, 7);
    } else if (dist > FAR_THRESHOLD)
    {
        // Far: single dot in center
        screen.point(5, 3);
        screen.point(6, 4);
    } else if (dist > MID_THRESHOLD)
    {
        // Middle: circle outline
        screen.noFill();
        screen.circle(5, 3, 2);
    }
    else
    {
        // Close: filled rectangle
        screen.fill(ON);
        screen.rect(2, 1, 8, 6);
    }
    
    screen.endDraw();
}
