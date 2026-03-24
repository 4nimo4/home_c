# V1 Methods (Deprecated)

This document lists the V1.x methods that are still supported for backward compatibility but are deprecated in V2.0. New projects should use the V2 `TinyScreen` API instead.

---

## Class Aliases

### `TinyFilmFestival`

**Status:** Deprecated alias for `TinyScreen`

```cpp
// V1 (still works)
TinyFilmFestival film;

// V2 (recommended)
TinyScreen screen;
```

---

### `CombinedFilmFestival`

**Status:** Deprecated — use `TinyScreen` with `addLayer()` instead

```cpp
// V1 (still works)
TinyFilmFestival bg, fg;
CombinedFilmFestival combined;

combined.begin();
bg.startAnimation(bgAnim, LOOP);
fg.startAnimation(fgAnim, LOOP);
combined.addFilm(bg);
combined.addFilm(fg);

void loop() {
    combined.update();
}

// V2 (recommended)
TinyScreen screen;

screen.begin();
screen.addLayer();
screen.play(bgAnim, LOOP);
screen.playOnLayer(1, fgAnim, LOOP);

void loop() {
    screen.update();
}
```

---

## Deprecated Methods

### `startAnimation()`

**Status:** Deprecated — use `play()` instead

```cpp
// V1 (still works)
film.startAnimation(myAnim, LOOP);
film.startAnimation(myAnim, LOOP, 2, 6);

// V2 (recommended)
screen.play(myAnim, LOOP);
screen.play(myAnim, LOOP, 2, 6);
```

---

### Direct Frame Array Syntax

**Status:** Deprecated — use `Animation` wrapper instead

```cpp
// V1 (still works)
film.startAnimation(animation, LOOP);  // 'animation' is raw uint32_t array

// V2 (recommended)
Animation myAnim = animation;          // Wrap in Animation object
screen.play(myAnim, LOOP);
```

---

## PlayMode Constants

Both naming styles work:

| V1 Constant | V2 Constant | Value |
|-------------|-------------|-------|
| `PLAY_ONCE` | `ONCE` | Play once and stop |
| `PLAY_LOOP` | `LOOP` | Loop continuously |
| `PLAY_BOOMERANG` | `BOOMERANG` | Ping-pong |

```cpp
// Both work
screen.play(myAnim, PLAY_LOOP);
screen.play(myAnim, LOOP);
```

---

## Migration Summary

| V1 | V2 |
|----|----|
| `TinyFilmFestival film;` | `TinyScreen screen;` |
| `film.startAnimation(anim, LOOP)` | `screen.play(anim, LOOP)` |
| `CombinedFilmFestival` + `addFilm()` | `screen.addLayer()` + `playOnLayer()` |
| `ArduinoLEDMatrix matrix;` + `matrix.beginDraw()` | `screen.beginDraw()` |

---

## Notes

- All V1 code should compile without changes
- V1 methods internally call the V2 implementations
- Performance is identical between V1 and V2 syntax
- New features (overlays, text methods) require V2 API
