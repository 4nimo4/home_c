/*
 * TinyFilmFestival - Animation Mode Stress Test
 * 
 * Comprehensive test of Animation Mode capabilities.
 * Cycles through various playback scenarios to verify functionality.
 * 
 * Hardware: Arduino UNO R4 WiFi
 */

#include "TinyFilmFestival.h"
#include "idle.h"
#include "landscape.h"

TinyScreen screen;
Animation anim1 = idle;
Animation anim2 = landscape;

// Test state
int testNum = 0;
unsigned long lastTestSwitch = 0;
int TEST_DURATION = 4000;  // 4 seconds per test
int NUM_TESTS = 12;

void setup()
{
    Serial.begin(115200);
    screen.begin();
    
    Serial.println("=== Animation Mode Stress Test ===");
    Serial.println("Cycles through 12 different animation scenarios");
    Serial.println("Watch Serial Monitor for test descriptions");
    Serial.println();
    
    printTestName(0);
    setupTest(0);
}

void printTestName(int test)
{
    Serial.print("\nTest ");
    Serial.print(test);
    Serial.print(": ");
    switch (test)
    {
        case 0:  Serial.println("Basic LOOP playback"); break;
        case 1:  Serial.println("ONCE playback (plays once, stops)"); break;
        case 2:  Serial.println("Speed change (fast -> slow -> fast)"); break;
        case 3:  Serial.println("Pause and Resume"); break;
        case 4:  Serial.println("Stop and Restart"); break;
        case 5:  Serial.println("Frame range (partial animation)"); break;
        case 6:  Serial.println("Animation switch"); break;
        case 7:  Serial.println("Invert ON"); break;
        case 8:  Serial.println("Invert OFF (normal)"); break;
        case 9:  Serial.println("Two layers combined"); break;
        case 10: Serial.println("Layer speed control"); break;
        case 11: Serial.println("Status reporting"); break;
    }
}

void loop()
{
    // Switch test every TEST_DURATION ms
    if (millis() - lastTestSwitch > TEST_DURATION)
    {
        testNum = (testNum + 1) % NUM_TESTS;
        lastTestSwitch = millis();
        printTestName(testNum);
        setupTest(testNum);
    }
    
    // Run current test's update logic
    runTest(testNum);
    
    // Always call update() for animation mode
    screen.update();
}

void setupTest(int test)
{
    // Reset state for each test
    screen.setInvert(false);
    screen.stopLayer(0);
    screen.stopLayer(1);
    
    switch (test)
    {
        case 0:  // Basic LOOP
            screen.play(anim1, LOOP);
            screen.setSpeed(100);
            break;
            
        case 1:  // ONCE playback
            screen.play(anim1, ONCE);
            screen.setSpeed(80);
            Serial.println("  -> Animation will play once then stop");
            break;
            
        case 2:  // Speed change
            screen.play(anim1, LOOP);
            screen.setSpeed(50);  // Start fast
            Serial.println("  -> Starting FAST (50ms)");
            break;
            
        case 3:  // Pause/Resume
            screen.play(anim1, LOOP);
            screen.setSpeed(100);
            Serial.println("  -> Will pause at 1s, resume at 2s");
            break;
            
        case 4:  // Stop/Restart
            screen.play(anim1, LOOP);
            screen.setSpeed(100);
            Serial.println("  -> Will stop at 1.5s, restart at 2.5s");
            break;
            
        case 5:  // Frame range
            // Play only frames 2-5 of the animation
            screen.play(anim1, LOOP, 2, 5);
            screen.setSpeed(150);
            Serial.println("  -> Playing frames 2-5 only");
            break;
            
        case 6:  // Animation switch
            screen.play(anim1, LOOP);
            screen.setSpeed(100);
            Serial.println("  -> Will switch to anim2 at 2s");
            break;
            
        case 7:  // Invert ON
            screen.play(anim1, LOOP);
            screen.setSpeed(100);
            screen.setInvert(true);
            Serial.println("  -> Display INVERTED");
            break;
            
        case 8:  // Invert OFF
            screen.play(anim1, LOOP);
            screen.setSpeed(100);
            screen.setInvert(false);
            Serial.println("  -> Display NORMAL");
            break;
            
        case 9:  // Two layers
            screen.play(anim1, LOOP);
            screen.setSpeed(120);
            screen.addLayer();
            screen.playOnLayer(1, anim2, LOOP);
            screen.setSpeedOnLayer(1, 100);
            Serial.println("  -> Two animations layered (OR combined)");
            break;
            
        case 10: // Layer speed control
            screen.play(anim1, LOOP);
            screen.setSpeed(200);  // Slow base
            screen.addLayer();
            screen.playOnLayer(1, anim2, LOOP);
            screen.setSpeedOnLayer(1, 50);  // Fast overlay
            Serial.println("  -> Layer 0: SLOW (200ms), Layer 1: FAST (50ms)");
            break;
            
        case 11: // Status reporting
            screen.play(anim1, LOOP);
            screen.setSpeed(100);
            Serial.println("  -> Check Serial for status updates");
            break;
    }
}

void runTest(int test)
{
    unsigned long elapsed = millis() - lastTestSwitch;
    
    switch (test)
    {
        case 1:  // ONCE - report when complete
            if (screen.isComplete())
            {
                static bool reported = false;
                if (!reported)
                {
                    Serial.println("  -> Animation COMPLETE (stopped)");
                    reported = true;
                }
            }
            break;
            
        case 2:  // Speed change
            if (elapsed < 1500)
            {
                // Fast phase
            } else if (elapsed < 2500)
            {
                // Slow phase
                static bool slowSet = false;
                if (!slowSet)
                {
                    screen.setSpeed(300);
                    Serial.println("  -> Changed to SLOW (300ms)");
                    slowSet = true;
                }
            }
            else
            {
                // Fast again
                static bool fastAgain = false;
                if (!fastAgain)
                {
                    screen.setSpeed(50);
                    Serial.println("  -> Back to FAST (50ms)");
                    fastAgain = true;
                }
            }
            break;
            
        case 3:  // Pause/Resume
            if (elapsed > 1000 && elapsed < 2000)
            {
                static bool paused = false;
                if (!paused)
                {
                    screen.pause();
                    Serial.println("  -> PAUSED");
                    paused = true;
                }
            } else if (elapsed >= 2000)
            {
                static bool resumed = false;
                if (!resumed)
                {
                    screen.resume();
                    Serial.println("  -> RESUMED");
                    resumed = true;
                }
            }
            break;
            
        case 4:  // Stop/Restart
            if (elapsed > 1500 && elapsed < 2500)
            {
                static bool stopped = false;
                if (!stopped)
                {
                    screen.stop();
                    Serial.println("  -> STOPPED");
                    stopped = true;
                }
            } else if (elapsed >= 2500)
            {
                static bool restarted = false;
                if (!restarted)
                {
                    screen.play(anim1, LOOP);
                    screen.setSpeed(100);
                    Serial.println("  -> RESTARTED");
                    restarted = true;
                }
            }
            break;
            
        case 6:  // Animation switch
            if (elapsed > 2000)
            {
                static bool switched = false;
                if (!switched)
                {
                    screen.play(anim2, LOOP);
                    screen.setSpeed(80);
                    Serial.println("  -> Switched to animation 2");
                    switched = true;
                }
            }
            break;
            
        case 11: // Status reporting
            static unsigned long lastStatus = 0;
            if (millis() - lastStatus > 500)
            {
                Serial.print("  Frame: ");
                Serial.print(screen.getCurrentFrame());
                Serial.print("/");
                Serial.print(screen.getTotalFrames());
                Serial.print(" | Speed: ");
                Serial.print(screen.getCurrentSpeed());
                Serial.print("ms | Playing: ");
                Serial.print(screen.isPlaying() ? "YES" : "NO");
                Serial.print(" | Paused: ");
                Serial.println(screen.isPaused() ? "YES" : "NO");
                lastStatus = millis();
            }
            break;
    }
}
