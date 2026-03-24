/*
 * TinyFilmFestival V2 - Interactive Character
 * 06_Projects/InteractiveCharacter
 * 
 * A complete project combining Animation Mode, Canvas Mode, and Sensors.
 * A character on the LED matrix reacts to proximity:
 * 
 *   FAR (>60cm):      Sleeping - slow idle animation, dim
 *   MEDIUM (30-60cm): Alert - normal speed idle
 *   CLOSE (15-30cm):  Excited - fast "go" animation + sparkle overlay
 *   VERY CLOSE (<15cm): Overwhelmed - animation pauses, warning flash
 * 
 * This example demonstrates:
 *   - Multi-zone sensor interaction
 *   - Animation switching based on state
 *   - Speed control based on proximity
 *   - Hybrid overlay effects (sparkles via beginOverlay/endOverlay)
 *   - Complete project structure with TinyScreen
 * 
 * Hardware:
 *   - Arduino UNO R4 WiFi (built-in 12×8 LED Matrix)
 *   - HC-SR04 Ultrasonic Distance Sensor
 *     - Trigger: Pin 14 (A0)
 *     - Echo: Pin 15 (A1)
 * 
 * Libraries Required:
 *   - TinyFilmFestival
 *   - HCSR04 (install from Library Manager)
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
#include "HCSR04.h"
#include "idle.h"
#include "go.h"

// === Hardware Setup ===
UltraSonicDistanceSensor sensor(14, 15, 200);  // trigger, echo, maxDistance

// === TinyScreen - Unified Interface ===
TinyScreen screen;
Animation idleAnim = idle;
Animation goAnim = go;

// === Character States ===
enum CharacterState {
    SLEEPING,
    ALERT,
    EXCITED,
    OVERWHELMED
};

CharacterState currentState = SLEEPING;
CharacterState previousState = SLEEPING;

// === Timing ===
unsigned long lastSensorRead = 0;
int SENSOR_INTERVAL = 100;  // Read sensor every 100ms

unsigned long lastSparkle = 0;
int SPARKLE_INTERVAL = 50;

unsigned long lastFlash = 0;
bool flashOn = false;

// === Sparkle overlay positions ===
int sparkleX = 0;
int sparkleY = 0;

void setup()
{
    Serial.begin(9600);
    screen.begin();
    
    // Start in sleeping state
    screen.play(idleAnim, LOOP);
    screen.setSpeed(300);  // Very slow for sleeping
    
    Serial.println("=== Interactive Character ===");
    Serial.println("Approach the sensor to wake the character!");
}

void loop()
{
    // Read distance sensor
    float distance = readDistance();
    
    // Determine character state based on distance
    CharacterState newState = determineState(distance);
    
    // Handle state changes
    if (newState != currentState)
    {
        previousState = currentState;
        currentState = newState;
        onStateChange();
    }
    
    // Draw overlays (this also updates the animation)
    drawOverlay();
    
    // Draw overlays based on state
    drawOverlay();
    
    // Debug output
    static unsigned long lastDebug = 0;
    if (millis() - lastDebug > 500)
    {
        Serial.print("Distance: ");
        Serial.print(distance);
        Serial.print(" cm, State: ");
        Serial.println(getStateName(currentState));
        lastDebug = millis();
    }
}

float readDistance()
{
    static float lastDistance = 100;
    
    if (millis() - lastSensorRead >= SENSOR_INTERVAL)
    {
        float d = sensor.measureDistanceCm();
        if (d > 0)
        {
            lastDistance = d;
        }
        lastSensorRead = millis();
    }
    return lastDistance;
}

CharacterState determineState(float distance)
{
    if (distance < 15)
    {
        return OVERWHELMED;
    } else if (distance < 30)
    {
        return EXCITED;
    } else if (distance < 60)
    {
        return ALERT;
    }
    else
    {
        return SLEEPING;
    }
}

void onStateChange()
{
    switch (currentState)
    {
        case SLEEPING:
            screen.play(idleAnim, LOOP);
            screen.setSpeed(300);  // Very slow
            break;
            
        case ALERT:
            screen.play(idleAnim, LOOP);
            screen.setSpeed(100);  // Normal speed
            break;
            
        case EXCITED:
            screen.play(goAnim, LOOP);
            screen.setSpeed(50);  // Fast!
            break;
            
        case OVERWHELMED:
            screen.pause();  // Too close! Freeze up
            break;
    }
}

void drawOverlay()
{
    // beginOverlay updates animation AND starts drawing mode
    screen.beginOverlay();
    
    switch (currentState)
    {
        case SLEEPING:
            // Draw "Zzz" indicator in corner
            screen.point(10, 0);
            if ((millis() / 500) % 2 == 0)
            {
                screen.point(11, 1);
            }
            break;
            
        case ALERT:
            // Draw subtle corner dots to show awareness
            screen.point(0, 0);
            screen.point(11, 0);
            break;
            
        case EXCITED:
            // Draw random sparkles around the character
            if (millis() - lastSparkle >= SPARKLE_INTERVAL)
            {
                sparkleX = random(0, 12);
                sparkleY = random(0, 8);
                lastSparkle = millis();
            }
            screen.point(sparkleX, sparkleY);
            // Extra sparkle
            screen.point((sparkleX + 5) % 12, (sparkleY + 3) % 8);
            break;
            
        case OVERWHELMED:
            // Flash warning border
            if (millis() - lastFlash >= 100)
            {
                flashOn = !flashOn;
                lastFlash = millis();
            }
            if (flashOn)
            {
                // Draw border
                screen.line(0, 0, 11, 0);   // Top
                screen.line(0, 7, 11, 7);   // Bottom
                screen.line(0, 0, 0, 7);    // Left
                screen.line(11, 0, 11, 7);  // Right
            }
            break;
    }
    
    screen.endOverlay();
}

const char* getStateName(CharacterState state)
{
    switch (state)
    {
        case SLEEPING: return "SLEEPING";
        case ALERT: return "ALERT";
        case EXCITED: return "EXCITED";
        case OVERWHELMED: return "OVERWHELMED";
        default: return "UNKNOWN";
    }
}
