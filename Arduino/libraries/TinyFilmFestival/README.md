# TinyFilmFestival

| ![Character](images/character.gif) | ![Landscape](images/landscape.gif) |
|------------------------------------|------------------------------------|
| ![Load](images/load.gif)           | ![Pow](images/pow.gif)             |

A library for the Arduino UNO R4 WiFi's built-in 12×8 LED Matrix. One class, four modes:

**📚 [View Full Documentation](https://digitalfuturesocadu.github.io/TinyFilmFestival/docs/)**

| Mode | What it does | Key methods |
|------|--------------|-------------|
| **Simple LED** | Control individual LEDs | `ledWrite()`, `ledToggle()`, `ledClear()` |
| **Animation** | Play pre-made frames | `play()`, `pause()`, `setSpeed()` |
| **Canvas** | Draw with code | `beginDraw()`, `point()`, `line()` |
| **Hybrid** | Draw over animations | `beginOverlay()`, `endOverlay()` |

---

## LED Matrix Layout

The R4 WiFi has a 12×8 LED matrix (96 LEDs total). Use (x, y) coordinates or linear index:

```
     x=0  x=1  x=2  x=3  x=4  x=5  x=6  x=7  x=8  x=9  x=10 x=11
    +----+----+----+----+----+----+----+----+----+----+----+----+
y=0 |  0 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |  8 |  9 | 10 | 11 |
    +----+----+----+----+----+----+----+----+----+----+----+----+
y=1 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 20 | 21 | 22 | 23 |
    +----+----+----+----+----+----+----+----+----+----+----+----+
y=2 | 24 | 25 | 26 | 27 | 28 | 29 | 30 | 31 | 32 | 33 | 34 | 35 |
    +----+----+----+----+----+----+----+----+----+----+----+----+
y=3 | 36 | 37 | 38 | 39 | 40 | 41 | 42 | 43 | 44 | 45 | 46 | 47 |
    +----+----+----+----+----+----+----+----+----+----+----+----+
y=4 | 48 | 49 | 50 | 51 | 52 | 53 | 54 | 55 | 56 | 57 | 58 | 59 |
    +----+----+----+----+----+----+----+----+----+----+----+----+
y=5 | 60 | 61 | 62 | 63 | 64 | 65 | 66 | 67 | 68 | 69 | 70 | 71 |
    +----+----+----+----+----+----+----+----+----+----+----+----+
y=6 | 72 | 73 | 74 | 75 | 76 | 77 | 78 | 79 | 80 | 81 | 82 | 83 |
    +----+----+----+----+----+----+----+----+----+----+----+----+
y=7 | 84 | 85 | 86 | 87 | 88 | 89 | 90 | 91 | 92 | 93 | 94 | 95 |
    +----+----+----+----+----+----+----+----+----+----+----+----+

Numbers inside = linear index for ledWrite(index, state)
```

---

## Quick Start

### Simple LED Mode
Control individual LEDs like `digitalWrite()` — the simplest way to get started.

```cpp
#include "TinyFilmFestival.h"

void setup() {
    ledBegin();                   // Initialize matrix
    
    ledWrite(0, 0, HIGH);         // Top-left ON
    ledWrite(11, 7, HIGH);        // Bottom-right ON
    ledToggle(0, 0);              // Toggle top-left OFF
}

void loop() {
    ledWrite(5, 3, HIGH);         // Center ON
    delay(500);
    ledWrite(5, 3, LOW);          // Center OFF
    delay(500);
}
```

**Simple LED Functions:**
| Function | Description |
|----------|-------------|
| `ledBegin()` | Initialize the matrix (call once in setup) |
| `ledWrite(x, y, state)` | Set LED at (x,y) to HIGH or LOW |
| `ledWrite(index, state)` | Set LED by linear index (0-95) |
| `ledRead(x, y)` | Read current state of LED |
| `ledToggle(x, y)` | Toggle LED on/off |
| `ledClear()` | Turn off all LEDs |

### Animation Mode
Play pre-made frame animations created in the LED Matrix Editor.

```cpp
#include "TinyFilmFestival.h"
#include "idle.h"               // Your animation file

TinyScreen screen;
Animation myAnim = idle;        // Variable name from .h file

void setup() {
    screen.begin();
    screen.play(myAnim, LOOP);
}

void loop() {
    screen.update();
}
```

### Canvas Mode
Draw graphics in real-time using code — perfect for dynamic visualizations.

```cpp
#include "TinyFilmFestival.h"

TinyScreen screen;
int x = 0;

void setup() {
    screen.begin();
}

void loop() {
    screen.beginDraw();
    screen.clear();
    screen.point(x, 4);
    screen.endDraw();
    
    x = (x + 1) % 12;
    delay(100);
}
```

See the Canvas Mode API in the docs: https://digitalfuturesocadu.github.io/TinyFilmFestival/docs/#canvas-mode

### Hybrid Mode
Combine both — play an animation and draw additional elements on top.

```cpp
#include "TinyFilmFestival.h"
#include "idle.h"

TinyScreen screen;
Animation myAnim = idle;

void setup() {
    screen.begin();
    screen.play(myAnim, LOOP);
}

void loop() {
    screen.update();            // Update animation
    
    screen.beginOverlay();      // Draw on top
    screen.point(11, 0);        // Status indicator
    screen.endOverlay();
}
```

---

## Installation

1. Open Arduino IDE → **Sketch** → **Include Library** → **Manage Libraries**
2. Search: `TinyFilmFestival`
3. Click **Install** → Choose **INSTALL ALL** when prompted

---

## How It Works

TinyFilmFestival wraps the Arduino LED Matrix and ArduinoGraphics libraries into a single unified `TinyScreen` class. You only need one include and one object to access all features.

**Animation files** come from the [LED Matrix Editor](https://ledmatrix-editor.arduino.cc/) — a web tool where you draw frames visually and export `.h` files. These files contain frame data and timing that `TinyScreen` plays back automatically.

**Canvas drawing** uses the same graphics primitives as ArduinoGraphics (`point`, `line`, `rect`, `circle`, `text`) but through the `TinyScreen` wrapper, so you don't need separate includes.

### Documentation

| Guide | What's inside |
|-------|---------------|
| [Animation Mode API](https://digitalfuturesocadu.github.io/TinyFilmFestival/docs/#animation-mode) | `play()`, `pause()`, `setSpeed()`, layering multiple animations |
| [Canvas Mode API](https://digitalfuturesocadu.github.io/TinyFilmFestival/docs/#canvas-mode) | `point()`, `line()`, `rect()`, `circle()`, `text()`, scrolling |
| [Hybrid Mode API](https://digitalfuturesocadu.github.io/TinyFilmFestival/docs/#hybrid-mode) | `beginOverlay()` / `endOverlay()` for drawing over animations |
| [LED Matrix Editor Guide](editor-guide.md) | How to create and export `.h` animation files |
| [Example Animations](exampleAnimations/) | Pre-made `.h` files ready to use in your projects |

---

## Examples

Find these in **File → Examples → TinyFilmFestival**

### 01_Basics
| Example | Description |
|---------|-------------|
| **SimpleLED** | Simplest example — control individual LEDs like digitalWrite |
| **FirstAnimation** | Animation Mode — load and play a pre-made animation |
| **FirstCanvas** | Canvas Mode — bouncing dot drawn with code |

### 02_Animation_Mode
| Example | Description |
|---------|-------------|
| **PlaybackControl** | All playback modes (LOOP/ONCE/BOOMERANG), speed control, partial clips |
| **LayeredAnimations** | Multiple animations combined with addLayer() and playOnLayer() |
| **PositionedAnimation** | Move animations with x/y offsets and clipping |
| **Sensor_Input/** | Distance and pressure sensor-driven animation control |

### 03_Canvas_Mode
| Example | Description |
|---------|-------------|
| **MovingShapes** | Motion patterns mirroring animation modes (continuous, single-pass, bounce) |
| **LayeredGraphics** | Multiple independent animated elements drawn in layers |
| **ScrollingText** | Static and scrolling text display |
| **Sensor_Input/** | Distance and pressure sensor-driven canvas visuals |

### 04_Hybrid_Mode
| Example | Description |
|---------|-------------|
| **AnimationWithOverlay** | Pre-made animation with code-drawn progress indicator |
| **Sensor_Input/** | Sensor-driven overlays on animations |

### 05_Projects
| Example | Description |
|---------|-------------|
| **InteractiveCharacter** | Complete project: 4-zone proximity character with sparkle effects |

### 06_Canvas_Animation
| Example | Description |
|---------|-------------|
| **EaseDemo** | Smooth value interpolation with `Ease` |
| **OscillatorDemo** | Continuous sine wave motion using `oscillate()` |
| **SmoothAnimation** | Combining Ease and oscillate for complex motion |

### 07_Utilities
| Example | Description |
|---------|-------------|
| **AnimationTest** | Animation Mode comprehensive test |
| **DrawingTest** | Canvas Mode comprehensive test |
| **HybridTest** | Hybrid Mode comprehensive test |
| **PlaybackTest** | Focused playback control test |
| **SmoothAnimation** | Alternate smooth motion demo |

---

## Hardware

**Required:** Arduino UNO R4 WiFi (has built-in 12×8 LED Matrix)

**Optional (for sensor examples):**
- Pushbutton (pin 2 → GND)
- HC-SR04 ultrasonic (Trigger: A0, Echo: A1)
- Pressure sensor (A0)

---

## License

MIT License — see [LICENSE](LICENSE)
