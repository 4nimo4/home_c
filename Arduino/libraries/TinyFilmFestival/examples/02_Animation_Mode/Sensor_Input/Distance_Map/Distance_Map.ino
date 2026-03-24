/*
 * TinyFilmFestival V2 - Distance Map
 * 02_Animation_Mode/Distance_Map
 * 
 * Map distance sensor values directly to animation speed.
 * Closer = faster, farther = slower.
 * 
 * Hardware:
 * - Arduino UNO R4 WiFi (built-in 12×8 LED Matrix)
 * - HC-SR04 ultrasonic sensor (Trigger: A0, Echo: A1)
 * 
 * Library: EasyUltrasonic by George Spulber
 * 
 * Concept: MAP
 * The map() function converts a value from one range to another.
 * This creates smooth, continuous control rather than discrete steps.
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
#include "idle.h"

TinyScreen screen;
Animation idleAnim = idle;

// Distance sensor
int trigPin = A0;
int echoPin = A1;
EasyUltrasonic ultrasonic;

// Define the input range (distance in cm)
const float MIN_DISTANCE = 5.0;    // Closest expected distance
const float MAX_DISTANCE = 100.0;  // Farthest expected distance

// Define the output range (speed multiplier x10 for integer math)
int MIN_SPEED = 5;   // 0.5x speed when far
int MAX_SPEED = 30;  // 3.0x speed when close

unsigned long lastRead = 0;
int readInterval = 50;  // Faster reads for smoother response

void setup()
{
    Serial.begin(9600);
    ultrasonic.attach(trigPin, echoPin);
    screen.begin();
    screen.play(idleAnim, LOOP);
    Serial.println("Distance Map Demo");
    Serial.println("Move hand closer = faster animation");
}

void loop()
{
    if (millis() - lastRead > readInterval)
    {
        float dist = ultrasonic.getDistanceCM();
        
        if (dist > 0)
        {
            // Constrain distance to expected range
            dist = constrain(dist, MIN_DISTANCE, MAX_DISTANCE);
            
            // Map distance to speed (inverted: closer = faster)
            // Use integer math then convert to float
            int speedValue = map(dist * 10, MIN_DISTANCE * 10, MAX_DISTANCE * 10, 
                                 MAX_SPEED, MIN_SPEED);
            float speed = speedValue / 10.0;
            
            screen.setSpeed(speed);
            
            Serial.print(dist);
            Serial.print(" cm -> ");
            Serial.print(speed);
            Serial.println("x speed");
        }
        lastRead = millis();
    }
    
    screen.update();
}
