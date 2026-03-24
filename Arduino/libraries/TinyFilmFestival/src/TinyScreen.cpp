// TinyScreen.cpp
// Implementation of the unified TinyScreen class
#include "TinyScreen.h"

//==============================================================================
// AnimationLayer Implementation
//==============================================================================

AnimationLayer::AnimationLayer() :
    isCustomSpeed(false),
    customSpeedInterval(0),
    originalTimings(nullptr),
    frameCount(0),
    currentFrameIndex(0),
    startFrameIndex(0),
    endFrameIndex(0),
    isBoomerang(false),
    isReversing(false),
    isPlayingBackward(false),
    defaultPlayingBackward(false),
    speedDirectionOverride(false),
    useSpeedMultiplier(false),
    speedMultiplier(1.0f),
    currentAnimation(nullptr),
    lastUpdateTime(0),
    currentMode(PLAY_ONCE),
    currentState(IDLE),
    offsetX(0),
    offsetY(0)
{
}

AnimationLayer::~AnimationLayer() {
    cleanup();
}

void AnimationLayer::cleanup() {
    if (originalTimings != nullptr) {
        delete[] originalTimings;
        originalTimings = nullptr;
    }
    currentAnimation = nullptr;
    frameCount = 0;
    currentState = IDLE;
}

void AnimationLayer::copyTimings(const uint32_t frames[][4], int numFrames) {
    if (originalTimings != nullptr) {
        delete[] originalTimings;
    }
    
    originalTimings = new uint32_t[numFrames];
    for (int i = 0; i < numFrames; i++) {
        originalTimings[i] = frames[i][3];
    }
}

void AnimationLayer::start(const Animation& animation, PlayMode mode, int startFrame, int endFrame) {
    cleanup();
    
    currentMode = mode;
    isBoomerang = (mode == PLAY_BOOMERANG);
    frameCount = animation.getFrameCount();
    currentAnimation = animation.getFrames();
    
    if (frameCount == 0 || currentAnimation == nullptr) {
        currentState = IDLE;
        return;
    }
    
    copyTimings(currentAnimation, frameCount);
    
    // Convert 1-based input to 0-based internal indices
    if (startFrame == 0 && endFrame == 0) {
        startFrameIndex = 0;
        endFrameIndex = frameCount - 1;
    } else {
        startFrameIndex = max(1, min(startFrame, frameCount)) - 1;
        endFrameIndex = endFrame == 0 ? frameCount - 1 : max(1, min(endFrame, frameCount)) - 1;
    }
    
    isPlayingBackward = startFrameIndex > endFrameIndex;
    if (isPlayingBackward) {
        int temp = endFrameIndex;
        endFrameIndex = startFrameIndex;
        startFrameIndex = temp;
    }

    defaultPlayingBackward = isPlayingBackward;
    speedDirectionOverride = false;
    useSpeedMultiplier = false;
    speedMultiplier = 1.0f;
    
    currentFrameIndex = isPlayingBackward ? endFrameIndex : startFrameIndex;
    isReversing = false;
    lastUpdateTime = millis();
    currentState = PLAYING;
}

void AnimationLayer::setSpeed(int speedMs) {
    if (!isValidAnimation()) return;
    
    if (speedMs == 0) {
        currentState = PAUSED;
    } else {
        isCustomSpeed = true;
        useSpeedMultiplier = false;
        customSpeedInterval = abs(speedMs);
        if (speedMs < 0) {
            isPlayingBackward = true;
            speedDirectionOverride = true;
        } else {
            speedDirectionOverride = false;
            isPlayingBackward = defaultPlayingBackward;
        }
        
        if (currentState != PLAYING) {
            lastUpdateTime = millis();
            currentState = PLAYING;
        }
    }
}

void AnimationLayer::setSpeed(float multiplier) {
    if (!isValidAnimation()) return;

    if (multiplier == 0.0f) {
        currentState = PAUSED;
        return;
    }

    isCustomSpeed = true;
    useSpeedMultiplier = true;
    speedMultiplier = abs(multiplier);

    if (multiplier < 0.0f) {
        isPlayingBackward = true;
        speedDirectionOverride = true;
    } else {
        speedDirectionOverride = false;
        isPlayingBackward = defaultPlayingBackward;
    }

    if (currentState != PLAYING) {
        lastUpdateTime = millis();
        currentState = PLAYING;
    }
}

void AnimationLayer::pause() {
    if (currentState == PLAYING) {
        currentState = PAUSED;
    }
}

void AnimationLayer::resume() {
    if (currentState != PLAYING && isValidAnimation()) {
        lastUpdateTime = millis();
        currentState = PLAYING;
    }
}

void AnimationLayer::restoreOriginalSpeed() {
    if (!isValidAnimation()) return;
    
    if (isCustomSpeed) {
        isCustomSpeed = false;
        useSpeedMultiplier = false;
        speedMultiplier = 1.0f;
        if (speedDirectionOverride) {
            speedDirectionOverride = false;
            isPlayingBackward = defaultPlayingBackward;
        }
        if (currentState != PLAYING) {
            lastUpdateTime = millis();
            currentState = PLAYING;
        }
    }
}

void AnimationLayer::stop() {
    cleanup();
}

int AnimationLayer::getCurrentSpeed() const {
    if (!isValidAnimation()) return 0;
    if (!isCustomSpeed) {
        return originalTimings[currentFrameIndex];
    }
    if (useSpeedMultiplier) {
        float interval = (float)originalTimings[currentFrameIndex] / speedMultiplier;
        return (int)max(1.0f, round(interval));
    }
    return customSpeedInterval;
}

bool AnimationLayer::getFrame(uint32_t frame[3]) const {
    if (!isValidAnimation() || currentState == IDLE) return false;
    
    if (offsetX == 0 && offsetY == 0) {
        // No offset — return raw frame data
        frame[0] = currentAnimation[currentFrameIndex][0];
        frame[1] = currentAnimation[currentFrameIndex][1];
        frame[2] = currentAnimation[currentFrameIndex][2];
    } else {
        // Apply position offset with clipping
        const uint32_t* src = currentAnimation[currentFrameIndex];
        frame[0] = 0;
        frame[1] = 0;
        frame[2] = 0;
        
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 12; col++) {
                // Map display pixel back to source animation pixel
                int srcCol = col - offsetX;
                int srcRow = row - offsetY;
                
                // Skip if source is out of bounds (clipping)
                if (srcCol < 0 || srcCol >= 12 || srcRow < 0 || srcRow >= 8) continue;
                
                // Read source pixel
                int srcBit = srcRow * 12 + srcCol;
                bool pixelOn = false;
                if (srcBit < 32) {
                    pixelOn = (src[0] & (1UL << (31 - srcBit))) != 0;
                } else if (srcBit < 64) {
                    pixelOn = (src[1] & (1UL << (63 - srcBit))) != 0;
                } else {
                    pixelOn = (src[2] & (1UL << (95 - srcBit))) != 0;
                }
                
                // Write to destination if source pixel is on
                if (pixelOn) {
                    int dstBit = row * 12 + col;
                    if (dstBit < 32) {
                        frame[0] |= (1UL << (31 - dstBit));
                    } else if (dstBit < 64) {
                        frame[1] |= (1UL << (63 - dstBit));
                    } else {
                        frame[2] |= (1UL << (95 - dstBit));
                    }
                }
            }
        }
    }
    return true;
}

void AnimationLayer::setOffset(int x, int y) {
    offsetX = (int8_t)x;
    offsetY = (int8_t)y;
}

bool AnimationLayer::updateFrame() {
    if (currentState != PLAYING || !isValidAnimation()) return false;

    unsigned long currentTime = millis();
    uint32_t interval = 0;
    if (isCustomSpeed && useSpeedMultiplier) {
        float scaled = (float)originalTimings[currentFrameIndex] / speedMultiplier;
        interval = (uint32_t)max(1.0f, round(scaled));
    } else {
        interval = isCustomSpeed ? customSpeedInterval : originalTimings[currentFrameIndex];
    }

    if (currentTime - lastUpdateTime < interval) return false;

    // Advance frame
    if (!isBoomerang) {
        if (isPlayingBackward) {
            currentFrameIndex--;
            if (currentFrameIndex < startFrameIndex) {
                if (currentMode == PLAY_ONCE) {
                    currentState = COMPLETED;
                    return true;
                } else {
                    currentFrameIndex = endFrameIndex;
                }
            }
        } else {
            currentFrameIndex++;
            if (currentFrameIndex > endFrameIndex) {
                if (currentMode == PLAY_ONCE) {
                    currentState = COMPLETED;
                    return true;
                } else {
                    currentFrameIndex = startFrameIndex;
                }
            }
        }
    } else {
        // Boomerang mode
        if (!isReversing) {
            currentFrameIndex = isPlayingBackward ? currentFrameIndex - 1 : currentFrameIndex + 1;
            if (currentFrameIndex >= endFrameIndex || currentFrameIndex <= startFrameIndex) {
                isReversing = true;
                currentFrameIndex = isPlayingBackward ? startFrameIndex : endFrameIndex;
            }
        } else {
            currentFrameIndex = isPlayingBackward ? currentFrameIndex + 1 : currentFrameIndex - 1;
            if (currentFrameIndex <= startFrameIndex || currentFrameIndex >= endFrameIndex) {
                isReversing = false;
                if (currentMode == PLAY_ONCE) {
                    currentState = COMPLETED;
                    return true;
                } else {
                    currentFrameIndex = isPlayingBackward ? endFrameIndex : startFrameIndex;
                }
            }
        }
    }

    lastUpdateTime = currentTime;
    return true;
}

//==============================================================================
// TinyScreen Implementation
//==============================================================================

TinyScreen::TinyScreen() : layerCount(1), inOverlay(false), autoShow(true), ledBufferDirty(false) {
    combinedFrame[0] = 0;
    combinedFrame[1] = 0;
    combinedFrame[2] = 0;
    
    // Initialize LED buffer and blink state to all off
    for (int x = 0; x < 12; x++) {
        for (int y = 0; y < 8; y++) {
            ledBuffer[x][y] = 0;
            blinkRate[x][y] = 0;
            lastBlinkTime[x][y] = 0;
        }
    }
    
    // Initialize canvas buffer
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 12; col++) {
            canvasBuffer[row][col] = 0;
        }
    }
    inCanvasDraw = false;
    strokeEnabled = true;
    fillEnabled = false;
    strokeValue = 1;
    fillValue = 1;
    
    // Initialize text state
    textSize = 1;
    scrollOffset = 0;
    scrollSpeed = 100;  // 100ms per pixel default
    lastScrollTime = 0;
    
    // Initialize rotation
    rotation = 0;
    
    // Initialize invert
    invertDisplay = false;
}

bool TinyScreen::begin() {
    return matrix.begin();
}

PlayMode TinyScreen::coercePlayMode(int mode) {
    switch (mode) {
        case PLAY_ONCE:
            return PLAY_ONCE;
        case PLAY_LOOP:
            return PLAY_LOOP;
        case PLAY_BOOMERANG:
            return PLAY_BOOMERANG;
        default:
            return LOOP;
    }
}

//--- Animation Mode (simple) ---

void TinyScreen::play(const Animation& animation, PlayMode mode) {
    primary().start(animation, mode, 0, 0);
}

void TinyScreen::play(const Animation& animation, PlayMode mode, int startFrame, int endFrame) {
    primary().start(animation, mode, startFrame, endFrame);
}

void TinyScreen::play(const Animation& animation, int mode) {
    play(animation, coercePlayMode(mode));
}

void TinyScreen::play(const Animation& animation, int mode, int startFrame, int endFrame) {
    play(animation, coercePlayMode(mode), startFrame, endFrame);
}

void TinyScreen::startAnimation(const Animation& animation, PlayMode mode, int startFrame, int endFrame) {
    primary().start(animation, mode, startFrame, endFrame);
}

void TinyScreen::startAnimation(const Animation& animation, int mode, int startFrame, int endFrame) {
    startAnimation(animation, coercePlayMode(mode), startFrame, endFrame);
}

//--- Layered Animations ---

int TinyScreen::addLayer() {
    if (layerCount >= MAX_LAYERS) return -1;
    return layerCount++;
}

void TinyScreen::playOnLayer(int layer, const Animation& animation, PlayMode mode) {
    if (layer >= 0 && layer < layerCount) {
        layers[layer].start(animation, mode, 0, 0);
    }
}

void TinyScreen::playOnLayer(int layer, const Animation& animation, PlayMode mode, int startFrame, int endFrame) {
    if (layer >= 0 && layer < layerCount) {
        layers[layer].start(animation, mode, startFrame, endFrame);
    }
}

void TinyScreen::playOnLayer(int layer, const Animation& animation, int mode) {
    playOnLayer(layer, animation, coercePlayMode(mode));
}

void TinyScreen::playOnLayer(int layer, const Animation& animation, int mode, int startFrame, int endFrame) {
    playOnLayer(layer, animation, coercePlayMode(mode), startFrame, endFrame);
}

void TinyScreen::setSpeedOnLayer(int layer, int speedMs) {
    if (layer >= 0 && layer < layerCount) {
        layers[layer].setSpeed(speedMs);
    }
}

void TinyScreen::setSpeedOnLayer(int layer, float speedMultiplier) {
    if (layer >= 0 && layer < layerCount) {
        layers[layer].setSpeed(speedMultiplier);
    }
}

void TinyScreen::setSpeedOnLayer(int layer, double speedMultiplier) {
    setSpeedOnLayer(layer, (float)speedMultiplier);
}

void TinyScreen::pauseLayer(int layer) {
    if (layer >= 0 && layer < layerCount) {
        layers[layer].pause();
    }
}

void TinyScreen::resumeLayer(int layer) {
    if (layer >= 0 && layer < layerCount) {
        layers[layer].resume();
    }
}

void TinyScreen::stopLayer(int layer) {
    if (layer >= 0 && layer < layerCount) {
        layers[layer].stop();
    }
}

//--- Primary Layer Control ---

void TinyScreen::setSpeed(int speedMs) {
    primary().setSpeed(speedMs);
}

void TinyScreen::setSpeed(float speedMultiplier) {
    primary().setSpeed(speedMultiplier);
}

void TinyScreen::setSpeed(double speedMultiplier) {
    setSpeed((float)speedMultiplier);
}

void TinyScreen::pause() {
    primary().pause();
}

void TinyScreen::resume() {
    primary().resume();
}

void TinyScreen::restoreOriginalSpeed() {
    primary().restoreOriginalSpeed();
}

void TinyScreen::stop() {
    primary().stop();
}

//--- Animation Position ---

void TinyScreen::setPosition(int x, int y) {
    primary().setOffset(x, y);
}

void TinyScreen::setPositionOnLayer(int layer, int x, int y) {
    if (layer >= 0 && layer < layerCount) {
        layers[layer].setOffset(x, y);
    }
}

int TinyScreen::getPositionX() const {
    return primary().getOffsetX();
}

int TinyScreen::getPositionY() const {
    return primary().getOffsetY();
}

int TinyScreen::getPositionXOnLayer(int layer) const {
    if (layer >= 0 && layer < layerCount) {
        return layers[layer].getOffsetX();
    }
    return 0;
}

int TinyScreen::getPositionYOnLayer(int layer) const {
    if (layer >= 0 && layer < layerCount) {
        return layers[layer].getOffsetY();
    }
    return 0;
}

//--- Frame Update ---

void TinyScreen::update() {
    // Clear combined frame
    combinedFrame[0] = 0;
    combinedFrame[1] = 0;
    combinedFrame[2] = 0;

    // Update each layer and combine frames
    uint32_t frame[3];
    for (int i = 0; i < layerCount; i++) {
        layers[i].updateFrame();
        if (layers[i].getFrame(frame)) {
            combinedFrame[0] |= frame[0];
            combinedFrame[1] |= frame[1];
            combinedFrame[2] |= frame[2];
        }
    }

    // Apply invert if enabled
    if (invertDisplay) {
        combinedFrame[0] = ~combinedFrame[0];
        combinedFrame[1] = ~combinedFrame[1];
        combinedFrame[2] = ~combinedFrame[2];
    }

    // Display combined frame
    matrix.loadFrame(combinedFrame);
}

//==============================================================================
// Canvas Mode - Buffered Drawing (Flicker-Free)
//==============================================================================

// Convert canvas buffer to 96-bit frame format with rotation
void TinyScreen::canvasBufferToFrame(uint32_t frame[3]) {
    frame[0] = 0;
    frame[1] = 0;
    frame[2] = 0;
    
    // The LED matrix frame format is 3 x 32-bit words = 96 bits
    // Bit 0 of frame[0] = LED at position 0 (top-left)
    // Layout: row-major, 12 columns x 8 rows
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 12; col++) {
            // Apply rotation to get source coordinates
            int srcRow, srcCol;
            switch (rotation) {
                case 1: { // 90° clockwise (center-cropped)
                    const int xOffset = 2; // (12 - 8) / 2
                    const int yOffset = 2; // (12 - 8) / 2
                    int rx = col - xOffset;     // rotated x in 0..7
                    int ry = row + yOffset;     // rotated y in 0..11
                    srcRow = (rx >= 0 && rx < 8 && ry >= 0 && ry < 12) ? (7 - rx) : -1;
                    srcCol = ry;
                    break;
                }
                case 2:  // 180°
                    srcRow = 7 - row;
                    srcCol = 11 - col;
                    break;
                case 3: { // 270° clockwise (center-cropped)
                    const int xOffset = 2; // (12 - 8) / 2
                    const int yOffset = 2; // (12 - 8) / 2
                    int rx = col - xOffset;     // rotated x in 0..7
                    int ry = row + yOffset;     // rotated y in 0..11
                    srcRow = rx;
                    srcCol = (rx >= 0 && rx < 8 && ry >= 0 && ry < 12) ? (11 - ry) : -1;
                    break;
                }
                default: // 0° (no rotation)
                    srcRow = row;
                    srcCol = col;
                    break;
            }
            
            // Check bounds and get pixel value
            bool pixelOn = false;
            if (srcRow >= 0 && srcRow < 8 && srcCol >= 0 && srcCol < 12) {
                pixelOn = canvasBuffer[srcRow][srcCol];
            }
            
            if (pixelOn) {
                int bitIndex = row * 12 + col;
                if (bitIndex < 32) {
                    frame[0] |= (1UL << (31 - bitIndex));
                } else if (bitIndex < 64) {
                    frame[1] |= (1UL << (63 - bitIndex));
                } else {
                    frame[2] |= (1UL << (95 - bitIndex));
                }
            }
        }
    }
}

// Set display rotation (0, 90, 180, or 270 degrees)
void TinyScreen::setRotation(int degrees) {
    switch (degrees) {
        case 90:  rotation = 1; break;
        case 180: rotation = 2; break;
        case 270: rotation = 3; break;
        default:  rotation = 0; break;
    }
}

// Get current rotation in degrees
int TinyScreen::getRotation() {
    return rotation * 90;
}

// Set display invert (flip all pixels)
void TinyScreen::setInvert(bool invert) {
    invertDisplay = invert;
}

// Get current invert state
bool TinyScreen::getInvert() {
    return invertDisplay;
}

// Load a 96-bit frame into the canvas buffer (for hybrid mode)
void TinyScreen::frameToBuffer(const uint32_t frame[3]) {
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 12; col++) {
            int bitIndex = row * 12 + col;
            bool pixelOn = false;
            if (bitIndex < 32) {
                pixelOn = (frame[0] & (1UL << (31 - bitIndex))) != 0;
            } else if (bitIndex < 64) {
                pixelOn = (frame[1] & (1UL << (63 - bitIndex))) != 0;
            } else {
                pixelOn = (frame[2] & (1UL << (95 - bitIndex))) != 0;
            }
            canvasBuffer[row][col] = pixelOn ? 1 : 0;
        }
    }
}

void TinyScreen::beginDraw() {
    inCanvasDraw = true;
    // Don't clear buffer here - let user call clear() explicitly if needed
}

void TinyScreen::endDraw() {
    if (inCanvasDraw) {
        // Convert buffer to frame and display
        uint32_t frame[3];
        canvasBufferToFrame(frame);
        
        // Apply invert if enabled
        if (invertDisplay) {
            frame[0] = ~frame[0];
            frame[1] = ~frame[1];
            frame[2] = ~frame[2];
        }
        
        matrix.loadFrame(frame);
        inCanvasDraw = false;
    }
}

void TinyScreen::clear() {
    // Alias for background(OFF) - clears all pixels
    background(OFF);
}

void TinyScreen::bufferPoint(int x, int y, uint8_t value) {
    if (x >= 0 && x < 12 && y >= 0 && y < 8) {
        canvasBuffer[y][x] = value;
    }
}

void TinyScreen::set(int x, int y, bool on) {
    if (x >= 0 && x < 12 && y >= 0 && y < 8) {
        canvasBuffer[y][x] = on ? 1 : 0;
    }
}

void TinyScreen::point(int x, int y) {
    if (strokeEnabled) {
        bufferPoint(x, y, strokeValue);
    }
}

// Bresenham's line algorithm
void TinyScreen::bufferLine(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    
    while (true) {
        bufferPoint(x1, y1, strokeValue);
        
        if (x1 == x2 && y1 == y2) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void TinyScreen::line(int x1, int y1, int x2, int y2) {
    if (strokeEnabled) {
        bufferLine(x1, y1, x2, y2);
    }
}

void TinyScreen::bufferRect(int x, int y, int w, int h) {
    // Fill first if enabled
    if (fillEnabled) {
        for (int row = y; row < y + h; row++) {
            for (int col = x; col < x + w; col++) {
                bufferPoint(col, row, fillValue);
            }
        }
    }
    
    // Stroke (outline) if enabled
    if (strokeEnabled) {
        // Top and bottom edges
        for (int col = x; col < x + w; col++) {
            bufferPoint(col, y, strokeValue);
            bufferPoint(col, y + h - 1, strokeValue);
        }
        // Left and right edges
        for (int row = y; row < y + h; row++) {
            bufferPoint(x, row, strokeValue);
            bufferPoint(x + w - 1, row, strokeValue);
        }
    }
}

void TinyScreen::rect(int x, int y, int width, int height) {
    bufferRect(x, y, width, height);
}

// Midpoint circle algorithm
void TinyScreen::bufferCircle(int cx, int cy, int r) {
    if (r <= 0) {
        if (fillEnabled) bufferPoint(cx, cy, fillValue);
        else if (strokeEnabled) bufferPoint(cx, cy, strokeValue);
        return;
    }
    
    int x = r;
    int y = 0;
    int err = 1 - r;
    
    while (x >= y) {
        // Draw 8 octants for outline
        if (strokeEnabled && !fillEnabled) {
            bufferPoint(cx + x, cy + y, strokeValue);
            bufferPoint(cx - x, cy + y, strokeValue);
            bufferPoint(cx + x, cy - y, strokeValue);
            bufferPoint(cx - x, cy - y, strokeValue);
            bufferPoint(cx + y, cy + x, strokeValue);
            bufferPoint(cx - y, cy + x, strokeValue);
            bufferPoint(cx + y, cy - x, strokeValue);
            bufferPoint(cx - y, cy - x, strokeValue);
        }
        
        // Fill horizontal spans if fill enabled
        if (fillEnabled) {
            for (int i = cx - x; i <= cx + x; i++) {
                bufferPoint(i, cy + y, fillValue);
                bufferPoint(i, cy - y, fillValue);
            }
            for (int i = cx - y; i <= cx + y; i++) {
                bufferPoint(i, cy + x, fillValue);
                bufferPoint(i, cy - x, fillValue);
            }
        }
        
        y++;
        if (err < 0) {
            err += 2 * y + 1;
        } else {
            x--;
            err += 2 * (y - x) + 1;
        }
    }
}

void TinyScreen::circle(int cx, int cy, int diameter) {
    int radius = diameter / 2;
    bufferCircle(cx, cy, radius);
}

void TinyScreen::ellipse(int cx, int cy, int width, int height) {
    // Simple ellipse using parametric approach
    int a = width / 2;   // semi-major axis
    int b = height / 2;  // semi-minor axis
    
    if (a <= 0 || b <= 0) {
        if (fillEnabled) bufferPoint(cx, cy, fillValue);
        else if (strokeEnabled) bufferPoint(cx, cy, strokeValue);
        return;
    }
    
    // Midpoint ellipse algorithm
    int x = 0;
    int y = b;
    
    // Region 1
    int d1 = (b * b) - (a * a * b) + (a * a / 4);
    int dx = 2 * b * b * x;
    int dy = 2 * a * a * y;
    
    while (dx < dy) {
        if (strokeEnabled && !fillEnabled) {
            bufferPoint(cx + x, cy + y, strokeValue);
            bufferPoint(cx - x, cy + y, strokeValue);
            bufferPoint(cx + x, cy - y, strokeValue);
            bufferPoint(cx - x, cy - y, strokeValue);
        }
        if (fillEnabled) {
            for (int i = cx - x; i <= cx + x; i++) {
                bufferPoint(i, cy + y, fillValue);
                bufferPoint(i, cy - y, fillValue);
            }
        }
        
        x++;
        dx += 2 * b * b;
        if (d1 < 0) {
            d1 += dx + b * b;
        } else {
            y--;
            dy -= 2 * a * a;
            d1 += dx - dy + b * b;
        }
    }
    
    // Region 2
    int d2 = b * b * (x * x + x) + a * a * (y - 1) * (y - 1) - a * a * b * b;
    
    while (y >= 0) {
        if (strokeEnabled && !fillEnabled) {
            bufferPoint(cx + x, cy + y, strokeValue);
            bufferPoint(cx - x, cy + y, strokeValue);
            bufferPoint(cx + x, cy - y, strokeValue);
            bufferPoint(cx - x, cy - y, strokeValue);
        }
        if (fillEnabled) {
            for (int i = cx - x; i <= cx + x; i++) {
                bufferPoint(i, cy + y, fillValue);
                bufferPoint(i, cy - y, fillValue);
            }
        }
        
        y--;
        dy -= 2 * a * a;
        if (d2 > 0) {
            d2 += a * a - dy;
        } else {
            x++;
            dx += 2 * b * b;
            d2 += dx - dy + a * a;
        }
    }
}

//--- Style Control ---

void TinyScreen::stroke(uint32_t color) {
    strokeEnabled = true;
    strokeValue = (color != 0) ? 1 : 0;
    matrix.stroke(color);  // Keep for text rendering
}

void TinyScreen::stroke(uint8_t r, uint8_t g, uint8_t b) {
    strokeEnabled = true;
    strokeValue = (r != 0 || g != 0 || b != 0) ? 1 : 0;
    matrix.stroke(r, g, b);
}

void TinyScreen::fill(uint32_t color) {
    fillEnabled = true;
    fillValue = (color != 0) ? 1 : 0;
    matrix.fill(color);  // Keep for text rendering
}

void TinyScreen::fill(uint8_t r, uint8_t g, uint8_t b) {
    fillEnabled = true;
    fillValue = (r != 0 || g != 0 || b != 0) ? 1 : 0;
    matrix.fill(r, g, b);
}

void TinyScreen::background(uint32_t color) {
    // For background, fill the entire canvas buffer
    uint8_t val = (color != 0) ? 1 : 0;
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 12; col++) {
            canvasBuffer[row][col] = val;
        }
    }
    matrix.background(color);
}

void TinyScreen::background(uint8_t r, uint8_t g, uint8_t b) {
    uint8_t val = (r != 0 || g != 0 || b != 0) ? 1 : 0;
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 12; col++) {
            canvasBuffer[row][col] = val;
        }
    }
    matrix.background(r, g, b);
}

void TinyScreen::noStroke() {
    strokeEnabled = false;
    matrix.noStroke();
}

void TinyScreen::noFill() {
    fillEnabled = false;
    matrix.noFill();
}

//--- Text Methods (Buffered) ---

// Simple 3x5 font for canvas buffer rendering
// Each character is 3 pixels wide, 5 pixels tall
// Stored as 5 bytes (rows), 3 bits used per row (MSB first)
static const uint8_t font3x5[][5] PROGMEM = {
    // Space (32)
    {0b000, 0b000, 0b000, 0b000, 0b000},
    // ! (33)
    {0b010, 0b010, 0b010, 0b000, 0b010},
    // " (34)
    {0b101, 0b101, 0b000, 0b000, 0b000},
    // # (35)
    {0b101, 0b111, 0b101, 0b111, 0b101},
    // $ (36)
    {0b111, 0b110, 0b111, 0b011, 0b111},
    // % (37)
    {0b101, 0b001, 0b010, 0b100, 0b101},
    // & (38)
    {0b010, 0b101, 0b010, 0b101, 0b011},
    // ' (39)
    {0b010, 0b010, 0b000, 0b000, 0b000},
    // ( (40)
    {0b001, 0b010, 0b010, 0b010, 0b001},
    // ) (41)
    {0b100, 0b010, 0b010, 0b010, 0b100},
    // * (42)
    {0b101, 0b010, 0b111, 0b010, 0b101},
    // + (43)
    {0b000, 0b010, 0b111, 0b010, 0b000},
    // , (44)
    {0b000, 0b000, 0b000, 0b010, 0b100},
    // - (45)
    {0b000, 0b000, 0b111, 0b000, 0b000},
    // . (46)
    {0b000, 0b000, 0b000, 0b000, 0b010},
    // / (47)
    {0b001, 0b001, 0b010, 0b100, 0b100},
    // 0 (48)
    {0b111, 0b101, 0b101, 0b101, 0b111},
    // 1 (49)
    {0b010, 0b110, 0b010, 0b010, 0b111},
    // 2 (50)
    {0b111, 0b001, 0b111, 0b100, 0b111},
    // 3 (51)
    {0b111, 0b001, 0b111, 0b001, 0b111},
    // 4 (52)
    {0b101, 0b101, 0b111, 0b001, 0b001},
    // 5 (53)
    {0b111, 0b100, 0b111, 0b001, 0b111},
    // 6 (54)
    {0b111, 0b100, 0b111, 0b101, 0b111},
    // 7 (55)
    {0b111, 0b001, 0b001, 0b001, 0b001},
    // 8 (56)
    {0b111, 0b101, 0b111, 0b101, 0b111},
    // 9 (57)
    {0b111, 0b101, 0b111, 0b001, 0b111},
    // : (58)
    {0b000, 0b010, 0b000, 0b010, 0b000},
    // ; (59)
    {0b000, 0b010, 0b000, 0b010, 0b100},
    // < (60)
    {0b001, 0b010, 0b100, 0b010, 0b001},
    // = (61)
    {0b000, 0b111, 0b000, 0b111, 0b000},
    // > (62)
    {0b100, 0b010, 0b001, 0b010, 0b100},
    // ? (63)
    {0b111, 0b001, 0b011, 0b000, 0b010},
    // @ (64)
    {0b011, 0b101, 0b111, 0b100, 0b011},
    // A (65)
    {0b010, 0b101, 0b111, 0b101, 0b101},
    // B (66)
    {0b110, 0b101, 0b110, 0b101, 0b110},
    // C (67)
    {0b011, 0b100, 0b100, 0b100, 0b011},
    // D (68)
    {0b110, 0b101, 0b101, 0b101, 0b110},
    // E (69)
    {0b111, 0b100, 0b110, 0b100, 0b111},
    // F (70)
    {0b111, 0b100, 0b110, 0b100, 0b100},
    // G (71)
    {0b011, 0b100, 0b101, 0b101, 0b011},
    // H (72)
    {0b101, 0b101, 0b111, 0b101, 0b101},
    // I (73)
    {0b111, 0b010, 0b010, 0b010, 0b111},
    // J (74)
    {0b001, 0b001, 0b001, 0b101, 0b010},
    // K (75)
    {0b101, 0b101, 0b110, 0b101, 0b101},
    // L (76)
    {0b100, 0b100, 0b100, 0b100, 0b111},
    // M (77)
    {0b101, 0b111, 0b101, 0b101, 0b101},
    // N (78)
    {0b101, 0b111, 0b111, 0b101, 0b101},
    // O (79)
    {0b010, 0b101, 0b101, 0b101, 0b010},
    // P (80)
    {0b110, 0b101, 0b110, 0b100, 0b100},
    // Q (81)
    {0b010, 0b101, 0b101, 0b111, 0b011},
    // R (82)
    {0b110, 0b101, 0b110, 0b101, 0b101},
    // S (83)
    {0b011, 0b100, 0b010, 0b001, 0b110},
    // T (84)
    {0b111, 0b010, 0b010, 0b010, 0b010},
    // U (85)
    {0b101, 0b101, 0b101, 0b101, 0b111},
    // V (86)
    {0b101, 0b101, 0b101, 0b101, 0b010},
    // W (87)
    {0b101, 0b101, 0b101, 0b111, 0b101},
    // X (88)
    {0b101, 0b101, 0b010, 0b101, 0b101},
    // Y (89)
    {0b101, 0b101, 0b010, 0b010, 0b010},
    // Z (90)
    {0b111, 0b001, 0b010, 0b100, 0b111},
};

// Internal helper: Draw a character with scaling
void TinyScreen::bufferCharScaled(char c, int x, int y, uint8_t value, int scale) {
    int index = -1;
    
    // Map character to font index
    if (c >= 32 && c <= 90) {
        index = c - 32;
    } else if (c >= 'a' && c <= 'z') {
        // Lowercase maps to uppercase
        index = c - 'a' + 33;  // 'A' is at index 33 (65-32)
    }
    
    if (index < 0 || index >= 59) return;
    
    // Draw 3x5 character with scaling
    for (int row = 0; row < 5; row++) {
        uint8_t rowData = pgm_read_byte(&font3x5[index][row]);
        for (int col = 0; col < 3; col++) {
            if (rowData & (0b100 >> col)) {
                // Draw scaled pixel (scale x scale block)
                for (int sy = 0; sy < scale; sy++) {
                    for (int sx = 0; sx < scale; sx++) {
                        bufferPoint(x + col * scale + sx, y + row * scale + sy, value);
                    }
                }
            }
        }
    }
}

// Draw a single character to the canvas buffer
void TinyScreen::textChar(char c, int x, int y) {
    uint8_t value = strokeEnabled ? strokeValue : 1;
    bufferCharScaled(c, x, y, value, textSize);
}

// Set text size (1 = normal 3x5, 2 = double 6x10)
void TinyScreen::setTextSize(int size) {
    textSize = (size < 1) ? 1 : ((size > 2) ? 2 : size);
}

// Draw a string to the canvas buffer
void TinyScreen::text(const char* str, int x, int y) {
    int charWidth = 3 * textSize + 1;  // char width + 1 pixel spacing
    int cursorX = x;
    while (*str) {
        textChar(*str, cursorX, y);
        cursorX += charWidth;
        str++;
    }
}

void TinyScreen::text(const String& str, int x, int y) {
    text(str.c_str(), x, y);
}

// Set scroll speed (milliseconds per pixel)
void TinyScreen::setScrollSpeed(unsigned long ms) {
    scrollSpeed = ms;
}

// Reset scroll position
void TinyScreen::resetScroll() {
    scrollOffset = 0;
    lastScrollTime = millis();
}

// Get current scroll offset
int TinyScreen::getScrollOffset() {
    return scrollOffset;
}

// Draw scrolling text - call this in your loop
void TinyScreen::scrollText(const char* str, int y, int direction) {
    int charWidth = 3 * textSize + 1;
    int textWidth = strlen(str) * charWidth;
    
    // Update scroll position based on time
    unsigned long now = millis();
    if (now - lastScrollTime >= scrollSpeed) {
        if (direction == SCROLL_LEFT) {
            scrollOffset++;
            if (scrollOffset > textWidth) {
                scrollOffset = -12;  // Reset to right edge
            }
        } else if (direction == SCROLL_RIGHT) {
            scrollOffset--;
            if (scrollOffset < -textWidth) {
                scrollOffset = 12;  // Reset to left edge
            }
        }
        lastScrollTime = now;
    }
    
    // Draw text at scrolled position
    int drawX = (direction == SCROLL_LEFT) ? (12 - scrollOffset) : scrollOffset;
    text(str, drawX, y);
}

void TinyScreen::scrollText(const String& str, int y, int direction) {
    scrollText(str.c_str(), y, direction);
}

// Legacy methods that use ArduinoGraphics (won't work with buffered drawing)
void TinyScreen::textFont(const Font& font) {
    matrix.textFont(font);
}

int TinyScreen::textFontWidth() {
    return textSize * 3;  // Return our font width
}

int TinyScreen::textFontHeight() {
    return textSize * 5;  // Return our font height
}

//--- Legacy Scrolling Text (ArduinoGraphics) ---

void TinyScreen::beginText(int x, int y) {
    matrix.beginText(x, y);
}

void TinyScreen::beginText(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    matrix.beginText(x, y, r, g, b);
}

void TinyScreen::beginText(int x, int y, uint32_t color) {
    matrix.beginText(x, y, color);
}

void TinyScreen::endText(int scrollDirection) {
    matrix.endText(scrollDirection);
}

void TinyScreen::textScrollSpeed(unsigned long speed) {
    matrix.textScrollSpeed(speed);
}

//--- Print Interface ---

size_t TinyScreen::print(const char* str) {
    return matrix.print(str);
}

size_t TinyScreen::print(const String& str) {
    return matrix.print(str);
}

size_t TinyScreen::print(char c) {
    return matrix.print(c);
}

size_t TinyScreen::print(int n) {
    return matrix.print(n);
}

size_t TinyScreen::println() {
    return matrix.println();
}

size_t TinyScreen::println(const char* str) {
    return matrix.println(str);
}

size_t TinyScreen::println(const String& str) {
    return matrix.println(str);
}

//--- Hybrid Mode ---

void TinyScreen::beginOverlay() {
    inOverlay = true;
    // Update animations first
    combinedFrame[0] = 0;
    combinedFrame[1] = 0;
    combinedFrame[2] = 0;
    
    uint32_t frame[3];
    for (int i = 0; i < layerCount; i++) {
        layers[i].updateFrame();
        if (layers[i].getFrame(frame)) {
            combinedFrame[0] |= frame[0];
            combinedFrame[1] |= frame[1];
            combinedFrame[2] |= frame[2];
        }
    }
    
    // Load animation frame into canvas buffer for drawing on top
    frameToBuffer(combinedFrame);
    inOverlay = true;
    inCanvasDraw = true;
    
    // Set default drawing style for overlay
    strokeEnabled = true;
    strokeValue = 1;
}

void TinyScreen::endOverlay() {
    if (inOverlay) {
        // Convert buffer to frame and display
        uint32_t frame[3];
        canvasBufferToFrame(frame);
        
        // Apply invert if enabled
        if (invertDisplay) {
            frame[0] = ~frame[0];
            frame[1] = ~frame[1];
            frame[2] = ~frame[2];
        }
        
        matrix.loadFrame(frame);
        inOverlay = false;
        inCanvasDraw = false;
    }
}

void TinyScreen::displayFrame(const uint32_t frame[3]) {
    matrix.loadFrame(frame);
}

//==============================================================================
// CombinedFilmFestival (Backward Compatibility)
//==============================================================================

bool CombinedFilmFestival::addFilm(TinyScreen& film) {
    // This is a simplified backward-compat layer
    // In the new model, layers are internal to TinyScreen
    // For legacy code, we just track that something was added
    if (count >= MAX_LAYERS) return false;
    count++;
    return true;
}

//==============================================================================
// TinyScreen LED Control Methods (Proposal 2)
//==============================================================================

void TinyScreen::indexToXY(int index, int& x, int& y) {
    // Matrix is 12 columns x 8 rows = 96 LEDs
    // Index 0 is top-left (0,0), index 95 is bottom-right (11,7)
    index = constrain(index, 0, 95);
    x = index % 12;
    y = index / 12;
}

void TinyScreen::led(int x, int y, bool state) {
    if (x < 0 || x >= 12 || y < 0 || y >= 8) return;
    
    blinkRate[x][y] = 0;  // Direct write cancels blink on this LED
    ledBuffer[x][y] = state ? 1 : 0;
    ledBufferDirty = true;
    if (autoShow) {
        show();
    }
}

void TinyScreen::led(int ledNum, bool state) {
    int x, y;
    indexToXY(ledNum, x, y);
    led(x, y, state);
}

bool TinyScreen::getLed(int x, int y) {
    if (x < 0 || x >= 12 || y < 0 || y >= 8) return false;
    return ledBuffer[x][y] != 0;
}

bool TinyScreen::getLed(int ledNum) {
    int x, y;
    indexToXY(ledNum, x, y);
    return getLed(x, y);
}

void TinyScreen::toggle(int x, int y) {
    if (x < 0 || x >= 12 || y < 0 || y >= 8) return;
    
    blinkRate[x][y] = 0;  // Manual toggle cancels blink on this LED
    ledBuffer[x][y] = ledBuffer[x][y] ? 0 : 1;
    ledBufferDirty = true;
    if (autoShow) {
        show();
    }
}

void TinyScreen::toggle(int ledNum) {
    int x, y;
    indexToXY(ledNum, x, y);
    toggle(x, y);
}

void TinyScreen::clearLeds() {
    for (int x = 0; x < 12; x++) {
        for (int y = 0; y < 8; y++) {
            ledBuffer[x][y] = 0;
            blinkRate[x][y] = 0;
            lastBlinkTime[x][y] = 0;
        }
    }
    ledBufferDirty = true;
    if (autoShow) {
        show();
    }
}

void TinyScreen::show() {
    if (!ledBufferDirty) return;
    
    // Convert buffer to frame format and display
    // The LED matrix uses 3 x 32-bit words to store 12x8 = 96 bits
    uint32_t frame[3] = {0, 0, 0};
    
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 12; x++) {
            if (ledBuffer[x][y]) {
                int bitIndex = y * 12 + x;
                int wordIndex = bitIndex / 32;
                int bitPosition = 31 - (bitIndex % 32);
                frame[wordIndex] |= (1UL << bitPosition);
            }
        }
    }
    
    matrix.loadFrame(frame);
    ledBufferDirty = false;
}

//==============================================================================
// Blink Control Methods
//==============================================================================

void TinyScreen::blink(int x, int y, unsigned long rateMs) {
    if (x < 0 || x >= 12 || y < 0 || y >= 8) return;
    if (rateMs == 0) { noBlink(x, y); return; }
    
    blinkRate[x][y] = rateMs;
    lastBlinkTime[x][y] = millis();
    ledBuffer[x][y] = 1;      // Start in ON state
    ledBufferDirty = true;
    if (autoShow) {
        show();
    }
}

void TinyScreen::blink(int ledNum, unsigned long rateMs) {
    int x, y;
    indexToXY(ledNum, x, y);
    blink(x, y, rateMs);
}

void TinyScreen::noBlink(int x, int y) {
    if (x < 0 || x >= 12 || y < 0 || y >= 8) return;
    
    blinkRate[x][y] = 0;
    lastBlinkTime[x][y] = 0;
    ledBuffer[x][y] = 0;      // Turn LED off when blinking stops
    ledBufferDirty = true;
    if (autoShow) {
        show();
    }
}

void TinyScreen::noBlink(int ledNum) {
    int x, y;
    indexToXY(ledNum, x, y);
    noBlink(x, y);
}

void TinyScreen::noBlink() {
    for (int x = 0; x < 12; x++) {
        for (int y = 0; y < 8; y++) {
            blinkRate[x][y] = 0;
            lastBlinkTime[x][y] = 0;
        }
    }
    // Don't change ledBuffer — non-blinking LEDs keep their state
}

void TinyScreen::updateBlinks() {
    unsigned long now = millis();
    bool anyChanged = false;
    
    for (int x = 0; x < 12; x++) {
        for (int y = 0; y < 8; y++) {
            if (blinkRate[x][y] == 0) continue;  // Not blinking
            
            if (now - lastBlinkTime[x][y] >= blinkRate[x][y]) {
                lastBlinkTime[x][y] = now;
                ledBuffer[x][y] = ledBuffer[x][y] ? 0 : 1;
                anyChanged = true;
            }
        }
    }
    
    if (anyChanged) {
        ledBufferDirty = true;
        if (autoShow) {
            show();
        }
    }
}

void TinyScreen::setAutoShow(bool enabled) {
    autoShow = enabled;
}

//==============================================================================
// Standalone LED Functions (Proposal 1)
//==============================================================================

// Internal default TinyScreen instance for standalone functions
static TinyScreen* _defaultScreen = nullptr;

static TinyScreen& getDefaultScreen() {
    if (_defaultScreen == nullptr) {
        _defaultScreen = new TinyScreen();
    }
    return *_defaultScreen;
}

void ledBegin() {
    getDefaultScreen().begin();
}

void ledWrite(int x, int y, bool state) {
    getDefaultScreen().led(x, y, state);
}

void ledWrite(int ledNum, bool state) {
    getDefaultScreen().led(ledNum, state);
}

bool ledRead(int x, int y) {
    return getDefaultScreen().getLed(x, y);
}

bool ledRead(int ledNum) {
    return getDefaultScreen().getLed(ledNum);
}

void ledToggle(int x, int y) {
    getDefaultScreen().toggle(x, y);
}

void ledToggle(int ledNum) {
    getDefaultScreen().toggle(ledNum);
}

void ledClear() {
    getDefaultScreen().clearLeds();
}

void ledShow() {
    getDefaultScreen().show();
}

void ledBlink(int x, int y, unsigned long rateMs) {
    getDefaultScreen().blink(x, y, rateMs);
}

void ledBlink(int ledNum, unsigned long rateMs) {
    getDefaultScreen().blink(ledNum, rateMs);
}

void ledNoBlink(int x, int y) {
    getDefaultScreen().noBlink(x, y);
}

void ledNoBlink(int ledNum) {
    getDefaultScreen().noBlink(ledNum);
}

void ledNoBlink() {
    getDefaultScreen().noBlink();
}

void ledUpdate() {
    getDefaultScreen().updateBlinks();
}

TinyScreen& getLedMatrix() {
    return getDefaultScreen();
}

//------------------------------------------------------------------------------
// Animation Utilities Implementation
//------------------------------------------------------------------------------

float oscillate(float min, float max, unsigned long periodMs, float offset) {
    if (periodMs == 0) {
        return min;
    }
    // Use millis() to create a continuous sine wave
    float t = (float)(millis() % periodMs) / (float)periodMs + offset;
    float sineValue = sin(t * 2.0f * PI);  // -1 to 1
    float normalized = (sineValue + 1.0f) / 2.0f;  // 0 to 1
    return min + normalized * (max - min);
}

int oscillateInt(int min, int max, unsigned long periodMs, float offset) {
    return (int)round(oscillate((float)min, (float)max, periodMs, offset));
}

//------------------------------------------------------------------------------
// Ease Class Implementation
//------------------------------------------------------------------------------

Ease::Ease(float initial) 
    : _current(initial), _start(initial), _target(initial), 
      _startTime(0), _duration(0), _active(false) {
}

void Ease::to(float target, unsigned long durationMs) {
    _start = _current;
    _target = target;
    _startTime = millis();
    _duration = durationMs;
    _active = true;
}

float Ease::value() {
    if (!_active) {
        return _current;
    }
    
    unsigned long elapsed = millis() - _startTime;
    
    if (elapsed >= _duration) {
        // Animation complete
        _current = _target;
        _active = false;
        return _current;
    }
    
    // Linear interpolation
    float t = (float)elapsed / (float)_duration;
    _current = _start + t * (_target - _start);
    return _current;
}

int Ease::intValue() {
    return (int)round(value());
}

bool Ease::done() {
    // Call value() to update state, then check
    value();
    return !_active;
}

bool Ease::moving() {
    value();
    return _active;
}

void Ease::jump(float v) {
    _current = v;
    _target = v;
    _active = false;
}

float Ease::target() {
    return _target;
}
