/*
 * TinyFilmFestival V2 - Distance Map (Canvas)
 * 03_Canvas_Mode/Distance_Map
 * 
 * Map distance directly to the size of a shape.
 * Closer = bigger circle.
 * 
 * Hardware:
 * - Arduino UNO R4 WiFi (built-in 12×8 LED Matrix)
 * - HC-SR04 ultrasonic sensor (Trigger: A0, Echo: A1)
 * 
 * Library: EasyUltrasonic by George Spulber
 * 
 * Concept: MAP
 * Mapping distance to a visual property creates responsive, 
 * continuous feedback.
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

const float MIN_DISTANCE = 5.0;
const float MAX_DISTANCE = 80.0;
int MIN_RADIUS = 1;
int MAX_RADIUS = 4;

unsigned long lastRead = 0;
int readInterval = 50;
int currentRadius = 1;

void setup()
{
    Serial.begin(9600);
    ultrasonic.attach(trigPin, echoPin);
    screen.begin();
    Serial.println("Canvas Distance Map Demo");
    Serial.println("Move closer = bigger circle");
}

void loop()
{
    if (millis() - lastRead > readInterval)
    {
        float dist = ultrasonic.getDistanceCM();
        
        if (dist > 0)
        {
            // Constrain and map distance to radius
            dist = constrain(dist, MIN_DISTANCE, MAX_DISTANCE);
            // Map inverted: closer = bigger
            currentRadius = map(dist * 10, MIN_DISTANCE * 10, MAX_DISTANCE * 10, 
                               MAX_RADIUS * 10, MIN_RADIUS * 10) / 10;
        }
        lastRead = millis();
    }
    
    screen.beginDraw();
    screen.background(OFF);
    screen.stroke(ON);
    screen.fill(ON);
    screen.circle(5, 3, currentRadius);
    screen.endDraw();
}
