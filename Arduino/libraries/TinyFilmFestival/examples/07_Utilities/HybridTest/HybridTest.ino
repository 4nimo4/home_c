/*
 * TinyFilmFestival - Hybrid Mode Stress Test
 * 
 * Comprehensive test of Hybrid Mode: animations with canvas overlays.
 * Tests various combinations of animation playback with drawing on top.
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
int TEST_DURATION = 5000;  // 5 seconds per test
int NUM_TESTS = 9;

// Animation variables
int posX = 3;
int posDir = 1;
int indicatorLevel = 0;
int indicatorDir = 1;
unsigned long lastIndicatorUpdate = 0;

void setup()
{
    Serial.begin(115200);
    screen.begin();
    
    Serial.println("=== Hybrid Mode Stress Test ===");
    Serial.println("Cycles through 8 different hybrid scenarios");
    printTestName(0);
    
    // Start with first animation
    screen.play(anim1, LOOP);
    screen.setSpeed(120);
}

void printTestName(int test)
{
    Serial.print("Test ");
    Serial.print(test);
    Serial.print(": ");
    switch (test)
    {
        case 0: Serial.println("Animation + moving point"); break;
        case 1: Serial.println("Animation + indicator bar"); break;
        case 2: Serial.println("Animation + corner frame"); break;
        case 3: Serial.println("Animation + moving line"); break;
        case 4: Serial.println("Animation + text overlay"); break;
        case 5: Serial.println("Animation + inverted rect"); break;
        case 6: Serial.println("Animation switch + overlay"); break;
        case 7: Serial.println("Paused animation + drawing"); break;
        case 8: Serial.println("Inverted animation + overlay"); break;
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
    
    // Update indicator (for tests that use it)
    if (millis() - lastIndicatorUpdate >= 100)
    {
        indicatorLevel += indicatorDir;
        if (indicatorLevel >= 7 || indicatorLevel <= 0)
        {
            indicatorDir *= -1;
        }
        posX += posDir;
        if (posX >= 10 || posX <= 1)
        {
            posDir *= -1;
        }
        lastIndicatorUpdate = millis();
    }
    
    // Run current test
    switch (testNum)
    {
        case 0: testMovingPoint(); break;
        case 1: testIndicatorBar(); break;
        case 2: testCornerFrame(); break;
        case 3: testMovingLine(); break;
        case 4: testTextOverlay(); break;
        case 5: testInvertedRect(); break;
        case 6: testAnimationSwitch(); break;
        case 7: testPausedWithDrawing(); break;
        case 8: testInvertedAnimation(); break;
    }
}

void setupTest(int test)
{
    // Reset invert for all tests except test 8
    if (test != 8)
    {
        screen.setInvert(false);
    }
    
    // Setup for specific tests
    switch (test)
    {
        case 6:
            // Switch to second animation
            screen.play(anim2, LOOP);
            screen.setSpeed(100);
            break;
        case 7:
            // Pause animation for drawing test
            screen.play(anim1, LOOP);
            screen.pause();
            break;
        case 8:
            // Inverted animation
            screen.play(anim1, LOOP);
            screen.setSpeed(120);
            screen.setInvert(true);
            break;
        default:
            // Resume/restart first animation
            if (screen.isPaused())
            {
                screen.resume();
            } else if (test != 6)
            {
                screen.play(anim1, LOOP);
                screen.setSpeed(120);
            }
            break;
    }
}

// Test 0: Animation with a single moving point
void testMovingPoint()
{
    screen.beginOverlay();
    screen.stroke(ON);
    screen.point(posX, 4);
    screen.endOverlay();
}

// Test 1: Animation with indicator bar on right edge
void testIndicatorBar()
{
    screen.beginOverlay();
    screen.stroke(ON);
    
    // Draw indicator bar on the right edge
    for (int y = 7; y > 7 - indicatorLevel; y--)
    {
        screen.point(11, y);
    }
    screen.endOverlay();
}

// Test 2: Animation with corner brackets frame
void testCornerFrame()
{
    screen.beginOverlay();
    screen.stroke(ON);
    
    // Top-left corner
    screen.point(0, 0);
    screen.point(1, 0);
    screen.point(0, 1);
    
    // Top-right corner
    screen.point(11, 0);
    screen.point(10, 0);
    screen.point(11, 1);
    
    // Bottom-left corner
    screen.point(0, 7);
    screen.point(1, 7);
    screen.point(0, 6);
    
    // Bottom-right corner
    screen.point(11, 7);
    screen.point(10, 7);
    screen.point(11, 6);
    
    screen.endOverlay();
}

// Test 3: Animation with moving diagonal line
void testMovingLine()
{
    screen.beginOverlay();
    screen.stroke(ON);
    screen.line(posX - 2, 0, posX + 2, 7);
    screen.endOverlay();
}

// Test 4: Animation with text overlay
void testTextOverlay()
{
    screen.beginOverlay();
    screen.stroke(ON);
    screen.text("Hi", 0, 1);
    screen.endOverlay();
}

// Test 5: Animation with inverted rectangle (erase part of animation)
void testInvertedRect()
{
    screen.beginOverlay();
    
    // Draw a small filled OFF rectangle to "cut out" part of animation
    screen.fill(OFF);
    screen.noStroke();
    screen.rect(posX - 1, 2, 4, 4);
    
    // Add a bright border
    screen.stroke(ON);
    screen.noFill();
    screen.rect(posX - 2, 1, 6, 6);
    
    screen.endOverlay();
}

// Test 6: Switch animation while overlaying
void testAnimationSwitch()
{
    screen.beginOverlay();
    screen.stroke(ON);
    
    // Draw "2" to show we're on animation 2
    screen.text("2", 0, 0);
    
    // Moving indicator
    screen.point(posX, 7);
    screen.point(posX + 1, 7);
    
    screen.endOverlay();
}

// Test 7: Paused animation with active drawing
void testPausedWithDrawing()
{
    screen.beginOverlay();
    screen.stroke(ON);
    
    // Draw moving shapes over frozen animation
    screen.circle(posX, 4, 4);
    
    // Show "P" for paused
    screen.text("P", 0, 0);
    
    screen.endOverlay();
}

// Test 8: Inverted animation with overlay
void testInvertedAnimation()
{
    screen.beginOverlay();
    screen.stroke(ON);
    
    // Draw "I" for inverted
    screen.text("I", 0, 0);
    
    // Moving point shows overlay still works
    screen.point(posX, 7);
    
    screen.endOverlay();
}
