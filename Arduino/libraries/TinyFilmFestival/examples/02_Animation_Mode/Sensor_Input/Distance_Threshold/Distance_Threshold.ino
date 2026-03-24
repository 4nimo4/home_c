/*
 * TinyFilmFestival V2 - Distance Threshold
 * 02_Animation_Mode/Distance_Threshold
 * 
 * Switch between animations based on distance zones (thresholds).
 * The animation is always playing — distance determines which one.
 * 
 * Hardware:
 * - Arduino UNO R4 WiFi (built-in 12×8 LED Matrix)
 * - HC-SR04 ultrasonic sensor (Trigger: A0, Echo: A1)
 * 
 * Library: EasyUltrasonic by George Spulber
 * 
 * Concept: THRESHOLDS
 * Thresholds divide sensor input into discrete zones.
 * Each zone triggers a different mode/state.
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
#include "go.h"

TinyScreen screen;
Animation idleAnim = idle;
Animation goAnim = go;

// Distance sensor
int trigPin = A0;
int echoPin = A1;
EasyUltrasonic ultrasonic;

// Define threshold distances (in cm)
const float FAR_THRESHOLD = 60.0;   // Beyond this = idle
const float CLOSE_THRESHOLD = 30.0; // Closer than this = go (fast)
                                    // Between = idle (medium speed)

int currentMode = 0;  // 0=far(idle), 1=mid(idle fast), 2=close(go)

unsigned long lastRead = 0;
int readInterval = 100;

void setup()
{
    Serial.begin(9600);
    ultrasonic.attach(trigPin, echoPin);
    screen.begin();
    screen.play(idleAnim, LOOP);
    Serial.println("Distance Threshold Demo");
    Serial.println("Move hand to switch animations:");
    Serial.println("  Far (>60cm): idle slow");
    Serial.println("  Mid (30-60cm): idle fast");  
    Serial.println("  Close (<30cm): go");
}

void loop()
{
    if (millis() - lastRead > readInterval)
    {
        float dist = ultrasonic.getDistanceCM();
        
        if (dist > 0)
        {
            int newMode;
            
            // Determine mode based on thresholds
            if (dist > FAR_THRESHOLD)
            {
                newMode = 0;  // Far away = idle slow
            } else if (dist > CLOSE_THRESHOLD)
            {
                newMode = 1;  // Middle zone = idle fast
            }
            else
            {
                newMode = 2;  // Close = go
            }
            
            // Only switch animation when mode changes
            if (newMode != currentMode)
            {
                currentMode = newMode;
                
                switch (currentMode)
                {
                    case 0: 
                        screen.play(idleAnim, LOOP);
                        screen.setSpeed(1.0);
                        Serial.println("FAR - idle (slow)");
                        break;
                    case 1: 
                        screen.play(idleAnim, LOOP);
                        screen.setSpeed(2.0);
                        Serial.println("MID - idle (fast)");
                        break;
                    case 2: 
                        screen.play(goAnim, LOOP);
                        screen.setSpeed(1.0);
                        Serial.println("CLOSE - go");
                        break;
                }
            }
        }
        lastRead = millis();
    }
    
    screen.update();
}
