/*
 * TinyFilmFestival - Playback Control Test
 * 
 * Focused test for play/pause/resume and animation switching.
 * Uses button-style timing to clearly demonstrate each action.
 * 
 * Hardware: Arduino UNO R4 WiFi
 * 
 * Sequence (repeats):
 *   0-2s:   Play animation 1
 *   2-4s:   PAUSED
 *   4-6s:   RESUMED
 *   6-8s:   Switch to animation 2
 *   8-10s:  PAUSED
 *   10-12s: RESUMED
 *   12s:    Restart cycle
 */

#include "TinyFilmFestival.h"
#include "idle.h"
#include "landscape.h"

TinyScreen screen;
Animation anim1 = idle;
Animation anim2 = landscape;

// Timing
unsigned long cycleStart = 0;
const unsigned long CYCLE_LENGTH = 12000;  // 12 second cycle

// State tracking to only trigger once
int lastPhase = -1;

void setup()
{
    Serial.begin(115200);
    screen.begin();
    
    Serial.println("=== Playback Control Test ===");
    Serial.println("Testing: play / pause / resume / switch");
    Serial.println();
    Serial.println("Cycle timing:");
    Serial.println("  0-2s:  Animation 1 playing");
    Serial.println("  2-4s:  PAUSED");
    Serial.println("  4-6s:  RESUMED");
    Serial.println("  6-8s:  Animation 2 playing");
    Serial.println("  8-10s: PAUSED");
    Serial.println("  10-12s: RESUMED");
    Serial.println();
    
    // Start playing animation 1
    screen.play(anim1, LOOP);
    screen.setSpeed(100);
    cycleStart = millis();
    
    Serial.println(">> PLAY Animation 1");
}

void loop()
{
    unsigned long elapsed = millis() - cycleStart;
    
    // Determine current phase (0-5)
    int phase = elapsed / 2000;
    
    // Reset cycle
    if (elapsed >= CYCLE_LENGTH)
    {
        cycleStart = millis();
        lastPhase = -1;
        screen.play(anim1, LOOP);
        screen.setSpeed(100);
        Serial.println("\n--- CYCLE RESTART ---");
        Serial.println(">> PLAY Animation 1");
        return;
    }
    
    // Only act when phase changes
    if (phase != lastPhase)
    {
        lastPhase = phase;
        
        switch (phase)
        {
            case 0:
                // Already playing from setup/restart
                break;
                
            case 1:
                // Pause animation 1
                screen.pause();
                Serial.println(">> PAUSE (Animation 1 frozen)");
                break;
                
            case 2:
                // Resume animation 1
                screen.resume();
                Serial.println(">> RESUME (Animation 1 continues)");
                break;
                
            case 3:
                // Switch to animation 2
                screen.play(anim2, LOOP);
                screen.setSpeed(80);
                Serial.println(">> SWITCH to Animation 2");
                break;
                
            case 4:
                // Pause animation 2
                screen.pause();
                Serial.println(">> PAUSE (Animation 2 frozen)");
                break;
                
            case 5:
                // Resume animation 2
                screen.resume();
                Serial.println(">> RESUME (Animation 2 continues)");
                break;
        }
    }
    
    // Update display
    screen.update();
    
    // Print status every 500ms
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 500)
    {
        Serial.print("  [");
        Serial.print(elapsed / 1000.0, 1);
        Serial.print("s] Frame ");
        Serial.print(screen.getCurrentFrame());
        Serial.print(" | ");
        if (screen.isPaused())
        {
            Serial.println("PAUSED");
        }
        else
        {
            Serial.println("PLAYING");
        }
        lastPrint = millis();
    }
}
