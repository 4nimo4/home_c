// ============================================
// TinyFilmFestival Documentation App
// ============================================

document.addEventListener('DOMContentLoaded', () => {
    const sidebar = document.getElementById('sidebar');
    const overlay = document.getElementById('overlay');
    const menuToggle = document.querySelector('.menu-toggle');
    const content = document.getElementById('content');
    const navLinks = document.querySelectorAll('.nav-section a[data-page]');
    const searchInput = document.getElementById('search-input');
    const searchResults = document.getElementById('search-results');

    // Search functionality
    searchInput.addEventListener('input', (e) => {
        const query = e.target.value.toLowerCase().trim();
        
        if (query.length < 2)
        {
            searchResults.innerHTML = '';
            searchResults.classList.remove('visible');
            return;
        }
        
        const results = searchAPI(query);
        displaySearchResults(results, query);
    });

    searchInput.addEventListener('focus', () => {
        if (searchInput.value.length >= 2)
        {
            searchResults.classList.add('visible');
        }
    });

    document.addEventListener('click', (e) => {
        if (!e.target.closest('.search-container'))
        {
            searchResults.classList.remove('visible');
        }
    });

    // Mobile menu toggle
    menuToggle.addEventListener('click', () => {
        sidebar.classList.toggle('open');
        overlay.classList.toggle('visible');
    });

    overlay.addEventListener('click', () => {
        sidebar.classList.remove('open');
        overlay.classList.remove('visible');
    });

    // Navigation - regular links
    navLinks.forEach(link => {
        link.addEventListener('click', (e) => {
            e.preventDefault();
            const page = link.dataset.page;
            
            // Check if this is an expandable toggle
            if (link.classList.contains('nav-expand-toggle'))
            {
                const parent = link.closest('.nav-expandable');
                parent.classList.toggle('expanded');
                // Don't navigate, just expand
                return;
            }
            
            // Update active state
            navLinks.forEach(l => l.classList.remove('active'));
            link.classList.add('active');
            
            // Load page content
            loadPage(page);
            
            // Close mobile menu
            sidebar.classList.remove('open');
            overlay.classList.remove('visible');
            
            // Scroll to top
            window.scrollTo(0, 0);
        });
    });

    // Load initial page
    const hash = window.location.hash.slice(1) || 'home';
    loadPage(hash);

    // Handle back/forward navigation
    window.addEventListener('popstate', () => {
        const hash = window.location.hash.slice(1) || 'home';
        loadPage(hash, false);
    });
});

function loadPage(page, updateHistory = true) {
    const content = document.getElementById('content');
    const pageContent = pages[page] || pages['home'];
    
    content.innerHTML = pageContent;
    
    // Update URL
    if (updateHistory)
    {
        history.pushState(null, '', `#${page}`);
    }
    
    // Update active nav link
    document.querySelectorAll('.nav-section a[data-page]').forEach(link => {
        link.classList.toggle('active', link.dataset.page === page);
    });
    
    // Re-run Prism for syntax highlighting
    if (typeof Prism !== 'undefined')
    {
        Prism.highlightAll();
    }
    
    // Add click handlers for mode cards and other internal links
    content.querySelectorAll('[data-page]').forEach(el => {
        el.addEventListener('click', (e) => {
            e.preventDefault();
            loadPage(el.dataset.page);
            window.scrollTo(0, 0);
        });
    });
}

// Search index - methods and their locations
const searchIndex = [
    // Simple LED Mode
    { method: 'ledBegin()', description: 'Initialize the LED matrix', page: 'simple-led', category: 'Simple LED' },
    { method: 'ledWrite(x, y, state)', description: 'Set LED at coordinates to HIGH or LOW', page: 'simple-led', category: 'Simple LED' },
    { method: 'ledWrite(index, state)', description: 'Set LED by linear index (0-95)', page: 'simple-led', category: 'Simple LED' },
    { method: 'ledRead(x, y)', description: 'Read current state of an LED', page: 'simple-led', category: 'Simple LED' },
    { method: 'ledToggle(x, y)', description: 'Toggle LED between ON and OFF', page: 'simple-led', category: 'Simple LED' },
    { method: 'ledClear()', description: 'Turn off all LEDs', page: 'simple-led', category: 'Simple LED' },
    { method: 'setAutoShow(enabled)', description: 'Enable/disable auto display after LED writes', page: 'simple-led', category: 'Simple LED' },
    
    // LED Blink
    { method: 'ledBlink(x, y, rateMs)', description: 'Blink LED at (x,y) every rateMs milliseconds', page: 'led-blink', category: 'LED Blink' },
    { method: 'ledBlink(index, rateMs)', description: 'Blink LED by linear index (0-95)', page: 'led-blink', category: 'LED Blink' },
    { method: 'ledNoBlink(x, y)', description: 'Stop blinking LED at (x,y) and turn it off', page: 'led-blink', category: 'LED Blink' },
    { method: 'ledNoBlink(index)', description: 'Stop blinking LED by linear index', page: 'led-blink', category: 'LED Blink' },
    { method: 'ledNoBlink()', description: 'Stop all LEDs from blinking', page: 'led-blink', category: 'LED Blink' },
    { method: 'ledUpdate()', description: 'Process blink timers — call every loop', page: 'led-blink', category: 'LED Blink' },
    
    // Animation Mode
    { method: 'begin()', description: 'Initialize the LED matrix for TinyScreen', page: 'animation-mode', category: 'Animation Mode' },
    { method: 'play(animation, mode)', description: 'Start playing an animation', page: 'animation-mode', category: 'Animation Mode' },
    { method: 'play(animation, mode, start, end)', description: 'Play animation with frame range', page: 'animation-mode', category: 'Animation Mode' },
    { method: 'pause()', description: 'Pause current animation', page: 'animation-mode', category: 'Animation Mode' },
    { method: 'resume()', description: 'Resume paused animation', page: 'animation-mode', category: 'Animation Mode' },
    { method: 'stop()', description: 'Stop animation and clear display', page: 'animation-mode', category: 'Animation Mode' },
    { method: 'update()', description: 'Update animation frame (call every loop)', page: 'animation-mode', category: 'Animation Mode' },
    { method: 'setSpeed(speedMs | speedMultiplier)', description: 'Set ms per frame or a multiplier (negative = reverse)', page: 'animation-mode', category: 'Animation Mode' },
    { method: 'restoreOriginalSpeed()', description: 'Reset to original timing', page: 'animation-mode', category: 'Animation Mode' },
    { method: 'getCurrentSpeed()', description: 'Get current speed in ms per frame', page: 'animation-mode', category: 'Animation Mode' },
    { method: 'isPlaying()', description: 'Check if animation is playing', page: 'animation-mode', category: 'Animation Mode' },
    { method: 'isPaused()', description: 'Check if animation is paused', page: 'animation-mode', category: 'Animation Mode' },
    { method: 'isComplete()', description: 'Check if ONCE mode finished', page: 'animation-mode', category: 'Animation Mode' },
    { method: 'getCurrentFrame()', description: 'Get current frame number', page: 'animation-mode', category: 'Animation Mode' },
    { method: 'getTotalFrames()', description: 'Get total number of frames', page: 'animation-mode', category: 'Animation Mode' },
    { method: 'setInvert(bool)', description: 'Invert all pixels on display', page: 'animation-mode', category: 'Animation Mode' },
    { method: 'getInvert()', description: 'Check if display is inverted', page: 'animation-mode', category: 'Animation Mode' },
    { method: 'addLayer()', description: 'Add a new animation layer', page: 'animation-mode', category: 'Animation Mode' },
    { method: 'playOnLayer(layer, anim, mode)', description: 'Play animation on specific layer', page: 'animation-mode', category: 'Animation Mode' },
    { method: 'setSpeedOnLayer(layer, speedMs | speedMultiplier)', description: 'Set ms per frame or a multiplier for a layer', page: 'animation-mode', category: 'Animation Mode' },
    { method: 'pauseLayer(layer)', description: 'Pause specific layer', page: 'animation-mode', category: 'Animation Mode' },
    { method: 'resumeLayer(layer)', description: 'Resume specific layer', page: 'animation-mode', category: 'Animation Mode' },
    { method: 'stopLayer(layer)', description: 'Stop specific layer', page: 'animation-mode', category: 'Animation Mode' },
    { method: 'setPosition(x, y)', description: 'Set x,y position offset for primary animation', page: 'animation-mode', category: 'Animation Mode' },
    { method: 'setPositionOnLayer(layer, x, y)', description: 'Set position offset for specific layer', page: 'animation-mode', category: 'Animation Mode' },
    { method: 'getPositionX()', description: 'Get current X position offset', page: 'animation-mode', category: 'Animation Mode' },
    { method: 'getPositionY()', description: 'Get current Y position offset', page: 'animation-mode', category: 'Animation Mode' },
    { method: 'getPositionXOnLayer(layer)', description: 'Get X position of specific layer', page: 'animation-mode', category: 'Animation Mode' },
    { method: 'getPositionYOnLayer(layer)', description: 'Get Y position of specific layer', page: 'animation-mode', category: 'Animation Mode' },
    
    // Canvas Mode
    { method: 'beginDraw()', description: 'Begin a drawing operation', page: 'canvas-mode', category: 'Canvas Mode' },
    { method: 'endDraw()', description: 'End drawing and display result', page: 'canvas-mode', category: 'Canvas Mode' },
    { method: 'background(state)', description: 'Set background state (ON/OFF) - use instead of clear()', page: 'canvas-mode', category: 'Canvas Mode' },
    { method: 'stroke(state)', description: 'Set outline state for shapes', page: 'canvas-mode', category: 'Canvas Mode' },
    { method: 'noStroke()', description: 'Disable outlines', page: 'canvas-mode', category: 'Canvas Mode' },
    { method: 'fill(state)', description: 'Set fill state for shapes', page: 'canvas-mode', category: 'Canvas Mode' },
    { method: 'noFill()', description: 'Disable fill', page: 'canvas-mode', category: 'Canvas Mode' },
    { method: 'point(x, y)', description: 'Draw a single point', page: 'canvas-mode', category: 'Canvas Mode' },
    { method: 'line(x1, y1, x2, y2)', description: 'Draw a line between two points', page: 'canvas-mode', category: 'Canvas Mode' },
    { method: 'rect(x, y, width, height)', description: 'Draw a rectangle', page: 'canvas-mode', category: 'Canvas Mode' },
    { method: 'circle(cx, cy, diameter)', description: 'Draw a circle', page: 'canvas-mode', category: 'Canvas Mode' },
    { method: 'ellipse(cx, cy, width, height)', description: 'Draw an ellipse', page: 'canvas-mode', category: 'Canvas Mode' },
    { method: 'text(string, x, y)', description: 'Draw text at position', page: 'canvas-mode', category: 'Canvas Mode' },
    { method: 'setTextSize(size)', description: 'Set text size (1 or 2)', page: 'canvas-mode', category: 'Canvas Mode' },
    { method: 'scrollText(string, y, direction)', description: 'Draw scrolling text', page: 'canvas-mode', category: 'Canvas Mode' },
    { method: 'setScrollSpeed(ms)', description: 'Set scroll speed in milliseconds', page: 'canvas-mode', category: 'Canvas Mode' },
    { method: 'resetScroll()', description: 'Reset scroll position to start', page: 'canvas-mode', category: 'Canvas Mode' },
    { method: 'setRotation(degrees)', description: 'Set display rotation (0, 90, 180, 270)', page: 'canvas-mode', category: 'Canvas Mode' },
    { method: 'getRotation()', description: 'Get current rotation in degrees', page: 'canvas-mode', category: 'Canvas Mode' },
    { method: 'setInvert(bool)', description: 'Invert all pixels on display', page: 'canvas-mode', category: 'Canvas Mode' },
    { method: 'getInvert()', description: 'Check if display is inverted', page: 'canvas-mode', category: 'Canvas Mode' },
    { method: 'width()', description: 'Get matrix width (12)', page: 'canvas-mode', category: 'Canvas Mode' },
    { method: 'height()', description: 'Get matrix height (8)', page: 'canvas-mode', category: 'Canvas Mode' },
    
    // Hybrid Mode
    { method: 'beginOverlay()', description: 'Begin overlay drawing (preserves animation)', page: 'hybrid-mode', category: 'Hybrid Mode' },
    { method: 'endOverlay()', description: 'End overlay and display combined result', page: 'hybrid-mode', category: 'Hybrid Mode' },
    
    // Canvas Animation Utilities
    { method: 'Ease(start)', description: 'Create easing object with start value', page: 'example-ease-demo', category: 'Canvas Animation' },
    { method: 'ease.to(target, duration)', description: 'Animate to target over duration ms', page: 'example-ease-demo', category: 'Canvas Animation' },
    { method: 'ease.intValue()', description: 'Get current value as integer', page: 'example-ease-demo', category: 'Canvas Animation' },
    { method: 'ease.done()', description: 'Check if animation is complete', page: 'example-ease-demo', category: 'Canvas Animation' },
    { method: 'ease.target()', description: 'Get the target value', page: 'example-ease-demo', category: 'Canvas Animation' },
    { method: 'oscillateInt(min, max, period, offset)', description: 'Get oscillating integer value with optional phase offset', page: 'example-oscillator-demo', category: 'Canvas Animation' },
    { method: 'oscillate(min, max, period, offset)', description: 'Get oscillating float value with optional phase offset', page: 'example-oscillator-demo', category: 'Canvas Animation' },
    
    // Constants
    { method: 'LOOP', description: 'Play animation continuously', page: 'animation-mode', category: 'Constants' },
    { method: 'ONCE', description: 'Play animation once and stop', page: 'animation-mode', category: 'Constants' },
    { method: 'BOOMERANG', description: 'Play animation forward then backward', page: 'animation-mode', category: 'Constants' },
    { method: 'ON', description: 'LED/pixel on state', page: 'canvas-mode', category: 'Constants' },
    { method: 'OFF', description: 'LED/pixel off state', page: 'canvas-mode', category: 'Constants' },
    { method: 'HIGH', description: 'LED on (Simple LED mode)', page: 'simple-led', category: 'Constants' },
    { method: 'LOW', description: 'LED off (Simple LED mode)', page: 'simple-led', category: 'Constants' },
];

function searchAPI(query) {
    return searchIndex.filter(item => 
        item.method.toLowerCase().includes(query) ||
        item.description.toLowerCase().includes(query) ||
        item.category.toLowerCase().includes(query)
    ).slice(0, 10); // Limit to 10 results
}

function displaySearchResults(results, query) {
    const searchResults = document.getElementById('search-results');
    
    if (results.length === 0)
    {
        searchResults.innerHTML = '<div class="search-no-results">No results found</div>';
        searchResults.classList.add('visible');
        return;
    }
    
    const html = results.map(item => {
        const highlightedMethod = item.method.replace(
            new RegExp(`(${query})`, 'gi'),
            '<mark>$1</mark>'
        );
        return `
            <a href="#" class="search-result" data-page="${item.page}">
                <span class="search-method">${highlightedMethod}</span>
                <span class="search-category">${item.category}</span>
                <span class="search-desc">${item.description}</span>
            </a>
        `;
    }).join('');
    
    searchResults.innerHTML = html;
    searchResults.classList.add('visible');
    
    // Add click handlers to results
    searchResults.querySelectorAll('.search-result').forEach(result => {
        result.addEventListener('click', (e) => {
            e.preventDefault();
            const page = result.dataset.page;
            loadPage(page);
            searchResults.classList.remove('visible');
            document.getElementById('search-input').value = '';
            
            // Close mobile menu
            document.getElementById('sidebar').classList.remove('open');
            document.getElementById('overlay').classList.remove('visible');
        });
    });
}

// Page content
const pages = {
    'home': `
        <h1>TinyFilmFestival <span class="version-badge">v2.5.0</span></h1>
        <p>A library for the Arduino UNO R4 WiFi's built-in 12×8 LED Matrix. One class, four modes.</p>

        <h2>Four Modes</h2>
        <div class="mode-grid">
            <a href="#" class="mode-card" data-page="simple-led">
                <h4>Simple LED</h4>
                <p>Control individual LEDs like digitalWrite()</p>
                <code>ledWrite(), ledToggle(), ledClear()</code>
            </a>
            <a href="#" class="mode-card" data-page="animation-mode">
                <h4>Animation Mode</h4>
                <p>Play pre-made frame animations</p>
                <code>play(), pause(), setSpeed()</code>
            </a>
            <a href="#" class="mode-card" data-page="canvas-mode">
                <h4>Canvas Mode</h4>
                <p>Draw with code in real-time</p>
                <code>beginDraw(), point(), line()</code>
            </a>
            <a href="#" class="mode-card" data-page="hybrid-mode">
                <h4>Hybrid Mode</h4>
                <p>Draw over animations</p>
                <code>beginOverlay(), endOverlay()</code>
            </a>
        </div>

        <h2>Mixing Modes</h2>
        <p><strong>Animation, Canvas, and Hybrid modes can be combined</strong> in the same sketch — they all use the same <code>TinyScreen</code> object:</p>
        <ul>
            <li>Play an animation, then switch to canvas drawing</li>
            <li>Use <code>beginOverlay()</code> to draw on top of animations (Hybrid)</li>
            <li>Mix <code>oscillateInt()</code> and <code>Ease</code> in any mode</li>
        </ul>
        <div class="info-box warning">
            <strong>Simple LED is standalone</strong> — it uses a separate internal display buffer, so don't mix <code>ledWrite()</code> with the other modes in the same sketch.
        </div>

        <h2>LED Matrix Layout</h2>
        <p>The R4 WiFi has a 12×8 LED matrix (96 LEDs total). Use (x, y) coordinates or linear index:</p>
        <div class="matrix-grid">     x=0  x=1  x=2  x=3  x=4  x=5  x=6  x=7  x=8  x=9  x=10 x=11
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
    +----+----+----+----+----+----+----+----+----+----+----+----+</div>
        <p>Numbers inside = linear index for <code>ledWrite(index, state)</code></p>

        <h2>Getting Started</h2>
        <p>Install the library, include it in your sketch, and start drawing:</p>
        <pre><code class="language-cpp">#include "TinyFilmFestival.h"

TinyScreen screen;

void setup()
{
    screen.begin();
}

void loop()
{
    screen.beginDraw();
    screen.background(OFF);
    screen.stroke(ON);
    
    // Draw a bouncing dot
    int x = oscillateInt(0, 11, 2000);
    int y = oscillateInt(0, 7, 1500);
    screen.point(x, y);
    
    screen.endDraw();
}</code></pre>
    `,

    'installation': `
        <h1>Installation</h1>
        
        <h2>Arduino IDE Library Manager</h2>
        <ol>
            <li>Open Arduino IDE</li>
            <li>Go to <strong>Sketch → Include Library → Manage Libraries...</strong></li>
            <li>Search for "TinyFilmFestival"</li>
            <li>Click <strong>Install</strong></li>
            <li>When prompted, select <strong>"Install all"</strong> to include all required dependencies</li>
        </ol>

        <div class="info-box note">
            <strong>Important</strong>
            When the dependency dialog appears, make sure to choose "Install all" (or "Install with dependencies"). This ensures ArduinoGraphics and other required libraries are installed automatically.
        </div>

        <h2>Manual Installation</h2>
        <ol>
            <li>Download the library from <a href="https://github.com/DigitalFuturesOCADU/TinyFilmFestival" target="_blank">GitHub</a></li>
            <li>Extract the ZIP file</li>
            <li>Copy the folder to your Arduino libraries directory:
                <ul>
                    <li><strong>macOS:</strong> ~/Documents/Arduino/libraries/</li>
                    <li><strong>Windows:</strong> Documents\\Arduino\\libraries\\</li>
                    <li><strong>Linux:</strong> ~/Arduino/libraries/</li>
                </ul>
            </li>
            <li>Restart Arduino IDE</li>
        </ol>

        <h2>Requirements</h2>
        <table>
            <tr>
                <th>Requirement</th>
                <th>Details</th>
            </tr>
            <tr>
                <td>Hardware</td>
                <td>Arduino UNO R4 WiFi</td>
            </tr>
            <tr>
                <td>Arduino IDE</td>
                <td>Version 2.0 or later</td>
            </tr>
            <tr>
                <td>Board Package</td>
                <td>Arduino UNO R4 Boards</td>
            </tr>
        </table>

        <div class="info-box note">
            <strong>Note</strong>
            The TinyFilmFestival library automatically includes ArduinoGraphics and Arduino_LED_Matrix, so you don't need to install or include them separately.
        </div>
    `,

    'quick-start': `
        <h1>Quick Start</h1>

        <h2>Simple LED Mode</h2>
        <p>Control individual LEDs like <code>digitalWrite()</code> — the simplest way to get started.</p>
        <pre><code class="language-cpp">#include "TinyFilmFestival.h"

unsigned long lastBlink = 0;
bool ledState = false;

void setup()
{
    ledBegin();                   // Initialize matrix
    
    ledWrite(0, 0, HIGH);         // Top-left ON
    ledWrite(11, 7, HIGH);        // Bottom-right ON
    ledToggle(0, 0);              // Toggle top-left OFF
}

void loop()
{
    // Non-blocking blink using millis()
    if (millis() - lastBlink >= 500)
    {
        lastBlink = millis();
        ledState = !ledState;
        ledWrite(5, 3, ledState ? HIGH : LOW);
    }
}</code></pre>

        <h2>Animation Mode</h2>
        <p>Play pre-made frame animations created in the LED Matrix Editor.</p>
        <pre><code class="language-cpp">#include "TinyFilmFestival.h"
#include "idle.h"               // Your animation file

TinyScreen screen;
Animation myAnim = idle;        // Variable name from .h file

void setup()
{
    screen.begin();
    screen.play(myAnim, LOOP);
}

void loop()
{
    screen.update();
}</code></pre>

        <h2>Canvas Mode</h2>
        <p>Draw graphics with code using Processing-style commands.</p>
        <pre><code class="language-cpp">#include "TinyFilmFestival.h"

TinyScreen screen;

void setup()
{
    screen.begin();
}

void loop()
{
    screen.beginDraw();
    screen.background(OFF);       // Clear to black
    screen.stroke(ON);
    screen.point(5, 3);           // Draw a point
    screen.line(0, 0, 11, 7);     // Draw a line
    screen.endDraw();
}</code></pre>

        <h2>Hybrid Mode</h2>
        <p>Draw over animations — the best of both worlds.</p>
        <pre><code class="language-cpp">#include "TinyFilmFestival.h"
#include "myAnimation.h"

TinyScreen screen;
Animation anim = animation;

void setup()
{
    screen.begin();
    screen.play(anim, LOOP);
}

void loop()
{
    screen.update();
    
    screen.beginOverlay();
    screen.stroke(ON);
    screen.point(11, 0);          // Status indicator
    screen.endOverlay();
}</code></pre>
    `,

    'editor-guide': `
        <h1>LED Matrix Editor</h1>
        <p>The <a href="https://ledmatrix-editor.arduino.cc/" target="_blank">Arduino LED Matrix Editor</a> provides a visual interface for creating LED matrix animations.</p>

        <h2>Overview</h2>
        <p>The editor allows you to:</p>
        <ul>
            <li>Create animations by painting/erasing pixels frame by frame</li>
            <li>Copy frames for easy transitions</li>
            <li>Move pixels as a group within a frame</li>
            <li>Set display time for each frame</li>
        </ul>

        <h2>Interface</h2>
        <img src="images/matrixEditor.png" alt="LED Matrix Editor Interface" class="doc-image">

        <h2>Keyboard Shortcuts</h2>
        
        <h3>Project Controls</h3>
        <table>
            <tr><th>Action</th><th>Shortcut</th></tr>
            <tr><td>Start/Stop preview</td><td>Spacebar</td></tr>
            <tr><td>Preview with loop</td><td>⇧ + Spacebar</td></tr>
            <tr><td>Upload project</td><td>Ctrl + U</td></tr>
            <tr><td>Save project</td><td>Ctrl + S</td></tr>
            <tr><td>Export code</td><td>Ctrl + E</td></tr>
        </table>

        <h3>Canvas Controls</h3>
        <table>
            <tr><th>Action</th><th>Shortcut</th></tr>
            <tr><td>Brush</td><td>B</td></tr>
            <tr><td>Eraser</td><td>E</td></tr>
            <tr><td>Clear frame</td><td>⌘ + ⌫</td></tr>
            <tr><td>Move pixels</td><td>⇧ + Arrow keys</td></tr>
            <tr><td>Move with wrap</td><td>⇧ + ⌘ + Arrow keys</td></tr>
        </table>

        <h3>Timeline Controls</h3>
        <table>
            <tr><th>Action</th><th>Shortcut</th></tr>
            <tr><td>New frame</td><td>Ctrl + N</td></tr>
            <tr><td>Duplicate frame</td><td>Ctrl + D</td></tr>
            <tr><td>Delete frame</td><td>Ctrl + ⌫</td></tr>
            <tr><td>Delete all frames</td><td>Ctrl + ⇧ + ⌫</td></tr>
            <tr><td>Move between frames</td><td>Arrow keys</td></tr>
        </table>

        <h2>File Types</h2>
        <p>The editor is a web-based tool, so your work is not automatically saved. Use these controls to manage your files:</p>
        
        <img src="images/downloadControls.png" alt="Download Controls" class="doc-image">
        
        <table>
            <tr><th>Icon</th><th>File Type</th><th>Description</th></tr>
            <tr>
                <td><img src="images/mpjDownload.png" alt="Save MPJ" class="icon-inline"></td>
                <td><strong>.mpj</strong></td>
                <td>Save working project file (preserves your session)</td>
            </tr>
            <tr>
                <td><img src="images/mpjUpload.png" alt="Load MPJ" class="icon-inline"></td>
                <td><strong>.mpj</strong></td>
                <td>Restore a previous session by uploading</td>
            </tr>
            <tr>
                <td><img src="images/hDownload.png" alt="Export H" class="icon-inline"></td>
                <td><strong>.h</strong></td>
                <td>Export animation as Arduino code</td>
            </tr>
        </table>

        <h2>Using Animations in Arduino</h2>
        <p>Follow these steps to use your animation in an Arduino sketch:</p>

        <h3>Step 1: Export Your Animation</h3>
        <p>Click the <img src="images/hDownload.png" alt="Export" class="icon-inline"> button to download your animation as a <code>.h</code> file. Choose a descriptive name for the file.</p>
        
        <div class="info-box warning">
            <strong>Important</strong>
            Do not include spaces in the file name when exporting.
        </div>

        <img src="images/downloadSteps.png" alt="Download Steps" class="doc-image">

        <h3>Step 2: Add to Your Sketch</h3>
        <ol>
            <li>Locate the folder containing your Arduino sketch (<code>.ino</code> file)</li>
            <li>Copy the <code>.h</code> file you downloaded into that folder</li>
            <li>Include it in your sketch: <code>#include "yourAnimation.h"</code></li>
            <li>Create an Animation variable using the name from inside the file</li>
            <li>Play it with <code>screen.play()</code></li>
        </ol>

        <h3>Understanding Variable Names</h3>
        <p>When you export an animation, the <code>.h</code> file contains a variable with a specific name. You need to reference this name in your code:</p>
        
        <img src="images/variableNames.png" alt="Variable Names" class="doc-image">

        <pre><code class="language-cpp">#include "TinyFilmFestival.h"
#include "myAnimation.h"    // Your exported file

TinyScreen screen;
Animation myAnim = animation;   // 'animation' is the variable name in the .h file

void setup()
{
    screen.begin();
    screen.play(myAnim, LOOP);
}

void loop()
{
    screen.update();
}</code></pre>

        <div class="info-box note">
            <strong>Tip</strong>
            The variable name inside the <code>.h</code> file is typically <code>animation</code>, but check your exported file to confirm.
        </div>

        <h2>Example Animations</h2>
        <p>The library includes several <a href="https://github.com/DigitalFuturesOCADU/TinyFilmFestival/tree/main/exampleAnimations" target="_blank">example animations</a> you can use to get started.</p>

        <h2>Converting to GIF</h2>
        <p>For documentation purposes, you can convert <code>.h</code> files to animated GIFs:</p>
        <ol>
            <li>Go to the <a href="https://npuckett.github.io/gifBuilder/" target="_blank">GIF Builder</a></li>
            <li>Click "Choose File" and select your <code>.h</code> file</li>
            <li>Click "Save GIF" to download the animated GIF</li>
        </ol>
    `,

    'simple-led': `
        <h1>Simple LED Mode</h1>
        <p>Control individual LEDs like <code>digitalWrite()</code> — the simplest way to get started with the LED matrix.</p>

        <h2>Setup</h2>
        <pre><code class="language-cpp">#include "TinyFilmFestival.h"

void setup()
{
    ledBegin();    // Initialize the matrix
}</code></pre>

        <h2>Functions</h2>

        <div class="api-method">
            <h3>ledBegin()</h3>
            <p>Initialize the LED matrix. Call once in <code>setup()</code>.</p>
            <pre><code class="language-cpp">void setup()
            {
    ledBegin();
}</code></pre>
        </div>

        <div class="api-method">
            <h3>ledWrite(x, y, state)</h3>
            <p>Set an LED at coordinates (x, y) to HIGH or LOW.</p>
            <table>
                <tr><th>Parameter</th><th>Type</th><th>Description</th></tr>
                <tr><td>x</td><td>int</td><td>X coordinate (0-11)</td></tr>
                <tr><td>y</td><td>int</td><td>Y coordinate (0-7)</td></tr>
                <tr><td>state</td><td>int</td><td>HIGH or LOW</td></tr>
            </table>
            <pre><code class="language-cpp">ledWrite(0, 0, HIGH);    // Top-left ON
ledWrite(5, 3, LOW);     // Center OFF</code></pre>
        </div>

        <div class="api-method">
            <h3>ledWrite(index, state)</h3>
            <p>Set an LED by linear index (0-95).</p>
            <pre><code class="language-cpp">ledWrite(0, HIGH);       // First LED ON
ledWrite(95, HIGH);      // Last LED ON</code></pre>
        </div>

        <div class="api-method">
            <h3>ledRead(x, y)</h3>
            <p>Read the current state of an LED.</p>
            <p><strong>Returns:</strong> <code>true</code> (on) or <code>false</code> (off)</p>
            <pre><code class="language-cpp">bool state = ledRead(0, 0);
if (state)
{
    // LED is on
}</code></pre>
        </div>

        <div class="api-method">
            <h3>ledToggle(x, y)</h3>
            <p>Toggle an LED between ON and OFF.</p>
            <pre><code class="language-cpp">ledToggle(5, 3);    // If on, turn off. If off, turn on.</code></pre>
        </div>

        <div class="api-method">
            <h3>ledClear()</h3>
            <p>Turn off all LEDs.</p>
            <pre><code class="language-cpp">ledClear();    // All LEDs off</code></pre>
        </div>

        <div class="api-method">
            <h3>setAutoShow(enabled)</h3>
            <p>Enable or disable automatic display updates after LED writes when using a <code>TinyScreen</code> object.</p>
            <pre><code class="language-cpp">TinyScreen screen;
screen.begin();

screen.setAutoShow(false);  // Batch multiple LED writes
screen.led(0, 0, true);
screen.led(1, 0, true);
screen.show();              // Push once</code></pre>
        </div>
    `,

    'led-blink': `
        <h1>LED Blink</h1>
        <p>Non-blocking per-LED blink control. Each LED can blink at its own independent rate while other LEDs remain static or blink at different rates.</p>

        <div class="info-box note">
            <strong>Requires ledUpdate()</strong>
            Unlike the other Simple LED functions, blink requires <code>ledUpdate()</code> to be called every <code>loop()</code> to process the blink timers.
        </div>

        <h2>Setup</h2>
        <pre><code class="language-cpp">#include "TinyFilmFestival.h"

void setup()
{
    ledBegin();
    ledBlink(5, 3, 500);    // Blink center LED every 500ms
}

void loop()
{
    ledUpdate();             // REQUIRED — drives all blink timers
}</code></pre>

        <h2>Functions</h2>

        <div class="api-method">
            <h3>ledBlink(x, y, rateMs)</h3>
            <p>Start blinking an LED at coordinates (x, y). The LED toggles ON/OFF every <code>rateMs</code> milliseconds. Non-blocking — all LEDs run on independent timers.</p>
            <table>
                <tr><th>Parameter</th><th>Type</th><th>Description</th></tr>
                <tr><td>x</td><td>int</td><td>X coordinate (0-11)</td></tr>
                <tr><td>y</td><td>int</td><td>Y coordinate (0-7)</td></tr>
                <tr><td>rateMs</td><td>unsigned long</td><td>Toggle interval in milliseconds</td></tr>
            </table>
            <pre><code class="language-cpp">ledBlink(0, 0, 200);     // Fast blink (200ms)
ledBlink(5, 3, 1000);    // Slow blink (1 second)</code></pre>
        </div>

        <div class="api-method">
            <h3>ledBlink(index, rateMs)</h3>
            <p>Start blinking an LED by linear index (0-95).</p>
            <pre><code class="language-cpp">ledBlink(0, 250);        // First LED, 250ms blink
ledBlink(95, 750);       // Last LED, 750ms blink</code></pre>
        </div>

        <div class="api-method">
            <h3>ledNoBlink(x, y)</h3>
            <p>Stop blinking a specific LED. The LED is turned OFF.</p>
            <pre><code class="language-cpp">ledNoBlink(0, 0);    // Stop blinking, turn LED off</code></pre>
        </div>

        <div class="api-method">
            <h3>ledNoBlink(index)</h3>
            <p>Stop blinking an LED by linear index. The LED is turned OFF.</p>
            <pre><code class="language-cpp">ledNoBlink(41);      // Stop blinking LED 41</code></pre>
        </div>

        <div class="api-method">
            <h3>ledNoBlink()</h3>
            <p>Stop <strong>all</strong> blinking. Non-blinking LEDs that were set with <code>ledWrite()</code> keep their current state.</p>
            <pre><code class="language-cpp">ledNoBlink();        // Cancel all blinks (static LEDs unaffected)</code></pre>
        </div>

        <div class="api-method">
            <h3>ledUpdate()</h3>
            <p>Process all blink timers. <strong>Must be called every <code>loop()</code>.</strong> Only writes to the display when a blink state actually changes, so it is very efficient.</p>
            <pre><code class="language-cpp">void loop()
{
    ledUpdate();
}</code></pre>
        </div>

        <div class="info-box note">
            <strong>Auto-show</strong>
            If you are using a <code>TinyScreen</code> object with <code>setAutoShow(false)</code>, call <code>screen.show()</code> after <code>screen.updateBlinks()</code> to flush changes.
        </div>

        <h2>Mixing with Other Simple LED Functions</h2>
        <p>Blink works alongside all existing Simple LED functions. They share the same LED buffer:</p>
        <ul>
            <li><code>ledWrite()</code> or <code>ledToggle()</code> on a blinking LED <strong>cancels its blink</strong> — the LED becomes static at the value you set.</li>
            <li><code>ledBlink()</code> on a static LED starts it blinking without affecting other LEDs.</li>
            <li><code>ledRead()</code> returns the LED's current instantaneous state (ON or OFF), even mid-blink.</li>
            <li><code>ledClear()</code> stops all blinks and turns everything off.</li>
        </ul>
        <pre><code class="language-cpp">void setup()
{
    ledBegin();

    ledWrite(0, 0, HIGH);        // Static ON
    ledBlink(5, 3, 500);         // Blinking at 500ms
    ledBlink(11, 7, 1000);       // Blinking at 1000ms

    // Later...
    ledWrite(5, 3, HIGH);        // Cancels blink — now static ON
    ledToggle(11, 7);            // Cancels blink — now static (toggled)
}</code></pre>

        <h2>TinyScreen Object Methods</h2>
        <p>If you are using the <code>TinyScreen</code> object directly, the equivalent methods are available:</p>
        <table>
            <tr><th>Standalone Function</th><th>TinyScreen Method</th></tr>
            <tr><td><code>ledBlink(x, y, rate)</code></td><td><code>screen.blink(x, y, rate)</code></td></tr>
            <tr><td><code>ledNoBlink(x, y)</code></td><td><code>screen.noBlink(x, y)</code></td></tr>
            <tr><td><code>ledNoBlink()</code></td><td><code>screen.noBlink()</code></td></tr>
            <tr><td><code>ledUpdate()</code></td><td><code>screen.updateBlinks()</code></td></tr>
        </table>
    `,

    'animation-mode': `
        <h1>Animation Mode API</h1>
        <p>Play pre-made frame-by-frame animations created with the LED Matrix Editor.</p>

        <div class="gif-grid">
            <img src="../images/character.gif" alt="Character animation">
            <img src="../images/landscape.gif" alt="Landscape animation">
            <img src="../images/load.gif" alt="Loading animation">
            <img src="../images/pow.gif" alt="Pow animation">
        </div>

        <div class="info-box tip">
            <strong>Dynamic Motion</strong>
            Animations aren't stuck in place — use <code>setPosition(x, y)</code> with <code>oscillateInt()</code> for continuous cyclic motion, or <code>Ease.to()</code> for smooth one-shot transitions to a target position. See <a href="#" onclick="navigateTo('animation-mode'); setTimeout(() => document.getElementById('animation-positioning')?.scrollIntoView({behavior:'smooth'}), 100); return false;">Animation Positioning</a> below.
        </div>

        <h2>Overview</h2>
        <p>Animation Mode is like a video player for the LED matrix:</p>
        <ul>
            <li><strong>Load</strong> animation frames from <code>.h</code> files</li>
            <li><strong>Play</strong> with different modes (loop, once, boomerang)</li>
            <li><strong>Control</strong> speed, pause, resume, and playback position</li>
            <li><strong>Position</strong> animations dynamically — slide, oscillate, or map to sensor input</li>
            <li><strong>Layer</strong> multiple animations on top of each other</li>
        </ul>

        <h2>Setup</h2>
        <pre><code class="language-cpp">#include "TinyFilmFestival.h"
#include "myAnimation.h"       // From LED Matrix Editor

TinyScreen screen;
Animation anim = animation;    // 'animation' is defined in .h file

void setup()
{
    screen.begin();
    screen.play(anim, LOOP);
}

void loop()
{
    screen.update();           // REQUIRED - call every loop!
}</code></pre>

        <div class="info-box note">
            <strong>Important</strong>
            You must call <code>screen.update()</code> in your <code>loop()</code> function. This advances the animation frames.
        </div>

        <h2>Playback Methods</h2>

        <div class="api-method">
            <h3>play(animation, mode)</h3>
            <p>Starts playing an animation with the specified mode.</p>
            <table>
                <tr><th>Parameter</th><th>Type</th><th>Description</th></tr>
                <tr><td>animation</td><td>Animation</td><td>Animation object from .h file</td></tr>
                <tr><td>mode</td><td>PlayMode</td><td>LOOP, ONCE, or BOOMERANG</td></tr>
                <tr><td>startFrame</td><td>int</td><td>Optional: first frame (1-indexed)</td></tr>
                <tr><td>endFrame</td><td>int</td><td>Optional: last frame (1-indexed)</td></tr>
            </table>
            <pre><code class="language-cpp">screen.play(myAnim, LOOP);
screen.play(myAnim, ONCE, 2, 6);  // Play frames 2-6 once</code></pre>
        </div>

        <div class="api-method">
            <h3>pause()</h3>
            <p>Pauses the current animation at its current frame.</p>
            <pre><code class="language-cpp">screen.pause();</code></pre>
        </div>

        <div class="api-method">
            <h3>resume()</h3>
            <p>Resumes a paused animation from where it stopped.</p>
            <pre><code class="language-cpp">screen.resume();</code></pre>
        </div>

        <div class="api-method">
            <h3>stop()</h3>
            <p>Stops the animation. The next <code>update()</code> clears the display.</p>
            <pre><code class="language-cpp">screen.stop();</code></pre>
        </div>

        <div class="api-method">
            <h3>update()</h3>
            <p>Updates the animation. <strong>Must be called every loop.</strong></p>
            <pre><code class="language-cpp">void loop()
            {
    screen.update();
}</code></pre>
        </div>

        <h2>Play Modes</h2>
        <table>
            <tr><th>Mode</th><th>Description</th></tr>
            <tr><td><code>LOOP</code></td><td>Play continuously, restarting when finished</td></tr>
            <tr><td><code>ONCE</code></td><td>Play once and stop on the last frame</td></tr>
            <tr><td><code>BOOMERANG</code></td><td>Play forward, then backward (ping-pong)</td></tr>
        </table>

        <h2>Speed Control</h2>

        <div class="api-method">
            <h3>setSpeed(speedMs | speedMultiplier)</h3>
            <p>Set playback speed using either milliseconds per frame or a multiplier. Negative values play backward. Use 0 to pause.</p>
            <pre><code class="language-cpp">// Milliseconds per frame
    screen.setSpeed(50);    // Very fast: 50ms per frame
    screen.setSpeed(500);   // Slow: half second per frame
    screen.setSpeed(-100);  // Backward: 100ms per frame

    // Multipliers (1.0 = original speed)
    screen.setSpeed(0.5);   // Half speed
    screen.setSpeed(2.0);   // Double speed</code></pre>
        </div>

        <div class="api-method">
            <h3>restoreOriginalSpeed()</h3>
            <p>Reset to the original timing from the animation file.</p>
            <pre><code class="language-cpp">screen.restoreOriginalSpeed();</code></pre>
        </div>

        <div class="api-method">
            <h3>getCurrentSpeed()</h3>
            <p>Get the current speed in milliseconds per frame.</p>
            <pre><code class="language-cpp">int speedMs = screen.getCurrentSpeed();</code></pre>
        </div>

        <h2>Status Methods</h2>
        <table>
            <tr><th>Method</th><th>Returns</th><th>Description</th></tr>
            <tr><td><code>isPlaying()</code></td><td>bool</td><td>True if animation is playing</td></tr>
            <tr><td><code>isPaused()</code></td><td>bool</td><td>True if animation is paused</td></tr>
            <tr><td><code>isComplete()</code></td><td>bool</td><td>True if ONCE mode finished</td></tr>
            <tr><td><code>getCurrentFrame()</code></td><td>int</td><td>Current frame number (1-indexed)</td></tr>
            <tr><td><code>getTotalFrames()</code></td><td>int</td><td>Total number of frames</td></tr>
        </table>

        <h2>Layered Animations</h2>
        <p>Stack multiple animations on top of each other.</p>
        <pre><code class="language-cpp">screen.begin();
screen.addLayer();                        // Add a second layer

screen.play(backgroundAnim, LOOP);        // Layer 0 (background)
screen.playOnLayer(1, foregroundAnim, LOOP);  // Layer 1 (foreground)</code></pre>

        <h3>Layer Methods</h3>
        <table>
            <tr><th>Method</th><th>Description</th></tr>
            <tr><td><code>addLayer()</code></td><td>Add a new animation layer</td></tr>
            <tr><td><code>playOnLayer(layer, anim, mode)</code></td><td>Play animation on specific layer</td></tr>
            <tr><td><code>setSpeedOnLayer(layer, speedMs | speedMultiplier)</code></td><td>Set ms per frame or multiplier (negative = reverse)</td></tr>
            <tr><td><code>pauseLayer(layer)</code></td><td>Pause specific layer</td></tr>
            <tr><td><code>resumeLayer(layer)</code></td><td>Resume specific layer</td></tr>
            <tr><td><code>stopLayer(layer)</code></td><td>Stop specific layer</td></tr>
        </table>

        <h2>Display Transform</h2>
        
        <div class="api-method">
            <h3>setInvert(bool)</h3>
            <p>Invert all pixels on the display (swap ON and OFF). Works in Animation Mode.</p>
            <pre><code class="language-cpp">screen.setInvert(true);    // Inverted display
screen.setInvert(false);   // Normal display</code></pre>
        </div>

        <div class="api-method">
            <h3>getInvert()</h3>
            <p>Check if display is currently inverted.</p>
            <pre><code class="language-cpp">bool inverted = screen.getInvert();</code></pre>
        </div>

        <h2 id="animation-positioning">Animation Positioning</h2>
        <p>Dynamically position an animation clip anywhere on the display. Pixels that move beyond the 12×8 matrix edges are automatically clipped, and areas with no animation data remain off.</p>
        <p>This is perfect for:</p>
        <ul>
            <li><strong>Moving characters</strong> — animate a small sprite, position it dynamically</li>
            <li><strong>Scrolling effects</strong> — slide animations on/off screen</li>
            <li><strong>Sensor-driven placement</strong> — map sensor input to animation position</li>
            <li><strong>Layered scenes</strong> — position foreground characters over background animations</li>
        </ul>

        <div class="api-method">
            <h3>setPosition(x, y)</h3>
            <p>Set the x,y position offset for the primary animation layer. The animation is shifted by the given amount — positive x moves right, positive y moves down. Negative values move left/up, clipping at the edge.</p>
            <table>
                <tr><th>Parameter</th><th>Type</th><th>Description</th></tr>
                <tr><td>x</td><td>int</td><td>Horizontal offset (positive = right, negative = left)</td></tr>
                <tr><td>y</td><td>int</td><td>Vertical offset (positive = down, negative = up)</td></tr>
            </table>
            <pre><code class="language-cpp">screen.setPosition(3, 2);    // Shift animation right 3, down 2
screen.setPosition(-2, 0);   // Shift left 2 (left edge is clipped)
screen.setPosition(0, 0);    // Reset to default position</code></pre>
        </div>

        <div class="api-method">
            <h3>setPositionOnLayer(layer, x, y)</h3>
            <p>Set position offset for a specific animation layer.</p>
            <table>
                <tr><th>Parameter</th><th>Type</th><th>Description</th></tr>
                <tr><td>layer</td><td>int</td><td>Layer index</td></tr>
                <tr><td>x</td><td>int</td><td>Horizontal offset</td></tr>
                <tr><td>y</td><td>int</td><td>Vertical offset</td></tr>
            </table>
            <pre><code class="language-cpp">screen.setPositionOnLayer(1, 5, 0);  // Move layer 1 right by 5</code></pre>
        </div>

        <div class="api-method">
            <h3>getPositionX() / getPositionY()</h3>
            <p>Get the current position offset of the primary layer.</p>
            <pre><code class="language-cpp">int x = screen.getPositionX();
int y = screen.getPositionY();</code></pre>
        </div>

        <div class="api-method">
            <h3>getPositionXOnLayer(layer) / getPositionYOnLayer(layer)</h3>
            <p>Get the current position offset of a specific layer.</p>
            <pre><code class="language-cpp">int x = screen.getPositionXOnLayer(1);
int y = screen.getPositionYOnLayer(1);</code></pre>
        </div>

        <h3>Example: Oscillating Animation</h3>
        <p>Use <code>oscillateInt()</code> to smoothly slide an animation back and forth across the display with a sine-wave motion. No manual timing needed — just set the range and period.</p>
        <pre><code class="language-cpp">#include "TinyFilmFestival.h"
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
    // Smoothly oscillate X position back and forth (2-second cycle)
    int x = oscillateInt(-2, 6, 2000);

    screen.setPosition(x, 0);
    screen.update();
}</code></pre>

        <div class="info-box tip">
            <strong>Combine axes</strong>
            Add a second <code>oscillateInt()</code> for the Y axis. Use a different period or a phase offset to create diagonal motion:
            <pre><code class="language-cpp">int x = oscillateInt(-2, 6, 2000);
int y = oscillateInt(-1, 3, 1500, 0.25);  // phase-shifted
screen.setPosition(x, y);</code></pre>
        </div>

        <div class="info-box note">
            <strong>Position persists</strong>
            The position offset is a property of the layer slot, not the animation. It persists across <code>play()</code> and <code>stop()</code> calls until you change it.
        </div>
    `,

    'canvas-mode': `
        <h1>Canvas Mode API</h1>
        <p>Draw directly to the LED matrix using code. Generate graphics in real-time with a double-buffered drawing system.</p>

        <h2>Overview</h2>
        <p>Canvas Mode is perfect for:</p>
        <ul>
            <li><strong>Dynamic data</strong> — sensor readings, counters, progress bars</li>
            <li><strong>Procedural effects</strong> — sine waves, bouncing dots, patterns</li>
            <li><strong>Interactive graphics</strong> — responding to input in real-time</li>
        </ul>

        <div class="info-box note">
            <strong>Monochrome Matrix</strong>
            The LED matrix is on/off only — no color or brightness. Use <code>ON</code> and <code>OFF</code> constants.
        </div>

        <h2>Setup</h2>
        <pre><code class="language-cpp">#include "TinyFilmFestival.h"

TinyScreen screen;

void setup()
{
    screen.begin();
}

void loop()
{
    screen.beginDraw();
    screen.background(OFF);   // Clear to black
    screen.stroke(ON);
    // Draw something here
    screen.endDraw();
}</code></pre>

        <h2>Drawing Control</h2>

        <div class="api-method">
            <h3>beginDraw() / endDraw()</h3>
            <p>Begin and end a drawing operation. All drawing commands go between these calls.</p>
            <pre><code class="language-cpp">screen.beginDraw();
screen.background(OFF);
screen.stroke(ON);
screen.point(5, 3);
screen.endDraw();</code></pre>
        </div>

        <div class="api-method">
            <h3>background(state)</h3>
            <p>Fill the entire display with the given state. Use <code>background(OFF)</code> to clear the screen.</p>
            <pre><code class="language-cpp">screen.background(OFF);   // Dark background (clear)
screen.background(ON);    // Lit background (all LEDs on)</code></pre>
        </div>

        <h2>Style Control</h2>

        <div class="api-method">
            <h3>stroke(state) / noStroke()</h3>
            <p>Set outline state for shapes.</p>
            <pre><code class="language-cpp">screen.stroke(ON);    // Outlines will be lit
screen.stroke(OFF);   // Outlines will be dark (erase)
screen.noStroke();    // No outlines drawn</code></pre>
        </div>

        <div class="api-method">
            <h3>fill(state) / noFill()</h3>
            <p>Set fill state for shapes.</p>
            <pre><code class="language-cpp">screen.fill(ON);      // Shapes will be filled (lit)
screen.fill(OFF);     // Shapes will be filled (dark/erase)
screen.noFill();      // No fill (outline only)</code></pre>
        </div>

        <h2>Drawing Methods</h2>

        <div class="info-box note">
            <strong>Coordinate System</strong>
            Coordinates start at the top-left corner of the matrix. <code>(0, 0)</code> is the top-left pixel, <code>(11, 7)</code> is the bottom-right.
        </div>

        <div class="api-method">
            <h3>point(x, y)</h3>
            <p>Draw a single point.</p>
            <pre><code class="language-cpp">screen.point(5, 3);   // Point at center-ish</code></pre>
        </div>

        <div class="api-method">
            <h3>line(x1, y1, x2, y2)</h3>
            <p>Draw a line between two points.</p>
            <pre><code class="language-cpp">screen.line(0, 0, 11, 7);   // Diagonal line</code></pre>
        </div>

        <div class="api-method">
            <h3>rect(x, y, width, height)</h3>
            <p>Draw a rectangle.</p>
            <pre><code class="language-cpp">screen.rect(2, 1, 8, 6);    // Rectangle</code></pre>
        </div>

        <div class="api-method">
            <h3>circle(cx, cy, diameter)</h3>
            <p>Draw a circle with given diameter.</p>
            <pre><code class="language-cpp">screen.circle(5, 3, 4);     // Circle at center, diameter 4</code></pre>
        </div>

        <div class="api-method">
            <h3>ellipse(cx, cy, width, height)</h3>
            <p>Draw an ellipse.</p>
            <pre><code class="language-cpp">screen.ellipse(5, 3, 6, 4); // Ellipse</code></pre>
        </div>

        <h2>Text Methods</h2>

        <div class="api-method">
            <h3>text(string, x, y)</h3>
            <p>Draw text at position using built-in 3x5 font.</p>
            <pre><code class="language-cpp">screen.text("Hi", 0, 1);</code></pre>
        </div>

        <div class="api-method">
            <h3>setTextSize(size)</h3>
            <p>Set text size: 1 = normal (3x5), 2 = double (6x10).</p>
            <pre><code class="language-cpp">screen.setTextSize(1);   // Normal size
screen.setTextSize(2);   // Double size</code></pre>
        </div>

        <div class="api-method">
            <h3>scrollText(string, y, direction)</h3>
            <p>Draw scrolling text that automatically moves across the display.</p>
            <pre><code class="language-cpp">screen.scrollText("Hello World!", 0, SCROLL_LEFT);
screen.scrollText("Hello World!", 7, SCROLL_RIGHT);</code></pre>
        </div>

        <div class="api-method">
            <h3>setScrollSpeed(ms)</h3>
            <p>Set scroll speed in milliseconds per pixel.</p>
            <pre><code class="language-cpp">screen.setScrollSpeed(100);   // 100ms per pixel</code></pre>
        </div>

        <div class="api-method">
            <h3>resetScroll()</h3>
            <p>Reset scroll position back to the start.</p>
            <pre><code class="language-cpp">screen.resetScroll();</code></pre>
        </div>

        <h2>Display Transform</h2>

        <div class="api-method">
            <h3>setRotation(degrees)</h3>
            <p>Rotate the display output. Valid values: 0, 90, 180, 270.</p>
            <pre><code class="language-cpp">screen.setRotation(0);     // Normal (default)
screen.setRotation(90);    // Rotated 90° clockwise
screen.setRotation(180);   // Upside down
screen.setRotation(270);   // Rotated 270° clockwise</code></pre>
            <p><strong>Note:</strong> 90° and 270° rotations use a centered crop because the matrix is 12x8 (non-square).</p>
        </div>

        <div class="api-method">
            <h3>getRotation()</h3>
            <p>Get the current rotation in degrees.</p>
            <pre><code class="language-cpp">int rot = screen.getRotation();  // Returns 0, 90, 180, or 270</code></pre>
        </div>

        <div class="api-method">
            <h3>setInvert(bool)</h3>
            <p>Invert all pixels on the display (swap ON and OFF).</p>
            <pre><code class="language-cpp">screen.setInvert(true);    // Inverted display
screen.setInvert(false);   // Normal display</code></pre>
        </div>

        <div class="api-method">
            <h3>getInvert()</h3>
            <p>Check if display is currently inverted.</p>
            <pre><code class="language-cpp">bool inverted = screen.getInvert();</code></pre>
        </div>

        <h2>Matrix Info</h2>
        <table>
            <tr><th>Method</th><th>Returns</th></tr>
            <tr><td><code>width()</code></td><td>12 (matrix width)</td></tr>
            <tr><td><code>height()</code></td><td>8 (matrix height)</td></tr>
        </table>

        <h2>Canvas Animation Methods</h2>
        <p>Smooth value control for creating fluid motion without manual timing math. These utilities work in both Canvas Mode and <a href="#" class="page-link" data-page="hybrid-mode">Hybrid Mode</a>.</p>

        <div class="api-method">
            <h3>oscillate(min, max, periodMs, offset)</h3>
            <p>Returns a float that smoothly cycles between min and max using a sine wave. The value oscillates automatically based on elapsed time.</p>
            <pre><code class="language-cpp">// Value cycles 0→100→0 over 2 seconds
float brightness = oscillate(0, 100, 2000);

// Same cycle but shifted by half a period
float brightness2 = oscillate(0, 100, 2000, 0.5);</code></pre>
            <p><strong>Parameters:</strong></p>
            <ul>
                <li><code>min</code> — Minimum value</li>
                <li><code>max</code> — Maximum value</li>
                <li><code>periodMs</code> — Time for one complete cycle in milliseconds</li>
                <li><code>offset</code> — Phase offset as a fraction of the period (0.0–1.0). Default: <code>0.0</code></li>
            </ul>
            <p><strong>Note:</strong> <code>periodMs</code> must be greater than 0. If you pass 0, the function returns <code>min</code>.</p>
        </div>

        <div class="api-method">
            <h3>oscillateInt(min, max, periodMs, offset)</h3>
            <p>Integer version of oscillate() for pixel coordinates.</p>
            <pre><code class="language-cpp">// Y position oscillates 0→7→0 over 1.5 seconds
int y = oscillateInt(0, 7, 1500);
screen.point(5, y);

// Two points oscillating out of phase
int y1 = oscillateInt(0, 7, 1500);          // In phase
int y2 = oscillateInt(0, 7, 1500, 0.5);     // Opposite phase
screen.point(4, y1);
screen.point(6, y2);</code></pre>
        </div>

        <h3>Ease Class</h3>
        <p>Smooth linear interpolation from current value to a target over time. Unlike oscillate() which cycles continuously, Ease moves to a target once then stops.</p>

        <div class="api-method">
            <h3>Ease(initial)</h3>
            <p>Create an Ease object with an initial value.</p>
            <pre><code class="language-cpp">Ease x(0);      // Start at 0
Ease y(3.5);    // Start at 3.5
Ease pos;       // Start at 0 (default)</code></pre>
        </div>

        <div class="api-method">
            <h3>ease.to(target, durationMs)</h3>
            <p>Start moving toward a target value over the specified duration.</p>
            <pre><code class="language-cpp">x.to(11, 2000);   // Move to 11 over 2 seconds</code></pre>
        </div>

        <div class="api-method">
            <h3>ease.value() / ease.intValue()</h3>
            <p>Get the current interpolated value. Use <code>intValue()</code> for pixel coordinates.</p>
            <pre><code class="language-cpp">float current = x.value();      // Float value
int pixel = x.intValue();       // Rounded integer
screen.point(x.intValue(), y.intValue());</code></pre>
        </div>

        <div class="api-method">
            <h3>ease.done()</h3>
            <p>Check if the animation has reached its target.</p>
            <pre><code class="language-cpp">if (x.done())
            {
    x.to(0, 2000);  // Start new animation
}</code></pre>
        </div>

        <div class="api-method">
            <h3>ease.target()</h3>
            <p>Get the current target value.</p>
            <pre><code class="language-cpp">// Ping-pong logic
if (x.done())
{
    if (x.target() > 5)
    {
        x.to(0, 2000);    // Go left
    }
    else
    {
        x.to(11, 2000);   // Go right
    }
}</code></pre>
        </div>

        <div class="api-method">
            <h3>ease.jump(value)</h3>
            <p>Instantly set the value without animation.</p>
            <pre><code class="language-cpp">x.jump(0);        // Reset to start instantly
x.to(11, 1000);   // Then animate</code></pre>
        </div>

        <div class="api-method">
            <h3>ease.moving()</h3>
            <p>Check if currently animating.</p>
            <pre><code class="language-cpp">if (x.moving())
            {
    // Animation in progress
}</code></pre>
        </div>

        <h3>Example: Combining Ease and oscillate()</h3>
        <pre><code class="language-cpp">#include "TinyFilmFestival.h"

TinyScreen screen;
Ease xPos(0);

void setup()
{
    screen.begin();
    xPos.to(11, 2000);  // Start moving right
}

void loop()
{
    // Ping-pong horizontal
    if (xPos.done())
    {
        if (xPos.target() > 5)
        {
            xPos.to(0, 2000);
        }
        else
        {
            xPos.to(11, 2000);
        }
    }
    
    // Continuous vertical wave
    int y = oscillateInt(1, 6, 1500);
    
    screen.beginDraw();
    screen.background(OFF);
    screen.stroke(ON);
    screen.point(xPos.intValue(), y);
    screen.endDraw();
}</code></pre>
    `,

    'hybrid-mode': `
        <h1>Hybrid Mode API</h1>
        <p>Combine animations with code-drawn elements. Draw on top of playing animations.</p>

        <h2>Overview</h2>
        <p>Hybrid Mode is perfect for:</p>
        <ul>
            <li><strong>Progress indicators</strong> on animated backgrounds</li>
            <li><strong>Sparkle/effect overlays</strong> on character animations</li>
            <li><strong>HUD elements</strong> over game backgrounds</li>
            <li><strong>Interactive highlights</strong> responding to input</li>
        </ul>

        <div class="info-box note">
            <strong>Key Difference</strong>
            <code>beginDraw()</code> clears the matrix. <code>beginOverlay()</code> preserves the current animation frame.
        </div>

        <h2>Setup</h2>
        <pre><code class="language-cpp">#include "TinyFilmFestival.h"
#include "myAnimation.h"

TinyScreen screen;
Animation anim = animation;

void setup()
{
    screen.begin();
    screen.play(anim, LOOP);
}

void loop()
{
    // 1. Update animation frame
    screen.update();
    
    // 2. Draw overlay on top
    screen.beginOverlay();
    screen.point(11, 0);        // Status indicator in corner
    screen.endOverlay();
}</code></pre>

        <h2>Overlay Methods</h2>

        <div class="api-method">
            <h3>beginOverlay()</h3>
            <p>Begin an overlay drawing operation. Preserves the current animation frame underneath.</p>
            <pre><code class="language-cpp">screen.beginOverlay();
// Draw on top of animation
screen.endOverlay();</code></pre>
        </div>

        <div class="api-method">
            <h3>endOverlay()</h3>
            <p>End the overlay and display the combined result.</p>
            <pre><code class="language-cpp">screen.endOverlay();</code></pre>
        </div>

        <h2>Drawing in Overlay</h2>
        <p>All Canvas Mode drawing methods work in overlay mode:</p>
        <pre><code class="language-cpp">screen.update();

screen.beginOverlay();

// Points
screen.point(x, y);

// Lines
screen.line(x1, y1, x2, y2);

// Shapes
screen.rect(x, y, w, h);
screen.circle(cx, cy, r);

screen.endOverlay();</code></pre>

        <h2>Example: Progress Bar Overlay</h2>
        <pre><code class="language-cpp">#include "TinyFilmFestival.h"
#include "loading.h"

TinyScreen screen;
Animation loadAnim = loading;
int progress = 0;
unsigned long lastUpdate = 0;

void setup()
{
    screen.begin();
    screen.play(loadAnim, LOOP);
}

void loop()
{
    screen.update();
    
    // Update progress every 50ms (non-blocking)
    if (millis() - lastUpdate >= 50)
    {
        lastUpdate = millis();
        progress = (progress + 1) % 101;
    }
    
    // Draw progress bar on bottom row
    screen.beginOverlay();
    screen.stroke(ON);
    int barWidth = map(progress, 0, 100, 0, 12);
    screen.line(0, 7, barWidth - 1, 7);
    screen.endOverlay();
}</code></pre>

        <h2>Display Transform</h2>
        <p>Transform methods work with both the animation and overlay:</p>
        <pre><code class="language-cpp">screen.setRotation(180);   // Flip display upside down
screen.setInvert(true);    // Invert all pixels</code></pre>
        <p>See <a href="#" class="page-link" data-page="canvas-mode">Canvas Mode API</a> for full transform documentation.</p>

        <h2>Canvas Animation Methods</h2>
        <p>Smooth value control for creating fluid motion in overlays. These utilities work in both Hybrid Mode and <a href="#" class="page-link" data-page="canvas-mode">Canvas Mode</a>.</p>

        <div class="api-method">
            <h3>oscillate(min, max, periodMs, offset)</h3>
            <p>Returns a float that smoothly cycles between min and max using a sine wave.</p>
            <pre><code class="language-cpp">// Overlay indicator that pulses up and down
screen.update();
screen.beginOverlay();
int y = oscillateInt(0, 7, 1500);
screen.point(11, y);
screen.endOverlay();</code></pre>
            <p><strong>Note:</strong> <code>periodMs</code> must be greater than 0. If you pass 0, the function returns <code>min</code>.</p>
        </div>

        <div class="api-method">
            <h3>oscillateInt(min, max, periodMs, offset)</h3>
            <p>Integer version for pixel coordinates. Use <code>offset</code> (0.0–1.0) to shift the phase so multiple indicators move out of sync.</p>
            <pre><code class="language-cpp">// Multiple indicators at different speeds and phases
screen.beginOverlay();
int y1 = oscillateInt(0, 7, 3000);           // Slow
int y2 = oscillateInt(0, 7, 1500, 0.25);     // Medium, quarter-phase offset
int y3 = oscillateInt(0, 7, 750, 0.5);       // Fast, half-phase offset
screen.point(9, y1);
screen.point(10, y2);
screen.point(11, y3);
screen.endOverlay();</code></pre>
        </div>

        <h3>Ease Class</h3>
        <p>Smooth interpolation to target values. Perfect for animated HUD elements.</p>

        <div class="api-method">
            <h3>Ease(initial)</h3>
            <p>Create an Ease object with an initial value.</p>
            <pre><code class="language-cpp">Ease barWidth(0);  // Progress bar starts at 0</code></pre>
        </div>

        <div class="api-method">
            <h3>ease.to(target, durationMs)</h3>
            <p>Animate toward a target value.</p>
            <pre><code class="language-cpp">barWidth.to(11, 3000);  // Fill over 3 seconds</code></pre>
        </div>

        <div class="api-method">
            <h3>ease.value() / ease.intValue()</h3>
            <p>Get the current interpolated value.</p>
            <pre><code class="language-cpp">screen.beginOverlay();
for (int x = 0; x <= barWidth.intValue(); x++)
{
    screen.point(x, 7);  // Progress bar
}
screen.endOverlay();</code></pre>
        </div>

        <div class="api-method">
            <h3>ease.done() / ease.target() / ease.jump()</h3>
            <p>Control methods for sequencing animations.</p>
            <pre><code class="language-cpp">if (barWidth.done())
            {
    delay(1000);           // Pause when full
    barWidth.jump(0);      // Reset instantly
    barWidth.to(11, 3000); // Restart animation
}</code></pre>
        </div>

        <h3>Example: Animated Progress Bar Overlay</h3>
        <pre><code class="language-cpp">#include "TinyFilmFestival.h"
#include "idle.h"

TinyScreen screen;
Animation idleAnim = idle;
Ease barWidth(0);

void setup()
{
    screen.begin();
    screen.play(idleAnim, LOOP);
    barWidth.to(11, 3000);
}

void loop()
{
    screen.update();
    
    if (barWidth.done())
    {
        delay(1000);
        barWidth.jump(0);
        barWidth.to(11, 3000);
    }
    
    screen.beginOverlay();
    screen.stroke(ON);
    for (int x = 0; x <= barWidth.intValue(); x++)
    {
        screen.point(x, 7);
    }
    screen.endOverlay();
}</code></pre>

        <h3>Example: Moving Indicator with Oscillation</h3>
        <pre><code class="language-cpp">#include "TinyFilmFestival.h"
#include "landscape.h"

TinyScreen screen;
Animation bgAnim = landscape;
Ease xPos(0);

void setup()
{
    screen.begin();
    screen.play(bgAnim, LOOP);
    xPos.to(11, 2000);
}

void loop()
{
    screen.update();
    
    // Ping-pong horizontal
    if (xPos.done())
    {
        if (xPos.target() > 5)
        {
            xPos.to(0, 2000);
        }
        else
        {
            xPos.to(11, 2000);
        }
    }
    
    // Oscillating vertical
    int y = oscillateInt(1, 6, 1000);
    
    screen.beginOverlay();
    screen.point(xPos.intValue(), y);
    screen.endOverlay();
}</code></pre>
    `,

    'examples-basics': `
        <h1>Basic Examples</h1>
        <p>Simple examples to get started with the TinyFilmFestival library.</p>

        <h2>SimpleLED</h2>
        <p>The most basic example — control individual LEDs.</p>
        <pre><code class="language-cpp">#include "TinyFilmFestival.h"

unsigned long lastBlink = 0;
bool ledState = false;

void setup()
{
    ledBegin();
    ledWrite(0, 0, HIGH);     // Top-left ON
}

void loop()
{
    // Non-blocking blink
    if (millis() - lastBlink >= 500)
    {
        lastBlink = millis();
        ledState = !ledState;
        ledWrite(5, 3, ledState ? HIGH : LOW);
    }
}</code></pre>

        <h2>FirstAnimation</h2>
        <p>Load and play a pre-made animation file.</p>
        <pre><code class="language-cpp">#include "TinyFilmFestival.h"
#include "animation.h"

TinyScreen screen;
Animation myAnim = animation;

void setup()
{
    screen.begin();
    screen.play(myAnim, LOOP);
}

void loop()
{
    screen.update();
}</code></pre>

        <h2>FirstCanvas</h2>
        <p>Draw graphics with code.</p>
        <pre><code class="language-cpp">#include "TinyFilmFestival.h"

TinyScreen screen;

void setup()
{
    screen.begin();
}

void loop()
{
    screen.beginDraw();
    screen.background(OFF);
    screen.stroke(ON);
    screen.line(0, 0, 11, 7);
    screen.endDraw();
}</code></pre>
    `,

    'examples-animation': `
        <h1>Animation Mode Examples</h1>
        <p>Examples demonstrating Animation Mode features including layering, playback control, and positioning.</p>

        <h2>Examples</h2>
        <ul class="example-list">
            <li><a href="#" class="example-link" data-page="example-layered-animations"><h5>LayeredAnimations</h5><p>Stack multiple animations on different layers</p></a></li>
            <li><a href="#" class="example-link" data-page="example-playback-control"><h5>PlaybackControl</h5><p>Pause, resume, and control animation speed</p></a></li>
            <li><a href="#" class="example-link" data-page="example-positioned-animation"><h5>PositionedAnimation</h5><p>Dynamically position an animation clip in x,y with edge clipping</p></a></li>
        </ul>
    `,

    'examples-canvas': `
        <h1>Canvas Mode Examples</h1>
        <p>Examples demonstrating Canvas Mode drawing features.</p>

        <h2>Examples</h2>
        <ul class="example-list">
            <li><a href="#" class="example-link" data-page="example-scrolling-text"><h5>ScrollingText</h5><p>Display scrolling text on the matrix</p></a></li>
            <li><a href="#" class="example-link" data-page="example-moving-shapes"><h5>MovingShapes</h5><p>Animate shapes moving across the display</p></a></li>
            <li><a href="#" class="example-link" data-page="example-layered-graphics"><h5>LayeredGraphics</h5><p>Multiple animated elements at different speeds</p></a></li>
        </ul>
    `,

    'examples-hybrid': `
        <h1>Hybrid Mode Examples</h1>
        <p>Examples demonstrating Hybrid Mode overlay features.</p>

        <h2>Examples</h2>
        <ul class="example-list">
            <li><a href="#" class="example-link" data-page="example-animation-overlay"><h5>AnimationWithOverlay</h5><p>Draw dynamic content over a playing animation</p></a></li>
        </ul>
    `,

    'examples-canvas-animation': `
        <h1>Canvas Animation Examples</h1>
        <p>Examples demonstrating smooth code-based animations using Ease and oscillate utilities.</p>

        <h2>Ease Class</h2>
        <p>The <code>Ease</code> class provides smooth interpolation between values over time — perfect for point-to-point motion.</p>
        <pre><code class="language-cpp">Ease x(0);           // Create with start value
x.to(11, 2000);      // Animate to 11 over 2 seconds
x.intValue();        // Get current value as int
x.done();            // Check if animation complete
x.target();          // Get target value</code></pre>

        <h2>Oscillate Functions</h2>
        <p>The <code>oscillateInt()</code> and <code>oscillate()</code> functions create continuous sine wave motion.</p>
        <pre><code class="language-cpp">int y = oscillateInt(0, 7, 1500);      // Oscillate 0-7 over 1.5 seconds
    float val = oscillate(0.0, 1.0, 1000);  // Oscillate 0.0-1.0 over 1 second</code></pre>

        <h2>Examples</h2>
        <ul class="example-list">
            <li><a href="#" class="example-link" data-page="example-ease-demo"><h5>EaseDemo</h5><p>Smooth motion between target positions using Ease class</p></a></li>
            <li><a href="#" class="example-link" data-page="example-oscillator-demo"><h5>OscillatorDemo</h5><p>Continuous sine wave motion using oscillateInt()</p></a></li>
            <li><a href="#" class="example-link" data-page="example-smooth-animation"><h5>SmoothAnimation</h5><p>Combining Ease and oscillate for complex motion</p></a></li>
        </ul>
    `,

    'examples-utilities': `
        <h1>Utility Examples</h1>
        <p>Stress tests and comprehensive feature demonstrations.</p>

        <h2>Test Sketches</h2>
        <ul class="example-list">
            <li><a href="#" class="example-link" data-page="example-drawing-test"><h5>DrawingTest</h5><p>Comprehensive Canvas Mode stress test (15 tests)</p></a></li>
            <li><a href="#" class="example-link" data-page="example-hybrid-test"><h5>HybridTest</h5><p>Hybrid Mode overlay stress test (9 tests)</p></a></li>
            <li><a href="#" class="example-link" data-page="example-animation-test"><h5>AnimationTest</h5><p>Animation Mode comprehensive test (12 tests)</p></a></li>
            <li><a href="#" class="example-link" data-page="example-playback-test"><h5>PlaybackTest</h5><p>Focused playback control test</p></a></li>
        </ul>
    `,

    'deprecated': `
        <h1>V1 Deprecated Methods</h1>
        <p>These V1.x methods are still supported for backward compatibility but are deprecated in V2.0. New projects should use the V2 <code>TinyScreen</code> API.</p>

        <h2>Class Aliases</h2>

        <h3>TinyFilmFestival</h3>
        <p>Deprecated alias for <code>TinyScreen</code>.</p>
        <pre><code class="language-cpp">// V1 (still works)
TinyFilmFestival film;

// V2 (recommended)
TinyScreen screen;</code></pre>

        <h3>CombinedFilmFestival</h3>
        <p>Deprecated — use <code>TinyScreen</code> with <code>addLayer()</code> instead.</p>
        <pre><code class="language-cpp">// V1 (still works)
TinyFilmFestival bg, fg;
CombinedFilmFestival combined;

combined.begin();
bg.startAnimation(bgAnim, LOOP);
fg.startAnimation(fgAnim, LOOP);
combined.addFilm(bg);
combined.addFilm(fg);

// V2 (recommended)
TinyScreen screen;

screen.begin();
screen.addLayer();
screen.play(bgAnim, LOOP);
screen.playOnLayer(1, fgAnim, LOOP);</code></pre>

        <h2>Deprecated Methods</h2>

        <h3>startAnimation()</h3>
        <p>Deprecated — use <code>play()</code> instead.</p>
        <pre><code class="language-cpp">// V1 (still works)
film.startAnimation(myAnim, LOOP);
film.startAnimation(myAnim, LOOP, 2, 6);

// V2 (recommended)
screen.play(myAnim, LOOP);
screen.play(myAnim, LOOP, 2, 6);</code></pre>

        <h2>PlayMode Constants</h2>
        <p>Both naming styles work:</p>
        <table>
            <tr><th>V1 Constant</th><th>V2 Constant</th><th>Description</th></tr>
            <tr><td><code>PLAY_ONCE</code></td><td><code>ONCE</code></td><td>Play once and stop</td></tr>
            <tr><td><code>PLAY_LOOP</code></td><td><code>LOOP</code></td><td>Loop continuously</td></tr>
            <tr><td><code>PLAY_BOOMERANG</code></td><td><code>BOOMERANG</code></td><td>Ping-pong</td></tr>
        </table>
    `,

    // Individual Examples - Basics
    'example-simple-led': `
        <h1>SimpleLED Example</h1>
        <p class="example-breadcrumb">Examples → Basics → SimpleLED</p>
        <p>The most basic example — control individual LEDs like <code>digitalWrite()</code>.</p>
        <pre><code class="language-cpp">#include "TinyFilmFestival.h"

unsigned long lastToggle = 0;
bool ledState = false;
int blinkSpeed = 500;   // Blink interval in milliseconds

void setup()
{
    ledBegin();                   // Initialize matrix
    
    ledWrite(0, 0, HIGH);         // Top-left ON
    ledWrite(11, 0, HIGH);        // Top-right ON
    ledWrite(0, 7, HIGH);         // Bottom-left ON
    ledWrite(11, 7, HIGH);        // Bottom-right ON
}

void loop()
{
    // Non-blocking blink using millis()
    if (millis() - lastToggle >= blinkSpeed)
    {
        lastToggle = millis();
        ledState = !ledState;
        ledWrite(5, 3, ledState);     // Center 2x2 block blinks together
        ledWrite(6, 3, ledState);
        ledWrite(5, 4, ledState);
        ledWrite(6, 4, ledState);
    }
}</code></pre>
        <h2>What This Demonstrates</h2>
        <ul>
            <li>Initializing the LED matrix with <code>ledBegin()</code></li>
            <li>Turning LEDs on/off with <code>ledWrite()</code></li>
            <li>Non-blocking timing with <code>millis()</code></li>
            <li><code>ledWrite()</code> accepts bool directly (no ternary needed)</li>
            <li>Multiple LEDs can be updated together in the same timer block</li>
        </ul>
    `,

    'example-blink-frame': `
        <h1>BlinkFrame Example</h1>
        <p class="example-breadcrumb">Examples → Basics → BlinkFrame</p>
        <p>A solid inner rectangle with an outer frame of LEDs blinking at random rates.</p>
        <pre><code class="language-cpp">#include "TinyFilmFestival.h"

void setup()
{
    ledBegin();
    randomSeed(analogRead(A0));   // Seed RNG for varied rates

    for (int x = 0; x < 12; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            // Inner rectangle: x 2-9, y 2-5  (solid ON)
            if (x >= 2 && x <= 9 && y >= 2 && y <= 5)
            {
                ledWrite(x, y, HIGH);
            }
            else
            {
                // Outer frame: blink at a random rate (100-800 ms)
                ledBlink(x, y, random(100, 800));
            }
        }
    }
}

void loop()
{
    ledUpdate();   // Process all blink timers — call every loop!
}</code></pre>
        <h2>What This Demonstrates</h2>
        <ul>
            <li>Using <code>ledBlink()</code> with per-LED random rates</li>
            <li>Mixing <code>ledWrite()</code> (static) and <code>ledBlink()</code> (animated) in the same sketch</li>
            <li>All 96 LEDs managed from one shared buffer — blinking and static coexist</li>
            <li>Calling <code>ledUpdate()</code> every loop to drive blink timers</li>
        </ul>
        <div class="info-box note">
            <strong>See also:</strong> <a href="#" data-page="led-blink">LED Blink API Reference</a> for the full list of blink functions.
        </div>
    `,

    'example-first-animation': `
        <h1>FirstAnimation Example</h1>
        <p class="example-breadcrumb">Examples → Basics → FirstAnimation</p>
        <p>Load and play a pre-made animation file from the LED Matrix Editor.</p>
        <pre><code class="language-cpp">#include "TinyFilmFestival.h"
#include "animation.h"

TinyScreen screen;
Animation myAnim = animation;

void setup()
{
    screen.begin();
    
    // Play animation in a loop
    // Try also: ONCE (play once) or BOOMERANG (forward/backward)
    screen.play(myAnim, LOOP);
}

void loop()
{
    screen.update();
}</code></pre>
        <h2>What This Demonstrates</h2>
        <ul>
            <li>Including an animation <code>.h</code> file</li>
            <li>Creating a <code>TinyScreen</code> object</li>
            <li>Wrapping raw animation data in an <code>Animation</code> object</li>
            <li>Playing with <code>LOOP</code> mode</li>
            <li>Calling <code>update()</code> every loop</li>
        </ul>

        <h2>Using Your Own Animation</h2>
        <p>To use an animation you created in the <a href="#" data-page="editor-guide">LED Matrix Editor</a>:</p>
        <ol>
            <li>Create your animation in the editor and click the <img src="images/hDownload.png" alt="Export" class="icon-inline"> button to download the <code>.h</code> file</li>
            <li>Copy the downloaded <code>.h</code> file into the same folder as your <code>.ino</code> sketch</li>
            <li>Update the <code>#include</code> to match your file name</li>
            <li>Update the <code>Animation</code> variable to match the array name inside the <code>.h</code> file</li>
        </ol>
        <p>For example, if you downloaded a file called <code>myWalk.h</code> and it contains an array called <code>myWalk</code>:</p>
        <pre><code class="language-cpp">#include "TinyFilmFestival.h"
#include "myWalk.h"              // Your downloaded file

TinyScreen screen;
Animation myAnim = myWalk;       // Must match the array name in the .h file

void setup()
{
    screen.begin();
    screen.play(myAnim, LOOP);
}

void loop()
{
    screen.update();
}</code></pre>
        <div class="info-box warning">
            <strong>Important</strong>
            Do not include spaces in the file name when exporting from the editor. The file name and the array name inside the file must also be valid C++ identifiers (no spaces, no special characters).
        </div>
    `,

    'example-first-canvas': `
        <h1>FirstCanvas Example</h1>
        <p class="example-breadcrumb">Examples → Basics → FirstCanvas</p>
        <p>Draw graphics with code using <code>oscillateInt()</code> for smooth motion.</p>
        <pre><code class="language-cpp">#include "TinyFilmFestival.h"

TinyScreen screen;

// Animation speed (milliseconds for one complete cycle)
int Y_PERIOD = 1500;  // How fast the point moves up/down

void setup()
{
    screen.begin();
}

void loop()
{
    screen.beginDraw();
    screen.background(OFF);
    screen.stroke(ON);
    
    // Point oscillates smoothly between y=0 and y=7
    int y = oscillateInt(0, 7, Y_PERIOD);
    screen.point(5, y);
    
    screen.endDraw();
}</code></pre>
        <h2>What This Demonstrates</h2>
        <ul>
            <li>Using <code>beginDraw()</code> and <code>endDraw()</code></li>
            <li>Clearing with <code>background(OFF)</code></li>
            <li>Using <code>oscillateInt()</code> for smooth cyclic motion</li>
            <li>Breaking out timing variables (<code>Y_PERIOD</code>) for easy adjustment</li>
        </ul>
    `,

    // Individual Examples - Animation Mode
    'example-layered-animations': `
        <h1>LayeredAnimations Example</h1>
        <p class="example-breadcrumb">Examples → Animation Mode → LayeredAnimations</p>
        <p>Stack multiple animations on different layers for complex visual effects.</p>
        <pre><code class="language-cpp">#include "TinyFilmFestival.h"
#include "landscape.h"
#include "fiz.h"

TinyScreen screen;
Animation bgAnim = landscape;
Animation fgAnim = fiz;

void setup()
{
    screen.begin();
    screen.addLayer();                        // Add layer 1
    
    screen.play(bgAnim, LOOP);                // Layer 0 (background)
    screen.playOnLayer(1, fgAnim, LOOP);      // Layer 1 (foreground)
}

void loop()
{
    screen.update();
}</code></pre>
        <h2>What This Demonstrates</h2>
        <ul>
            <li>Adding animation layers with <code>addLayer()</code></li>
            <li>Playing animations on specific layers</li>
            <li>Layer 0 is the background, higher layers render on top</li>
            <li>Each layer can have independent speed and playback control</li>
        </ul>
    `,

    'example-playback-control': `
        <h1>PlaybackControl Example</h1>
        <p class="example-breadcrumb">Examples → Animation Mode → PlaybackControl</p>
        <p>Control animation playback via Serial commands — pause, resume, change speed.</p>

        <div class="info-box note">
            <strong>Using the Serial Monitor</strong>
            <ol>
                <li>Upload the sketch to your board</li>
                <li>In the Arduino IDE, go to <strong>Tools → Serial Monitor</strong> (or press <code>Ctrl+Shift+M</code> / <code>Cmd+Shift+M</code>)</li>
                <li>Set the baud rate to <strong>9600</strong> (bottom-right dropdown)</li>
                <li>Type a single character (<code>p</code>, <code>r</code>, <code>+</code>, or <code>-</code>) into the text field at the top and press <strong>Send</strong> (or hit Enter)</li>
            </ol>
            Make sure <strong>"No line ending"</strong> is selected in the dropdown next to the baud rate so only the character itself is sent.
        </div>

        <pre><code class="language-cpp">#include "TinyFilmFestival.h"
#include "animation.h"

TinyScreen screen;
Animation anim = animation;

void setup()
{
    Serial.begin(9600);
    screen.begin();
    screen.play(anim, LOOP);
    
    Serial.println("Commands: p=pause, r=resume, +=faster, -=slower");
}

void loop()
{
    screen.update();
    
    if (Serial.available())
    {
        char c = Serial.read();
        
        switch (c)
        {
            case 'p': screen.pause(); break;
            case 'r': screen.resume(); break;
            case '+': screen.setSpeed(screen.getCurrentSpeed() + 50); break;
            case '-': screen.setSpeed(max(10, screen.getCurrentSpeed() - 50)); break;
        }
    }
}</code></pre>
        <h2>What This Demonstrates</h2>
        <ul>
            <li>Pausing and resuming animations</li>
            <li>Dynamically changing playback speed</li>
            <li>Reading current speed with <code>getCurrentSpeed()</code></li>
            <li>Serial communication for debugging</li>
        </ul>
    `,

    'example-positioned-animation': `
        <h1>PositionedAnimation Example</h1>
        <p class="example-breadcrumb">Examples → Animation Mode → PositionedAnimation</p>
        <p>Dynamically position an animation clip anywhere on the display. The character bounces around using <code>oscillateInt()</code>, and pixels are automatically clipped at the matrix edges.</p>

        <pre><code class="language-cpp">#include "TinyFilmFestival.h"
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
}</code></pre>
        <h2>What This Demonstrates</h2>
        <ul>
            <li>Using <code>setPosition(x, y)</code> to move an animation on the display</li>
            <li>Automatic edge clipping — pixels beyond the 12×8 boundary are not drawn</li>
            <li>Combining <code>oscillateInt()</code> with positioning for smooth motion</li>
            <li>Phase offset creates diagonal, organic-feeling movement</li>
            <li>Negative positions clip the animation at the left/top edges</li>
        </ul>
        <h2>How It Works</h2>
        <p>The <code>setPosition(x, y)</code> method shifts the entire animation frame by the given offset before displaying it. This happens at the bit level — each pixel in the 96-bit frame is remapped from its source position to the offset destination. Any pixel that falls outside the 12×8 matrix is simply discarded.</p>
        <div class="info-box note">
            <strong>Works with layers too!</strong>
            Use <code>setPositionOnLayer(layer, x, y)</code> to position individual layers independently. This lets you create scenes with a static background and a moving foreground character.
        </div>
    `,

    // Individual Examples - Canvas Mode
    'example-scrolling-text': `
        <h1>ScrollingText Example</h1>
        <p class="example-breadcrumb">Examples → Canvas Mode → ScrollingText</p>
        <p>Display scrolling text across the LED matrix.</p>
        <pre><code class="language-cpp">#include "TinyFilmFestival.h"

TinyScreen screen;

void setup()
{
    screen.begin();
    screen.setScrollSpeed(100);  // 100ms per pixel
}

void loop()
{
    screen.beginDraw();
    screen.background(OFF);
    screen.stroke(ON);
    screen.scrollText("Hello World!", 0, 1);
    screen.endDraw();
}</code></pre>
        <h2>What This Demonstrates</h2>
        <ul>
            <li>Using <code>scrollText()</code> for auto-scrolling text</li>
            <li>Setting scroll speed with <code>setScrollSpeed()</code></li>
            <li>Text automatically scrolls when wider than display</li>
            <li>Built-in 3x5 pixel font</li>
        </ul>
    `,

    'example-moving-shapes': `
        <h1>MovingShapes Example</h1>
        <p class="example-breadcrumb">Examples → Canvas Mode → MovingShapes</p>
        <p>Code-generated animations with different motion behaviors (ONCE, LOOP, BOOMERANG).</p>
        <pre><code class="language-cpp">#include "TinyFilmFestival.h"

TinyScreen screen;

// Motion modes
enum MotionMode { MODE_ONCE, MODE_LOOP, MODE_BOOMERANG };

int x = 0;
int direction = 1;
MotionMode currentMode = MODE_LOOP;
bool completed = false;

unsigned long lastMove = 0;
int speed = 80;

void setup()
{
    Serial.begin(9600);
    screen.begin();
    Serial.println("Send: 1=ONCE, 2=LOOP, 3=BOOMERANG");
}

void loop()
{
    if (Serial.available())
    {
        char c = Serial.read();
        if (c == '1')
        {
            currentMode = MODE_ONCE;
            resetAnimation();
        }
        if (c == '2')
        {
            currentMode = MODE_LOOP;
            resetAnimation();
        }
        if (c == '3')
        {
            currentMode = MODE_BOOMERANG;
            resetAnimation();
        }
    }
    
    if (millis() - lastMove >= speed && !completed)
    {
        updatePosition();
        lastMove = millis();
    }
    
    screen.beginDraw();
    screen.background(OFF);
    screen.fill(ON);
    screen.stroke(ON);
    screen.circle(x, 4, 4);
    screen.endDraw();
}

void updatePosition()
{
    x += direction;
    
    switch(currentMode)
    {
        case MODE_ONCE:
            if (x >= 10)
            {
                x = 10;
                completed = true;
            }
            break;
        case MODE_LOOP:
            if (x >= 10)
            {
                x = 0;
            }
            break;
        case MODE_BOOMERANG:
            if (x >= 10 || x <= 0)
            {
                direction *= -1;
            }
            break;
    }
}

void resetAnimation()
{
    x = 0;
    direction = 1;
    completed = false;
}</code></pre>
        <h2>What This Demonstrates</h2>
        <ul>
            <li>Code-based animation matching Animation Mode play modes</li>
            <li>Non-blocking timing with <code>millis()</code></li>
            <li>State machine for animation control</li>
            <li>Serial commands to change modes</li>
        </ul>
    `,

    'example-layered-graphics': `
        <h1>LayeredGraphics Example</h1>
        <p class="example-breadcrumb">Examples → Canvas Mode → LayeredGraphics</p>
        <p>Multiple independent animated elements, each at its own speed.</p>
        <pre><code class="language-cpp">#include "TinyFilmFestival.h"

TinyScreen screen;

// Layer 1: Slow scan line
int scanY = 0, scanDir = 1;
unsigned long lastScan = 0;
int SCAN_SPEED = 200;

// Layer 2: Fast bouncing dot
int dotX = 6, dotDir = 1;
unsigned long lastDot = 0;
int DOT_SPEED = 50;

// Layer 3: Pulsing ring
int ringSize = 2, ringDir = 1;
unsigned long lastRing = 0;
int RING_SPEED = 120;

void setup()
{
    screen.begin();
}

void loop()
{
    unsigned long now = millis();
    
    if (now - lastScan >= SCAN_SPEED)
    {
        scanY += scanDir;
        if (scanY >= 7 || scanY <= 0) scanDir *= -1;
        lastScan = now;
    }
    
    if (now - lastDot >= DOT_SPEED)
    {
        dotX += dotDir;
        if (dotX >= 11 || dotX <= 0) dotDir *= -1;
        lastDot = now;
    }
    
    if (now - lastRing >= RING_SPEED)
    {
        ringSize += ringDir;
        if (ringSize >= 4 || ringSize <= 1) ringDir *= -1;
        lastRing = now;
    }
    
    screen.beginDraw();
    screen.background(OFF);
    screen.stroke(ON);
    
    screen.line(0, scanY, 11, scanY);  // Scan line
    screen.point(dotX, 4);              // Bouncing dot
    screen.noFill();
    screen.circle(6, 4, ringSize * 2); // Pulsing ring
    
    screen.endDraw();
}</code></pre>
        <h2>What This Demonstrates</h2>
        <ul>
            <li>Multiple independent animation states</li>
            <li>Different speeds per element using <code>millis()</code></li>
            <li>Combining shapes in one frame</li>
            <li>Non-blocking timing for all layers</li>
        </ul>
    `,

    // Individual Examples - Hybrid Mode
    'example-animation-overlay': `
        <h1>AnimationWithOverlay Example</h1>
        <p class="example-breadcrumb">Examples → Hybrid Mode → AnimationWithOverlay</p>
        <p>Draw dynamic content over a playing animation.</p>
        <pre><code class="language-cpp">#include "TinyFilmFestival.h"
#include "landscape.h"

TinyScreen screen;
Animation bgAnim = landscape;

void setup()
{
    screen.begin();
    screen.play(bgAnim, LOOP);
}

void loop()
{
    screen.update();
    
    // Draw blinking indicator over animation
    screen.beginOverlay();
    if ((millis() / 500) % 2 == 0)
    {
        screen.point(11, 0);
    }
    screen.endOverlay();
}</code></pre>
        <h2>What This Demonstrates</h2>
        <ul>
            <li>Using <code>beginOverlay()</code> instead of <code>beginDraw()</code></li>
            <li>Overlay preserves the animation frame underneath</li>
            <li>Creating blinking effects with <code>millis()</code></li>
            <li>Combining pre-made animations with dynamic elements</li>
        </ul>
    `,

    // Individual Examples - Canvas Animation
    'example-ease-demo': `
        <h1>EaseDemo Example</h1>
        <p class="example-breadcrumb">Examples → Canvas Animation → EaseDemo</p>
        <p>Smooth motion between target positions using the Ease class.</p>
        <pre><code class="language-cpp">#include "TinyFilmFestival.h"

TinyScreen screen;

// Create Ease objects for x and y position
Ease x(0);   // Start at left
Ease y(0);   // Start at top

// Target positions (corners and center)
int targets[][2] = {
    {0, 0}, {11, 0}, {11, 7}, {0, 7}, {5, 3}
};
int numTargets = 5;
int currentTarget = 0;
unsigned long lastMove = 0;

void setup()
{
    screen.begin();
    x.to(targets[0][0], 1000);
    y.to(targets[0][1], 1000);
    lastMove = millis();
}

void loop()
{
    // When done, pause then move to next target
    if (x.done() && y.done())
    {
        if (millis() - lastMove > 500)
        {
            currentTarget = (currentTarget + 1) % numTargets;
            x.to(targets[currentTarget][0], 800);
            y.to(targets[currentTarget][1], 800);
            lastMove = millis();
        }
    }
    else
    {
        lastMove = millis();
    }
    
    screen.beginDraw();
    screen.background(OFF);
    screen.stroke(ON);
    screen.point(x.intValue(), y.intValue());
    screen.endDraw();
}</code></pre>
        <h2>What This Demonstrates</h2>
        <ul>
            <li>Creating <code>Ease</code> objects with initial values</li>
            <li>Using <code>to(target, duration)</code> to animate</li>
            <li>Checking <code>done()</code> to sequence animations</li>
            <li>Getting current value with <code>intValue()</code></li>
        </ul>
    `,

    'example-oscillator-demo': `
        <h1>OscillatorDemo Example</h1>
        <p class="example-breadcrumb">Examples → Canvas Animation → OscillatorDemo</p>
        <p>Continuous sine wave motion using oscillateInt().</p>
        <pre><code class="language-cpp">#include "TinyFilmFestival.h"

TinyScreen screen;

void setup()
{
    screen.begin();
}

void loop()
{
    screen.beginDraw();
    screen.background(OFF);
    screen.stroke(ON);
    
    // Three dots at different oscillation speeds
    
    // Slow (3 second period) - left
    int y1 = oscillateInt(0, 7, 3000);
    screen.point(2, y1);
    
    // Medium (1.5 second period) - middle
    int y2 = oscillateInt(0, 7, 1500);
    screen.point(5, y2);
    
    // Fast (0.75 second period) - right
    int y3 = oscillateInt(0, 7, 750);
    screen.point(9, y3);
    
    screen.endDraw();
}</code></pre>
        <h2>What This Demonstrates</h2>
        <ul>
            <li>Using <code>oscillateInt(min, max, period)</code></li>
            <li>Different oscillation speeds</li>
            <li>Continuous sine wave motion</li>
            <li>Multiple independent oscillators</li>
        </ul>
    `,

    'example-smooth-animation': `
        <h1>SmoothAnimation Example</h1>
        <p class="example-breadcrumb">Examples → Canvas Animation → SmoothAnimation</p>
        <p>Combining Ease and oscillate for complex smooth motion.</p>
        <pre><code class="language-cpp">#include "TinyFilmFestival.h"

TinyScreen screen;

Ease xPos(0);    // Horizontal easing

void setup()
{
    screen.begin();
    xPos.to(11, 2000);  // Start moving right
}

void loop()
{
    // Ping-pong horizontal motion
    if (xPos.done())
    {
        if (xPos.target() > 5)
        {
            xPos.to(0, 2000);
        }
        else
        {
            xPos.to(11, 2000);
        }
    }
    
    // Continuous vertical wave
    int y = oscillateInt(1, 6, 1500);
    
    screen.beginDraw();
    screen.background(OFF);
    screen.stroke(ON);
    screen.point(xPos.intValue(), y);
    screen.endDraw();
}</code></pre>
        <h2>What This Demonstrates</h2>
        <ul>
            <li>Combining <code>Ease</code> and <code>oscillateInt()</code></li>
            <li>Ease for controlled point-to-point motion</li>
            <li>oscillate for continuous cyclic motion</li>
            <li>Complex motion from simple primitives</li>
        </ul>
    `,

    // Utility test examples
    'example-drawing-test': `
        <h1>DrawingTest</h1>
        <p class="example-breadcrumb">Examples → Utilities → DrawingTest</p>
        <p>Comprehensive Canvas Mode stress test with 15 tests covering all drawing features.</p>
        
        <h2>Tests Include</h2>
        <ul>
            <li>Filled and stroked circles</li>
            <li>Diagonal and horizontal/vertical lines</li>
            <li>Random points</li>
            <li>Text at different sizes</li>
            <li>Scrolling text</li>
            <li>All rotations (0°, 90°, 180°, 270°)</li>
            <li>Inverted display mode</li>
        </ul>
        <p>Each test runs for 2 seconds. Total cycle: 30 seconds.</p>
    `,

    'example-hybrid-test': `
        <h1>HybridTest</h1>
        <p class="example-breadcrumb">Examples → Utilities → HybridTest</p>
        <p>Hybrid Mode overlay stress test with 9 tests.</p>
        
        <h2>Tests Include</h2>
        <ul>
            <li>Animation with point overlay</li>
            <li>Indicator bar overlay</li>
            <li>Corner frame overlay</li>
            <li>Moving line overlay</li>
            <li>Text overlay</li>
            <li>Inverted rectangle overlay</li>
            <li>Animation switching with overlay</li>
            <li>Paused animation with drawing</li>
            <li>Inverted animation display</li>
        </ul>
        <p>Each test runs for 3 seconds. Total cycle: 27 seconds.</p>
    `,

    'example-animation-test': `
        <h1>AnimationTest</h1>
        <p class="example-breadcrumb">Examples → Utilities → AnimationTest</p>
        <p>Animation Mode comprehensive test with 12 tests.</p>
        
        <h2>Tests Include</h2>
        <ul>
            <li>LOOP, ONCE, BOOMERANG play modes</li>
            <li>Speed changes (2x, 0.5x)</li>
            <li>Pause and resume</li>
            <li>Stop and restart</li>
            <li>Frame range playback</li>
            <li>Animation switching</li>
            <li>Invert on/off</li>
            <li>Two-layer animation</li>
            <li>Independent layer speeds</li>
            <li>Status reporting</li>
        </ul>
        <p>Each test runs for 3 seconds. Total cycle: 36 seconds.</p>
    `,

    'example-playback-test': `
        <h1>PlaybackTest</h1>
        <p class="example-breadcrumb">Examples → Utilities → PlaybackTest</p>
        <p>Focused playback control test in a 12-second repeating cycle.</p>
        
        <h2>Test Sequence</h2>
        <ol>
            <li><strong>0-3s:</strong> Play animation at normal speed</li>
            <li><strong>3-6s:</strong> Pause animation</li>
            <li><strong>6-9s:</strong> Resume animation</li>
            <li><strong>9-12s:</strong> Switch to different animation</li>
        </ol>
        <p>Cycle repeats indefinitely for continuous testing.</p>
    `,
};
