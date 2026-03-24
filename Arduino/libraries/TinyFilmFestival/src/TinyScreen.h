// TinyScreen.h
// Unified wrapper for Arduino R4 WiFi LED Matrix
// Combines Animation Mode + Canvas Mode in one simple interface
#pragma once

//------------------------------------------------------------------------------
// LED Matrix Layout (12 columns x 8 rows = 96 LEDs)
//------------------------------------------------------------------------------
//
//  Coordinate System: led(x, y, state) or ledWrite(x, y, state)
//
//       x=0  x=1  x=2  x=3  x=4  x=5  x=6  x=7  x=8  x=9  x=10 x=11
//      +----+----+----+----+----+----+----+----+----+----+----+----+
//  y=0 |  0 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |  8 |  9 | 10 | 11 |
//      +----+----+----+----+----+----+----+----+----+----+----+----+
//  y=1 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 20 | 21 | 22 | 23 |
//      +----+----+----+----+----+----+----+----+----+----+----+----+
//  y=2 | 24 | 25 | 26 | 27 | 28 | 29 | 30 | 31 | 32 | 33 | 34 | 35 |
//      +----+----+----+----+----+----+----+----+----+----+----+----+
//  y=3 | 36 | 37 | 38 | 39 | 40 | 41 | 42 | 43 | 44 | 45 | 46 | 47 |
//      +----+----+----+----+----+----+----+----+----+----+----+----+
//  y=4 | 48 | 49 | 50 | 51 | 52 | 53 | 54 | 55 | 56 | 57 | 58 | 59 |
//      +----+----+----+----+----+----+----+----+----+----+----+----+
//  y=5 | 60 | 61 | 62 | 63 | 64 | 65 | 66 | 67 | 68 | 69 | 70 | 71 |
//      +----+----+----+----+----+----+----+----+----+----+----+----+
//  y=6 | 72 | 73 | 74 | 75 | 76 | 77 | 78 | 79 | 80 | 81 | 82 | 83 |
//      +----+----+----+----+----+----+----+----+----+----+----+----+
//  y=7 | 84 | 85 | 86 | 87 | 88 | 89 | 90 | 91 | 92 | 93 | 94 | 95 |
//      +----+----+----+----+----+----+----+----+----+----+----+----+
//
//  Numbers inside = linear index for led(index, state) or ledWrite(index, state)
//
//  Examples:
//    ledWrite(0, 0, HIGH);   // Top-left corner
//    ledWrite(11, 0, HIGH);  // Top-right corner  
//    ledWrite(0, 7, HIGH);   // Bottom-left corner
//    ledWrite(11, 7, HIGH);  // Bottom-right corner
//    ledWrite(41, HIGH);     // Center LED (same as x=5, y=3)
//
//------------------------------------------------------------------------------

// ArduinoGraphics MUST be included before Arduino_LED_Matrix
// for drawing methods to work properly
#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"

#define MAX_LAYERS 5  // Maximum animation layers

// Forward declarations
class Animation;
class AnimationLayer;

//------------------------------------------------------------------------------
// PlayMode - How animations play back
//------------------------------------------------------------------------------
enum PlayMode {
    PLAY_ONCE,
    PLAY_LOOP,
    PLAY_BOOMERANG
};

// Shorthand aliases
static const PlayMode ONCE = PLAY_ONCE;
static const PlayMode LOOP = PLAY_LOOP;
static const PlayMode BOOMERANG = PLAY_BOOMERANG;

//------------------------------------------------------------------------------
// LED State Constants - Simple ON/OFF for monochrome matrix
//------------------------------------------------------------------------------
// ON = lit LED (white), OFF = unlit LED (black)
// These work with stroke(), fill(), background() and ledWrite()
static const uint32_t ON = 0xFFFFFF;
static const uint32_t OFF = 0x000000;

//------------------------------------------------------------------------------
// AnimationState - Current state of an animation
//------------------------------------------------------------------------------
enum AnimationState {
    IDLE,
    PLAYING,
    PAUSED,
    COMPLETED
};

//------------------------------------------------------------------------------
// Animation - Wrapper for LED Matrix Editor frame data
//------------------------------------------------------------------------------
class Animation {
private:
    const uint32_t (*frames)[4];
    int frameCount;

public:
    Animation() : frames(nullptr), frameCount(0) {}
    
    template<size_t N>
    Animation(const uint32_t (&frameArray)[N][4]) : frames(frameArray), frameCount(N) {}
    
    template<size_t N>
    Animation& operator=(const uint32_t (&frameArray)[N][4]) {
        frames = frameArray;
        frameCount = N;
        return *this;
    }

    const uint32_t (*getFrames() const)[4] { return frames; }
    int getFrameCount() const { return frameCount; }
};

//------------------------------------------------------------------------------
// AnimationLayer - Internal class for managing one animation track
//------------------------------------------------------------------------------
class AnimationLayer {
private:
    bool isCustomSpeed;
    int customSpeedInterval;
    uint32_t* originalTimings;
    int frameCount;
    int currentFrameIndex;
    int startFrameIndex;
    int endFrameIndex;
    bool isBoomerang;
    bool isReversing;
    bool isPlayingBackward;
    bool defaultPlayingBackward;
    bool speedDirectionOverride;
    bool useSpeedMultiplier;
    float speedMultiplier;
    const uint32_t (*currentAnimation)[4];
    unsigned long lastUpdateTime;
    PlayMode currentMode;
    AnimationState currentState;

    int8_t offsetX;
    int8_t offsetY;
    
    void cleanup();
    void copyTimings(const uint32_t frames[][4], int numFrames);
    bool isValidAnimation() const { return currentAnimation != nullptr && frameCount > 0; }

public:
    AnimationLayer();
    ~AnimationLayer();

    void start(const Animation& animation, PlayMode mode, int startFrame, int endFrame);
    void setSpeed(int speedMs);
    void setSpeed(float speedMultiplier);
    void setSpeed(double speedMultiplier);
    void pause();
    void resume();
    void restoreOriginalSpeed();
    void stop();
    bool updateFrame();  // Returns true if frame updated
    bool getFrame(uint32_t frame[3]) const;  // Get current frame data

    // Status
    bool isPaused() const { return currentState == PAUSED; }
    bool isComplete() const { return currentState == COMPLETED; }
    bool isPlaying() const { return currentState == PLAYING; }
    bool isIdle() const { return currentState == IDLE; }
    bool isCustomSpeedActive() const { return isCustomSpeed; }
    bool isPlayingBackwards() const { return isPlayingBackward; }
    
    AnimationState getState() const { return currentState; }
    int getTotalFrames() const { return frameCount; }
    int getCurrentFrameNum() const { return currentFrameIndex + 1; }
    int getCurrentSpeed() const;
    int getStartFrame() const { return startFrameIndex + 1; }
    int getEndFrame() const { return endFrameIndex + 1; }
    
    // Position offset
    void setOffset(int x, int y);
    int getOffsetX() const { return offsetX; }
    int getOffsetY() const { return offsetY; }
};

//------------------------------------------------------------------------------
// TinyScreen - The main unified class
//------------------------------------------------------------------------------
class TinyScreen {
private:
    ArduinoLEDMatrix matrix;
    AnimationLayer layers[MAX_LAYERS];
    uint8_t layerCount;
    uint32_t combinedFrame[3];
    bool inOverlay;
    bool autoShow;
    
    // Canvas mode buffering (flicker-free drawing)
    uint8_t canvasBuffer[8][12];      // [row][col] pixel buffer
    bool inCanvasDraw;                 // Currently in beginDraw/endDraw block
    bool strokeEnabled;
    bool fillEnabled;
    uint8_t strokeValue;               // 1 = ON, 0 = OFF
    uint8_t fillValue;                 // 1 = ON, 0 = OFF
    
    // Text rendering state
    uint8_t textSize;                  // 1 = normal (3x5), 2 = double (6x10)
    int scrollOffset;                  // Current scroll position
    unsigned long scrollSpeed;         // Ms per pixel
    unsigned long lastScrollTime;      // Last scroll update time
    
    // Display rotation (0, 90, 180, 270 degrees)
    uint8_t rotation;                  // 0=0°, 1=90°, 2=180°, 3=270°
    
    // Display invert
    bool invertDisplay;                // If true, flip all pixels
    
    // Internal drawing helpers
    void bufferPoint(int x, int y, uint8_t value);
    void bufferLine(int x1, int y1, int x2, int y2);
    void bufferRect(int x, int y, int w, int h);
    void bufferCircle(int cx, int cy, int r);
    void bufferToFrame(uint32_t frame[3]);
    void bufferCharScaled(char c, int x, int y, uint8_t value, int scale);
    void frameToBuffer(const uint32_t frame[3]);  // Load frame into canvas buffer
    
    // For backward compatibility - primary layer access
    AnimationLayer& primary() { return layers[0]; }
    const AnimationLayer& primary() const { return layers[0]; }

    static PlayMode coercePlayMode(int mode);

public:
    TinyScreen();
    
    //--- Initialization ---
    bool begin();
    
    //--- Animation Mode (simple single animation) ---
    void play(const Animation& animation, PlayMode mode = LOOP);
    void play(const Animation& animation, PlayMode mode, int startFrame, int endFrame);
    void play(const Animation& animation, int mode);
    void play(const Animation& animation, int mode, int startFrame, int endFrame);
    
    // Full API alias (backward compatible)
    void startAnimation(const Animation& animation, PlayMode mode = PLAY_ONCE, int startFrame = 0, int endFrame = 0);
    void startAnimation(const Animation& animation, int mode, int startFrame = 0, int endFrame = 0);
    
    template<size_t N>
    void startAnimation(const uint32_t (&frames)[N][4], PlayMode mode = PLAY_ONCE, int startFrame = 0, int endFrame = 0) {
        Animation anim(frames);
        startAnimation(anim, mode, startFrame, endFrame);
    }

    template<size_t N>
    void startAnimation(const uint32_t (&frames)[N][4], int mode, int startFrame = 0, int endFrame = 0) {
        Animation anim(frames);
        startAnimation(anim, mode, startFrame, endFrame);
    }
    
    //--- Layered Animations ---
    int addLayer();  // Returns layer index, or -1 if full
    void playOnLayer(int layer, const Animation& animation, PlayMode mode = LOOP);
    void playOnLayer(int layer, const Animation& animation, PlayMode mode, int startFrame, int endFrame);
    void playOnLayer(int layer, const Animation& animation, int mode);
    void playOnLayer(int layer, const Animation& animation, int mode, int startFrame, int endFrame);
    void setSpeedOnLayer(int layer, int speedMs);
    void setSpeedOnLayer(int layer, float speedMultiplier);
    void setSpeedOnLayer(int layer, double speedMultiplier);
    void pauseLayer(int layer);
    void resumeLayer(int layer);
    void stopLayer(int layer);
    
    //--- Playback Control (affects primary layer) ---
    void setSpeed(int speedMs);
    void setSpeed(float speedMultiplier);
    void setSpeed(double speedMultiplier);
    void pause();
    void resume();
    void restoreOriginalSpeed();
    void stop();
    
    //--- Animation Position (offset animations on the display) ---
    void setPosition(int x, int y);
    void setPositionOnLayer(int layer, int x, int y);
    int getPositionX() const;
    int getPositionY() const;
    int getPositionXOnLayer(int layer) const;
    int getPositionYOnLayer(int layer) const;
    
    //--- Frame Update (call in loop!) ---
    void update();
    
    //--- Canvas Mode (direct drawing) ---
    void beginDraw();
    void endDraw();
    void clear();
    void setRotation(int degrees);        // 0, 90, 180, or 270
    int getRotation();                    // Returns current rotation in degrees
    void setInvert(bool invert);          // Invert all pixels (flip ON/OFF)
    bool getInvert();                     // Returns current invert state
    
    // Drawing primitives
    void set(int x, int y, bool on);      // Single pixel
    void point(int x, int y);              // Pixel on (alias)
    void line(int x1, int y1, int x2, int y2);
    void rect(int x, int y, int width, int height);
    void circle(int cx, int cy, int diameter);
    void ellipse(int cx, int cy, int width, int height);
    
    // Style control - use ON/OFF for simplicity (they're just hex constants)
    // stroke(ON) = turn on, stroke(OFF) = turn off
    void stroke(uint32_t color);              // Use ON, OFF, or hex color
    void stroke(uint8_t r, uint8_t g, uint8_t b);
    void fill(uint32_t color);                // Use ON, OFF, or hex color
    void fill(uint8_t r, uint8_t g, uint8_t b);
    void background(uint32_t color);          // Use ON, OFF, or hex color
    void background(uint8_t r, uint8_t g, uint8_t b);
    void noStroke();
    void noFill();
    
    // Text methods - renders to canvas buffer (works with beginDraw/endDraw)
    // Built-in 3x5 font (4 pixels per char with spacing)
    void text(const char* str, int x, int y);
    void text(const String& str, int x, int y);
    void textChar(char c, int x, int y);      // Draw single character
    void setTextSize(int size);               // 1 = 3x5 (default), 2 = 6x10 (scaled)
    
    // Scrolling text - works with buffered drawing
    // Call in loop, auto-updates scroll position
    void scrollText(const char* str, int y, int direction = SCROLL_LEFT);
    void scrollText(const String& str, int y, int direction = SCROLL_LEFT);
    void setScrollSpeed(unsigned long ms);    // Milliseconds per pixel scroll
    void resetScroll();                       // Reset scroll position to start
    int getScrollOffset();                    // Get current scroll offset
    
    // Legacy text methods (use ArduinoGraphics - won't work with buffered drawing)
    void textFont(const Font& font);
    int textFontWidth();
    int textFontHeight();
    
    // Legacy scrolling text (ArduinoGraphics - won't work with buffered drawing)
    void beginText(int x = 0, int y = 0);
    void beginText(int x, int y, uint8_t r, uint8_t g, uint8_t b);
    void beginText(int x, int y, uint32_t color);
    void endText(int scrollDirection = NO_SCROLL);
    void textScrollSpeed(unsigned long speed);
    
    // Print interface (works after beginText)
    size_t print(const char* str);
    size_t print(const String& str);
    size_t print(char c);
    size_t print(int n);
    size_t println();
    size_t println(const char* str);
    size_t println(const String& str);
    
    // Matrix info
    int width() { return matrix.width(); }
    int height() { return matrix.height(); }
    
    //--- Hybrid Mode (draw over animation) ---
    void beginOverlay();
    void endOverlay();
    
    //--- Status (primary layer) ---
    bool isPaused() const { return primary().isPaused(); }
    bool isComplete() const { return primary().isComplete(); }
    bool isPlaying() const { return primary().isPlaying(); }
    bool isIdle() const { return primary().isIdle(); }
    bool isCustomSpeedActive() const { return primary().isCustomSpeedActive(); }
    bool isPlayingBackwards() const { return primary().isPlayingBackwards(); }
    
    AnimationState getState() const { return primary().getState(); }
    int getTotalFrames() const { return primary().getTotalFrames(); }
    int getCurrentFrame() const { return primary().getCurrentFrameNum(); }
    int getCurrentSpeed() const { return primary().getCurrentSpeed(); }
    int getStartFrame() const { return primary().getStartFrame(); }
    int getEndFrame() const { return primary().getEndFrame(); }
    
    //--- Direct matrix access (advanced) ---
    ArduinoLEDMatrix& getMatrix() { return matrix; }
    void displayFrame(const uint32_t frame[3]);
    
    //--- Simple LED Control (digitalWrite-style) ---
    void led(int x, int y, bool state);           // Set LED at (x,y) on/off
    void led(int ledNum, bool state);             // Set LED by linear index (0-95)
    bool getLed(int x, int y);                    // Read LED state at (x,y)
    bool getLed(int ledNum);                      // Read LED state by linear index
    void toggle(int x, int y);                    // Toggle LED at (x,y)
    void toggle(int ledNum);                      // Toggle LED by linear index
    void show();                                  // Push buffered LED changes to display
    void setAutoShow(bool enabled);               // Auto-display after LED writes
    bool getAutoShow() const { return autoShow; }
    void clearLeds();                             // Turn off all LEDs
    
    // Blink control (non-blocking, per-LED independent rates)
    void blink(int x, int y, unsigned long rateMs);   // Blink LED at (x,y) every rateMs
    void blink(int ledNum, unsigned long rateMs);     // Blink LED by index every rateMs
    void noBlink(int x, int y);                       // Stop blinking LED at (x,y), turn it OFF
    void noBlink(int ledNum);                         // Stop blinking LED by index
    void noBlink();                                   // Stop all blinking
    void updateBlinks();                              // Process blink timers (call in loop!)
    
private:
    uint8_t ledBuffer[12][8];                     // Internal buffer for LED states
    bool ledBufferDirty;                          // Track if buffer needs display update
    
    // Blink state (per-LED)
    unsigned long blinkRate[12][8];               // 0 = not blinking, >0 = interval in ms
    unsigned long lastBlinkTime[12][8];           // Last toggle time per LED
    
    // Helper to convert linear index to x,y
    void indexToXY(int index, int& x, int& y);
    
    // Canvas buffer to frame conversion helper
    void canvasBufferToFrame(uint32_t frame[3]);
};

//------------------------------------------------------------------------------
// Backward Compatibility Aliases
//------------------------------------------------------------------------------

// TinyFilmFestival now maps to TinyScreen
using TinyFilmFestival = TinyScreen;

//------------------------------------------------------------------------------
// Standalone LED Functions (digitalWrite-style convenience)
//------------------------------------------------------------------------------

// These functions use a default internal TinyScreen instance
// Call ledBegin() once in setup(), then use ledWrite() like digitalWrite()
void ledBegin();                                  // Initialize the LED matrix
void ledWrite(int x, int y, bool state);          // Set LED at (x,y) - like digitalWrite
void ledWrite(int ledNum, bool state);            // Set LED by index (0-95)
bool ledRead(int x, int y);                       // Read LED state at (x,y)
bool ledRead(int ledNum);                         // Read LED state by index
void ledToggle(int x, int y);                     // Toggle LED at (x,y)
void ledToggle(int ledNum);                       // Toggle LED by index
void ledClear();                                  // Turn off all LEDs
void ledShow();                                   // Push changes to display (auto-called by ledWrite)
void ledBlink(int x, int y, unsigned long rateMs); // Blink LED at (x,y) every rateMs
void ledBlink(int ledNum, unsigned long rateMs);   // Blink LED by index every rateMs
void ledNoBlink(int x, int y);                     // Stop blinking LED at (x,y)
void ledNoBlink(int ledNum);                       // Stop blinking LED by index
void ledNoBlink();                                 // Stop all blinking
void ledUpdate();                                  // Process blink timers (call in loop!)

// Get the internal TinyScreen instance (for mixing with other features)
TinyScreen& getLedMatrix();

//------------------------------------------------------------------------------
// Animation Utilities - Smooth value control
//------------------------------------------------------------------------------

// Oscillate: Returns a value that smoothly cycles between min and max
// Uses a sine wave, period is the time for one complete cycle in milliseconds
// Optional offset shifts the phase (0.0 = no shift, 0.5 = half cycle offset)
// Example: oscillate(0, 100, 2000) cycles 0→100→0 over 2 seconds
// Example: oscillate(0, 100, 2000, 0.5) same but shifted by half a cycle
float oscillate(float min, float max, unsigned long periodMs, float offset = 0.0f);

// Integer version for convenience
int oscillateInt(int min, int max, unsigned long periodMs, float offset = 0.0f);

//------------------------------------------------------------------------------
// Ease Class - Smooth linear interpolation to target values
//------------------------------------------------------------------------------

class Ease {
private:
    float _current;
    float _start;
    float _target;
    unsigned long _startTime;
    unsigned long _duration;
    bool _active;
    
public:
    // Create with initial value (default 0)
    Ease(float initial = 0);
    
    // Start moving to a target value over duration (milliseconds)
    void to(float target, unsigned long durationMs);
    
    // Get current interpolated value (auto-updates based on time)
    float value();
    
    // Get current value as integer (rounded)
    int intValue();
    
    // Check if animation is complete
    bool done();
    
    // Check if currently animating
    bool moving();
    
    // Instantly jump to a value (no animation)
    void jump(float v);
    
    // Get the target value
    float target();
};

//------------------------------------------------------------------------------
// Backward Compatibility Aliases
//------------------------------------------------------------------------------

// CombinedFilmFestival is deprecated but still works
class CombinedFilmFestival {
private:
    TinyScreen screen;
    int layerIndices[MAX_LAYERS];
    int count;
    
public:
    CombinedFilmFestival() : count(0) {}
    
    bool begin() { return screen.begin(); }
    
    bool addFilm(TinyScreen& film);  // Note: now takes TinyScreen
    
    void update() { screen.update(); }
};
