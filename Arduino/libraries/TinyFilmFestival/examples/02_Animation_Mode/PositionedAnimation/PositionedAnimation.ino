/*
 * PositionedAnimation Example
 * 
 * Demonstrates dynamic x,y positioning of animation clips.
 * A small robot character bounces around the display using
 * oscillateInt() for smooth sinusoidal motion.
 * 
 * The animation is automatically clipped at the matrix edges —
 * pixels that move beyond the 12x8 boundary are simply not drawn,
 * and areas with no animation data remain off.
 * 
 * Methods demonstrated:
 *   setPosition(x, y)   — move animation on the display
 *   getPositionX()       — read current X position
 *   getPositionY()       — read current Y position
 *   oscillateInt()       — smooth cyclic motion
 */

#include "TinyFilmFestival.h"
#include "character.h"

TinyScreen screen;
Animation charAnim = character;

// Motion timing (milliseconds per full cycle)
int X_PERIOD = 3000;   // Horizontal oscillation period
int Y_PERIOD = 2000;   // Vertical oscillation period

void setup()
{
    screen.begin();
    screen.play(charAnim, LOOP);
}

void loop()
{
    // Smoothly oscillate position across the display
    // X range: -2 to 9  (partially clips at both edges)
    // Y range: -1 to 3  (partially clips at top, fits at bottom)
    int x = oscillateInt(-2, 9, X_PERIOD);
    int y = oscillateInt(-1, 3, Y_PERIOD, 0.25);  // Phase offset for diagonal motion

    screen.setPosition(x, y);
    screen.update();
}
