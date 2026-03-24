/*
 * TinyFilmFestival V2 - Layered Animations
 * 02_Animation_Mode/LayeredAnimations
 * 
 * Play multiple animations simultaneously using TinyScreen layers.
 * Each layer runs independently with its own speed and playback mode.
 * Layers are combined (OR'd) when displayed.
 * 
 * Animation files (.h) are created using the Arduino LED Matrix Editor:
 * https://ledmatrix-editor.arduino.cc/
 * 
 * Hardware: Arduino UNO R4 WiFi (built-in 12×8 LED Matrix)
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
#include "landscape.h"
#include "fiz.h"

TinyScreen screen;

Animation bgAnim = landscape;   // Background layer
Animation fgAnim = fiz;         // Foreground layer

int fgLayer;  // Foreground layer index

void setup()
{
    Serial.begin(9600);
    screen.begin();
    
    // Layer 0 (primary) - Background, slow looping
    screen.play(bgAnim, LOOP);
    screen.setSpeed(150);
    
    // Add a second layer for foreground
    fgLayer = screen.addLayer();
    
    // Configure foreground layer (fast, boomerang)
    screen.playOnLayer(fgLayer, fgAnim, BOOMERANG);
    screen.setSpeedOnLayer(fgLayer, 60);
    
    Serial.println("Layered animation running");
}

void loop()
{
    // update() handles all layers automatically
    screen.update();
}
