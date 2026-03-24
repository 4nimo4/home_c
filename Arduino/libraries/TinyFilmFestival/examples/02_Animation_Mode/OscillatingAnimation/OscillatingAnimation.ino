/*
 * OscillatingAnimation Example
 * 
 * Plays a looping animation and uses oscillateInt() to
 * smoothly slide it back and forth across the LED matrix.
 * 
 * Methods demonstrated:
 *   play()              — start animation in LOOP mode
 *   setPosition(x, y)   — move animation on the display
 *   oscillateInt()       — smooth sine-wave cyclic motion
 *   update()             — refresh the display each frame
 */

#include "TinyFilmFestival.h"
#include "animation.h"

TinyScreen screen;
Animation anim = animation;

void setup()
{
    screen.begin();
    screen.play(anim, LOOP);
}

void loop()
{
    // Oscillate X position across the display (2-second cycle)
    int x = oscillateInt(-2, 6, 2000);

    screen.setPosition(x, 0);
    screen.update();
}
