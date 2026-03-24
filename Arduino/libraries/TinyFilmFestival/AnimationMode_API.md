# Animation Mode API Reference

## Table of Contents
- [Overview](#overview)
- [Setup](#setup)
- [Creating Animations](#creating-animations)
  - [Using the LED Matrix Editor](#using-the-led-matrix-editor)
  - [Animation Header Structure](#animation-header-structure)
- [Playback Methods](#playback-methods)
  - [play()](#play)
  - [pause()](#pause)
  - [resume()](#resume)
  - [stop()](#stop)
  - [update()](#update)
- [Speed Control](#speed-control)
  - [setSpeed()](#setspeed)
  - [restoreOriginalSpeed()](#restoreoriginalspeed)
  - [getCurrentSpeed()](#getcurrentspeed)
- [Play Modes](#play-modes)
  - [LOOP](#loop)
  - [ONCE](#once)
  - [BOOMERANG](#boomerang)
- [Partial Playback](#partial-playback)
  - [Playing a Range of Frames](#playing-a-range-of-frames)
- [Status Methods](#status-methods)
  - [isPlaying()](#isplaying)
  - [isPaused()](#ispaused)
  - [isComplete()](#iscomplete)
  - [getState()](#getstate)
  - [getCurrentFrame()](#getcurrentframe)
  - [getTotalFrames()](#gettotalframes)
- [Layered Animations](#layered-animations)
  - [addLayer()](#addlayer)
  - [playOnLayer()](#playonlayer)
  - [setSpeedOnLayer()](#setspeedonlayer)
  - [pauseLayer()](#pauselayer)
  - [resumeLayer()](#resumelayer)
  - [stopLayer()](#stoplayer)
- [Animation Positioning](#animation-positioning)
  - [setPosition()](#setposition)
  - [setPositionOnLayer()](#setpositiononlayer)
  - [getPositionX() / getPositionY()](#getpositionx--getpositiony)
  - [getPositionXOnLayer() / getPositionYOnLayer()](#getpositionxonlayer--getpositionyonlayer)
- [Example Sketches](#example-sketches)
  - [Basic Animation](#basic-animation)
  - [Playback Control](#playback-control)
  - [Layered Animations](#layered-animations-example)
  - [Positioned Animation](#positioned-animation-example)

---

## Overview

Animation Mode plays pre-made frame-by-frame animations created with the [LED Matrix Editor](https://ledmatrix-editor.arduino.cc/). Think of it like a video player:

- **Load** animation frames from `.h` files
- **Play** with different modes (loop, once, boomerang)
- **Control** speed, pause, resume, and playback position
- **Layer** multiple animations on top of each other

This is ideal for:
- **Detailed pixel art** — characters, icons, patterns
- **Complex animations** — walk cycles, explosions, transitions
- **Artist-created content** — design visually, code the behavior

---

## Setup

```cpp
#include "TinyFilmFestival.h"
#include "myAnimation.h"       // From LED Matrix Editor

TinyScreen screen;
Animation anim = animation;    // 'animation' is defined in .h file

void setup() {
    screen.begin();
    screen.play(anim, LOOP);
}

void loop() {
    screen.update();           // REQUIRED - call every loop!
}
```

> **Important:** You must call `screen.update()` in your `loop()` function. This is what actually advances the animation frames.

---

## Creating Animations

### Using the LED Matrix Editor

1. Go to [ledmatrix-editor.arduino.cc](https://ledmatrix-editor.arduino.cc/)
2. Click pixels to turn them on/off
3. Add frames using the **+** button
4. Set timing for each frame (in milliseconds)
5. Click **Export** and choose **.h file**
6. Save to your Arduino sketch folder

![LED Matrix Editor](images/editor.png)

### Animation Header Structure

The exported `.h` file looks like this:

```cpp
const uint32_t animation[][4] = {
    { 0x0, 0x0, 0x0, 100 },           // Frame 1, 100ms
    { 0x3C3C3C, 0x0, 0x0, 100 },      // Frame 2, 100ms
    { 0x7E7E7E, 0x0, 0x0, 100 },      // Frame 3, 100ms
    // ... more frames
};
```

Each row is one frame:
- **First 3 values:** Pixel data (encoded as 32-bit integers)
- **4th value:** Frame duration in milliseconds

---

## Playback Methods

### `play()`

#### Description

Starts playing an animation with the specified mode.

#### Syntax

```cpp
screen.play(animation, mode)
screen.play(animation, mode, startFrame, endFrame)
```

#### Parameters

- `animation`: An `Animation` object loaded from a `.h` file
- `mode`: Playback mode — `LOOP`, `ONCE`, or `BOOMERANG`
- `startFrame`: Optional — first frame to play (1-indexed)
- `endFrame`: Optional — last frame to play (1-indexed)

#### Returns

Nothing

#### Example

```cpp
#include "TinyFilmFestival.h"
#include "idle.h"

TinyScreen screen;
Animation idleAnim = idle;

void setup() {
    screen.begin();
    screen.play(idleAnim, LOOP);    // Play forever
}

void loop() {
    screen.update();
}
```

---

### `pause()`

#### Description

Pauses the currently playing animation. The animation stays on the current frame.

#### Syntax

```cpp
screen.pause()
```

#### Parameters

None

#### Returns

Nothing

#### Example

```cpp
screen.play(myAnim, LOOP);

// Later...
screen.pause();       // Freezes on current frame
```

---

### `resume()`

#### Description

Resumes a paused animation from where it stopped.

#### Syntax

```cpp
screen.resume()
```

#### Parameters

None

#### Returns

Nothing

#### Example

```cpp
screen.pause();

// Later...
screen.resume();      // Continues from paused frame
```

---

### `stop()`

#### Description

Stops the animation completely and resets to idle state.

#### Syntax

```cpp
screen.stop()
```

#### Parameters

None

#### Returns

Nothing

#### Example

```cpp
screen.play(myAnim, ONCE);

// Later...
screen.stop();        // Stop and reset
```

---

### `update()`

#### Description

Updates the animation frame based on timing. **Must be called in your `loop()` function!**

#### Syntax

```cpp
screen.update()
```

#### Parameters

None

#### Returns

Nothing

#### Example

```cpp
void loop() {
    screen.update();   // Required for animation to play
    
    // Your other code here...
}
```

---

## Speed Control

### `setSpeed()`

#### Description

Sets a custom playback speed, overriding the per-frame timings from the `.h` file. You can provide milliseconds per frame or a multiplier.

#### Syntax

```cpp
screen.setSpeed(speedMs)
screen.setSpeed(speedMultiplier)
```

#### Parameters

- `speedMs`: Milliseconds per frame (smaller = faster). Use a negative value to play backward. Use `0` to pause.
- `speedMultiplier`: Playback multiplier (1.0 = original speed, 0.5 = half speed, 2.0 = double speed). Use a negative value to play backward. Use `0` to pause.

#### Returns

Nothing

#### Example

```cpp
screen.play(myAnim, LOOP);
screen.setSpeed(50);   // Very fast: 50ms per frame
screen.setSpeed(500);  // Slow: half second per frame
screen.setSpeed(-100); // Backward: 100ms per frame
screen.setSpeed(0.5);  // Half speed (multiplier)
screen.setSpeed(2.0);  // Double speed (multiplier)
```

---

### `restoreOriginalSpeed()`

#### Description

Returns to the original per-frame timing from the `.h` file.

#### Syntax

```cpp
screen.restoreOriginalSpeed()
```

#### Parameters

None

#### Returns

Nothing

#### Example

```cpp
screen.setSpeed(50);           // Speed up
// Later...
screen.restoreOriginalSpeed(); // Back to original timing
```

---

### `getCurrentSpeed()`

#### Description

Returns the current playback speed in milliseconds per frame.

#### Syntax

```cpp
screen.getCurrentSpeed()
```

#### Parameters

None

#### Returns

Integer: current speed in milliseconds

#### Example

```cpp
int speed = screen.getCurrentSpeed();
Serial.print("Speed: ");
Serial.println(speed);
```

---

## Play Modes

### `LOOP`

Plays the animation continuously, restarting from the beginning after the last frame.

```cpp
screen.play(myAnim, LOOP);
// Frame 1 → 2 → 3 → 4 → 1 → 2 → 3 → 4 → ...
```

---

### `ONCE`

Plays the animation once from start to end, then stops.

```cpp
screen.play(myAnim, ONCE);
// Frame 1 → 2 → 3 → 4 → (stops, isComplete() returns true)
```

---

### `BOOMERANG`

Plays forward, then backward, then forward again (ping-pong).

```cpp
screen.play(myAnim, BOOMERANG);
// Frame 1 → 2 → 3 → 4 → 3 → 2 → 1 → 2 → 3 → ...
```

---

## Partial Playback

### Playing a Range of Frames

You can play only a subset of frames from an animation:

#### Syntax

```cpp
screen.play(animation, mode, startFrame, endFrame)
```

#### Parameters

- `startFrame`: First frame to play (1-indexed)
- `endFrame`: Last frame to play (1-indexed)

#### Example

```cpp
// Animation has 10 frames
// Play only frames 3-7
screen.play(myAnim, LOOP, 3, 7);
// Frame 3 → 4 → 5 → 6 → 7 → 3 → 4 → ...
```

This is useful for:
- **Sprite sheets** — multiple animations in one file
- **Segments** — play intro, loop, outro separately
- **Testing** — isolate specific frames

---

## Status Methods

### `isPlaying()`

#### Description

Returns true if an animation is currently playing (not paused or stopped).

#### Syntax

```cpp
screen.isPlaying()
```

#### Returns

Boolean: true if playing

#### Example

```cpp
if (screen.isPlaying()) {
    Serial.println("Animation is running");
}
```

---

### `isPaused()`

#### Description

Returns true if the animation is paused.

#### Syntax

```cpp
screen.isPaused()
```

#### Returns

Boolean: true if paused

#### Example

```cpp
if (screen.isPaused()) {
    Serial.println("Animation is paused");
}
```

---

### `isComplete()`

#### Description

Returns true if a ONCE-mode animation has finished playing.

#### Syntax

```cpp
screen.isComplete()
```

#### Returns

Boolean: true if complete

#### Example

```cpp
screen.play(introAnim, ONCE);

void loop() {
    screen.update();
    
    if (screen.isComplete()) {
        screen.play(mainAnim, LOOP);  // Switch to main animation
    }
}
```

---

### `getState()`

#### Description

Returns the current animation state as an enum value.

#### Syntax

```cpp
screen.getState()
```

#### Returns

`AnimationState` enum: `IDLE`, `PLAYING`, `PAUSED`, or `COMPLETED`

#### Example

```cpp
switch (screen.getState()) {
    case IDLE:
        Serial.println("No animation");
        break;
    case PLAYING:
        Serial.println("Playing");
        break;
    case PAUSED:
        Serial.println("Paused");
        break;
    case COMPLETED:
        Serial.println("Finished");
        break;
}
```

---

### `getCurrentFrame()`

#### Description

Returns the current frame number (1-indexed).

#### Syntax

```cpp
screen.getCurrentFrame()
```

#### Returns

Integer: current frame number (starting from 1)

#### Example

```cpp
Serial.print("Frame: ");
Serial.print(screen.getCurrentFrame());
Serial.print(" of ");
Serial.println(screen.getTotalFrames());
```

---

### `getTotalFrames()`

#### Description

Returns the total number of frames in the current animation.

#### Syntax

```cpp
screen.getTotalFrames()
```

#### Returns

Integer: total frame count

#### Example

```cpp
int total = screen.getTotalFrames();
Serial.print("Animation has ");
Serial.print(total);
Serial.println(" frames");
```

---

## Layered Animations

Stack multiple animations on top of each other! Lit pixels from higher layers override lower layers.

### `addLayer()`

#### Description

Adds a new animation layer. You can have up to 4 layers (plus the default layer 0).

#### Syntax

```cpp
screen.addLayer()
```

#### Parameters

None

#### Returns

Integer: the new layer index, or -1 if maximum layers reached

#### Example

```cpp
screen.begin();
int fgLayer = screen.addLayer();  // Returns 1
int effectLayer = screen.addLayer();  // Returns 2
```

---

### `playOnLayer()`

#### Description

Starts an animation on a specific layer.

#### Syntax

```cpp
screen.playOnLayer(layer, animation, mode)
screen.playOnLayer(layer, animation, mode, startFrame, endFrame)
```

#### Parameters

- `layer`: Layer index (0 is the default/background layer)
- `animation`: Animation to play
- `mode`: Playback mode
- `startFrame`, `endFrame`: Optional frame range

#### Returns

Nothing

#### Example

```cpp
TinyScreen screen;
Animation bgAnim = landscape;
Animation fgAnim = character;

void setup() {
    screen.begin();
    screen.addLayer();  // Add layer 1
    
    screen.play(bgAnim, LOOP);              // Layer 0 (background)
    screen.playOnLayer(1, fgAnim, LOOP);    // Layer 1 (foreground)
}

void loop() {
    screen.update();  // Updates all layers
}
```

---

### `setSpeedOnLayer()`

#### Description

Sets playback speed for a specific layer. You can provide milliseconds per frame or a multiplier.

#### Syntax

```cpp
screen.setSpeedOnLayer(layer, speedMs)
screen.setSpeedOnLayer(layer, speedMultiplier)
```

#### Parameters

- `layer`: Layer index
- `speedMs`: Milliseconds per frame. Use a negative value to play backward. Use `0` to pause.
- `speedMultiplier`: Playback multiplier (1.0 = original speed, 0.5 = half speed, 2.0 = double speed). Use a negative value to play backward. Use `0` to pause.

#### Returns

Nothing

#### Example

```cpp
screen.setSpeedOnLayer(1, 50);   // Speed up layer 1
screen.setSpeedOnLayer(1, -80);  // Play layer 1 backward
screen.setSpeedOnLayer(1, 0.5);  // Half speed (multiplier)
```

---

### `pauseLayer()`

#### Description

Pauses animation on a specific layer.

#### Syntax

```cpp
screen.pauseLayer(layer)
```

#### Parameters

- `layer`: Layer index to pause

#### Returns

Nothing

#### Example

```cpp
screen.pauseLayer(1);  // Pause foreground, background keeps playing
```

---

### `resumeLayer()`

#### Description

Resumes a paused layer.

#### Syntax

```cpp
screen.resumeLayer(layer)
```

#### Parameters

- `layer`: Layer index to resume

#### Returns

Nothing

---

### `stopLayer()`

#### Description

Stops animation on a specific layer.

#### Syntax

```cpp
screen.stopLayer(layer)
```

#### Parameters

- `layer`: Layer index to stop

#### Returns

Nothing

---

## Animation Positioning

Dynamically position an animation clip anywhere on the display. Pixels that move beyond the 12×8 matrix edges are automatically clipped, and areas with no animation data remain off.

This is ideal for:
- **Moving characters** — animate a small sprite, position it dynamically
- **Scrolling effects** — slide animations on/off screen
- **Sensor-driven placement** — map sensor input to animation position
- **Layered scenes** — position foreground characters over background animations

### `setPosition()`

#### Description

Sets the x,y position offset for the primary animation layer (layer 0). The animation is shifted by the given amount — positive x moves right, positive y moves down. Negative values shift left/up, clipping pixels at the edges.

The position persists across `play()` and `stop()` calls until you change it.

#### Syntax

```cpp
screen.setPosition(x, y)
```

#### Parameters

- `x`: Horizontal offset (positive = right, negative = left)
- `y`: Vertical offset (positive = down, negative = up)

#### Returns

Nothing

#### Example

```cpp
screen.setPosition(3, 2);    // Shift animation right 3, down 2
screen.setPosition(-2, 0);   // Shift left 2 (left columns clipped)
screen.setPosition(0, 0);    // Reset to default position
```

---

### `setPositionOnLayer()`

#### Description

Sets the x,y position offset for a specific animation layer.

#### Syntax

```cpp
screen.setPositionOnLayer(layer, x, y)
```

#### Parameters

- `layer`: Layer index (0 is the default/background layer)
- `x`: Horizontal offset
- `y`: Vertical offset

#### Returns

Nothing

#### Example

```cpp
screen.setPositionOnLayer(1, 5, 0);  // Move layer 1 right by 5
```

---

### `getPositionX()` / `getPositionY()`

#### Description

Returns the current position offset of the primary layer.

#### Syntax

```cpp
screen.getPositionX()
screen.getPositionY()
```

#### Parameters

None

#### Returns

Integer: current position offset

#### Example

```cpp
int x = screen.getPositionX();
int y = screen.getPositionY();
Serial.print("Position: ");
Serial.print(x);
Serial.print(", ");
Serial.println(y);
```

---

### `getPositionXOnLayer()` / `getPositionYOnLayer()`

#### Description

Returns the current position offset of a specific layer.

#### Syntax

```cpp
screen.getPositionXOnLayer(layer)
screen.getPositionYOnLayer(layer)
```

#### Parameters

- `layer`: Layer index

#### Returns

Integer: current position offset of the specified layer

#### Example

```cpp
int fgX = screen.getPositionXOnLayer(1);
int fgY = screen.getPositionYOnLayer(1);
```

---

## Example Sketches

### Basic Animation

```cpp
/*
 * Basic Animation Example
 * Load and play a simple looping animation
 */
#include "TinyFilmFestival.h"
#include "idle.h"

TinyScreen screen;
Animation idleAnim = idle;

void setup() {
    screen.begin();
    screen.play(idleAnim, LOOP);
}

void loop() {
    screen.update();
}
```

---

### Playback Control

```cpp
/*
 * Playback Control Example
 * Demonstrates all play modes and speed control
 */
#include "TinyFilmFestival.h"
#include "animation.h"

TinyScreen screen;
Animation anim = animation;

void setup() {
    Serial.begin(9600);
    screen.begin();
}

void loop() {
    // LOOP mode - continuous
    Serial.println("LOOP mode");
    screen.play(anim, LOOP);
    delay(3000);
    
    // ONCE mode - plays once and stops
    Serial.println("ONCE mode");
    screen.play(anim, ONCE);
    while (!screen.isComplete()) {
        screen.update();
    }
    delay(1000);
    
    // BOOMERANG mode - ping-pong
    Serial.println("BOOMERANG mode");
    screen.play(anim, BOOMERANG);
    delay(3000);
    
    // Speed control
    Serial.println("Fast speed");
    screen.play(anim, LOOP);
    screen.setSpeed(50);
    delay(2000);
    
    Serial.println("Slow speed");
    screen.setSpeed(500);
    delay(2000);
    
    Serial.println("Original speed");
    screen.restoreOriginalSpeed();
    delay(2000);
    
    // Partial playback (frames 2-5 only)
    Serial.println("Frames 2-5 only");
    screen.play(anim, LOOP, 2, 5);
    delay(3000);
}
```

---

### Layered Animations Example

```cpp
/*
 * Layered Animations Example
 * Combine multiple animations with independent control
 */
#include "TinyFilmFestival.h"
#include "landscape.h"      // Background scenery
#include "character.h"      // Foreground character

TinyScreen screen;
Animation bgAnim = landscape;
Animation fgAnim = character;

void setup() {
    screen.begin();
    
    // Add a foreground layer
    screen.addLayer();
    
    // Start both animations
    screen.play(bgAnim, LOOP);              // Layer 0: background
    screen.playOnLayer(1, fgAnim, LOOP);    // Layer 1: foreground
    
    // Different speeds
    screen.setSpeed(200);                   // Background: slow
    screen.setSpeedOnLayer(1, 100);         // Foreground: fast
}

void loop() {
    screen.update();  // Updates all layers and composites them
}
```

---

### Positioned Animation Example

```cpp
/*
 * Positioned Animation Example
 * Dynamically position an animation clip on the display.
 * The character bounces around using oscillateInt() and
 * pixels are automatically clipped at the matrix edges.
 */
#include "TinyFilmFestival.h"
#include "character.h"

TinyScreen screen;
Animation charAnim = character;

int X_PERIOD = 3000;   // Horizontal oscillation period
int Y_PERIOD = 2000;   // Vertical oscillation period

void setup() {
    screen.begin();
    screen.play(charAnim, LOOP);
}

void loop() {
    int x = oscillateInt(-2, 9, X_PERIOD);
    int y = oscillateInt(-1, 3, Y_PERIOD, 0.25);

    screen.setPosition(x, y);
    screen.update();
}
```

---

## See Also

- [Canvas Mode API](CanvasMode_API.md) — Real-time code-drawn graphics
- [Hybrid Mode](#hybrid-mode) — Combining animations with canvas drawing
- [LED Matrix Editor](https://ledmatrix-editor.arduino.cc/) — Create animation frames
