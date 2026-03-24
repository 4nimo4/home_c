/*
 * TinyFilmFestival - Drawing Stress Test
 * 
 * Comprehensive test of Canvas Mode drawing primitives.
 * Cycles through various scenarios to verify buffered drawing.
 * 
 * Hardware: Arduino UNO R4 WiFi
 */

#include "TinyFilmFestival.h"

TinyScreen screen;

// Animation variables
int posX = 3;
int posDir = 1;
int testNum = 12;  // Start at test 12
unsigned long lastTestSwitch = 0;
unsigned long lastFrame = 0;
int FRAME_DELAY = 60;  // Animation frame rate
int TEST_DURATION = 4000;  // 4 seconds per test
int NUM_TESTS = 15;

void setup()
{
    Serial.begin(115200);
    screen.begin();
    
    Serial.println("=== Drawing Stress Test ===");
    Serial.println("Cycles through 14 different drawing scenarios");
    printTestName(0);
}

void printTestName(int test)
{
    Serial.print("Test ");
    Serial.print(test);
    Serial.print(": ");
    switch (test)
    {
        case 0: Serial.println("Filled circle - normal"); break;
        case 1: Serial.println("Filled circle - inverted"); break;
        case 2: Serial.println("Stroked circle - normal"); break;
        case 3: Serial.println("Stroked circle - inverted"); break;
        case 4: Serial.println("Moving lines - normal"); break;
        case 5: Serial.println("Moving lines - inverted"); break;
        case 6: Serial.println("Moving points pattern"); break;
        case 7: Serial.println("Complex: multiple shapes"); break;
        case 8: Serial.println("Text with shapes"); break;
        case 9: Serial.println("Text inverted"); break;
        case 10: Serial.println("Scrolling text LEFT"); break;
        case 11: Serial.println("Scrolling text RIGHT"); break;
        case 12: Serial.println("Text size 2 (double)"); break;
        case 13: Serial.println("Rotation (cycles 0/90/180/270)"); break;
        case 14: Serial.println("Rotation with text"); break;
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
        screen.resetScroll();  // Reset scroll for scrolling tests
        screen.setTextSize(1);  // Reset text size
        screen.setRotation(0);  // Reset rotation
    }
    
    screen.beginDraw();
    
    switch (testNum)
    {
        case 0: testFilledCircleNormal(); break;
        case 1: testFilledCircleInverted(); break;
        case 2: testStrokedCircleNormal(); break;
        case 3: testStrokedCircleInverted(); break;
        case 4: testLinesNormal(); break;
        case 5: testLinesInverted(); break;
        case 6: testPointsPattern(); break;
        case 7: testComplexShapes(); break;
        case 8: testTextWithShapes(); break;
        case 9: testTextInverted(); break;
        case 10: testScrollingTextLeft(); break;
        case 11: testScrollingTextRight(); break;
        case 12: testTextSizeDouble(); break;
        case 13: testRotation(); break;
        case 14: testRotationWithText(); break;
    }
    
    screen.endDraw();
    
    // Update animation position at controlled rate
    if (millis() - lastFrame >= FRAME_DELAY)
    {
        lastFrame = millis();
        posX += posDir;
        if (posX >= 9 || posX <= 2)
        {
            posDir *= -1;
        }
    }
}

// Test 0: Filled circle - normal (dark bg, bright circle)
void testFilledCircleNormal()
{
    screen.background(OFF);
    screen.fill(ON);
    screen.noStroke();
    screen.circle(posX, 4, 6);
}

// Test 1: Filled circle - inverted (bright bg, dark circle)
void testFilledCircleInverted()
{
    screen.background(ON);
    screen.fill(OFF);
    screen.noStroke();
    screen.circle(posX, 4, 6);
}

// Test 2: Stroked circle - normal
void testStrokedCircleNormal()
{
    screen.background(OFF);
    screen.stroke(ON);
    screen.noFill();
    screen.circle(posX, 4, 6);
}

// Test 3: Stroked circle - inverted
void testStrokedCircleInverted()
{
    screen.background(ON);
    screen.stroke(OFF);
    screen.noFill();
    screen.circle(posX, 4, 6);
}

// Test 4: Moving lines - normal
void testLinesNormal()
{
    screen.background(OFF);
    screen.stroke(ON);
    // Diagonal lines that move
    screen.line(posX - 2, 0, posX + 2, 7);
    screen.line(posX + 2, 0, posX - 2, 7);
    // Static reference lines
    screen.line(0, 4, 2, 4);
    screen.line(9, 4, 11, 4);
}

// Test 5: Moving lines - inverted
void testLinesInverted()
{
    screen.background(ON);
    screen.stroke(OFF);
    screen.line(posX - 2, 0, posX + 2, 7);
    screen.line(posX + 2, 0, posX - 2, 7);
    screen.line(0, 4, 2, 4);
    screen.line(9, 4, 11, 4);
}

// Test 6: Moving points pattern
void testPointsPattern()
{
    screen.background(OFF);
    screen.stroke(ON);
    
    // Moving vertical line of points
    for (int y = 0; y < 8; y++)
    {
        screen.point(posX, y);
    }
    
    // Static corner points
    screen.point(0, 0);
    screen.point(11, 0);
    screen.point(0, 7);
    screen.point(11, 7);
    
    // Alternating row based on position
    for (int x = 0; x < 12; x += 2)
    {
        screen.point(x + (posX % 2), 3);
    }
}

// Test 7: Complex shapes with multiple fill/stroke combinations
void testComplexShapes()
{
    screen.background(OFF);
    
    // Filled rectangle on left
    screen.fill(ON);
    screen.noStroke();
    screen.rect(0, 1, 4, 6);
    
    // Stroked rectangle on right
    screen.noFill();
    screen.stroke(ON);
    screen.rect(8, 1, 4, 6);
    
    // Moving filled circle in middle (cuts through if overlapping)
    screen.fill(ON);
    screen.noStroke();
    screen.circle(posX, 4, 4);
    
    // Diagonal line across everything
    screen.stroke(ON);
    screen.line(0, 7, 11, 0);
}

// Test 8: Text combined with shapes
void testTextWithShapes()
{
    screen.background(OFF);
    screen.stroke(ON);
    
    // Draw a frame
    screen.noFill();
    screen.rect(0, 0, 12, 8);
    
    // Moving vertical line
    screen.line(posX, 1, posX, 6);
    
    // Draw text "Hi" using buffered text
    screen.text("Hi", 1, 1);
}

// Test 9: Inverted text effect
void testTextInverted()
{
    screen.background(ON);
    screen.stroke(OFF);
    
    // Draw inverted text
    screen.text("AB", posX - 2, 1);
    
    // Add corner markers
    screen.point(0, 0);
    screen.point(11, 0);
    screen.point(0, 7);
    screen.point(11, 7);
}

// Test 10: Scrolling text left
void testScrollingTextLeft()
{
    screen.background(OFF);
    screen.stroke(ON);
    screen.setScrollSpeed(60);
    
    // Scroll "HELLO" left
    screen.scrollText("HELLO", 1, SCROLL_LEFT);
}

// Test 11: Scrolling text right
void testScrollingTextRight()
{
    screen.background(OFF);
    screen.stroke(ON);
    screen.setScrollSpeed(60);
    
    // Scroll "WORLD" right
    screen.scrollText("WORLD", 1, SCROLL_RIGHT);
}

// Test 12: Text size 2 (double size)
void testTextSizeDouble()
{
    screen.background(OFF);
    screen.stroke(ON);
    screen.setTextSize(2);
    
    // Single large letter "A" - will be 6x10 but display is 8 tall
    // So we'll see top 8 rows of the 10-pixel-tall letter
    screen.text("A", 3, 0);
}

// Test 13: Rotation demo - cycles through 0, 90, 180, 270 degrees
void testRotation()
{
    // Cycle rotation every 1 second (based on millis)
    int rotationStep = ((millis() - lastTestSwitch) / 1000) % 4;
    screen.setRotation(rotationStep * 90);
    
    screen.background(OFF);
    screen.stroke(ON);
    
    // Draw an arrow pointing right (will rotate with display)
    // Arrow body
    screen.line(2, 4, 8, 4);
    // Arrow head
    screen.line(8, 4, 6, 2);
    screen.line(8, 4, 6, 6);
    
    // Corner marker at top-left (shows orientation)
    screen.point(0, 0);
    screen.point(1, 0);
    screen.point(0, 1);
}

// Test 14: Rotation with text
void testRotationWithText()
{
    // Cycle rotation every 1 second
    int rotationStep = ((millis() - lastTestSwitch) / 1000) % 4;
    screen.setRotation(rotationStep * 90);
    
    screen.background(OFF);
    screen.stroke(ON);
    
    // Draw "Hi" text - will rotate with display
    screen.text("Hi", 3, 1);
    
    // Corner marker
    screen.point(0, 0);
}
