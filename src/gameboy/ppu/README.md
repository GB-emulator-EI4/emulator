# PPU (Pixel Processing Unit)

The `PPU` class handles rendering the graphics for the GameBoy's display. It processes background tiles, window tiles, and sprites to generate the final framebuffer for each frame.

---

## Features

- **Background Rendering**: Renders the background layer based on the tile map and tile data.
- **Window Rendering**: Renders the window layer, which overlays the background.
- **Sprite Rendering**: Renders up to 10 sprites per scanline, respecting priority and transparency rules.
- **Scanline-Based Rendering**: Processes one scanline at a time (different from the actual hardware of the GameBoy which uses a FIFO).
- **Interrupt Handling**: Triggers LCD-related interrupts as required by the GameBoy hardware.

---

## Class Overview

### Public Methods

#### `PPU(Gameboy* gameboy)`
Constructor that initializes the PPU with a reference to the `Gameboy` instance.

#### `~PPU()`
Destructor for cleaning up resources.

#### `void step()`
Advances the PPU by one cycle, handling mode transitions and rendering logic.

#### `void renderScanline()`
Renders the current scanline by processing the background, window, and sprites.

#### `void drawBackground()`
Renders the background layer for the current scanline.

#### `void drawWindow()`
Renders the window layer for the current scanline.

#### `void drawSprites()`
Renders the sprites for the current scanline.

---

### Private Methods

#### `void fetchBackgroundTileData()`
Fetches and processes the tile data for the background layer.

#### `void fetchWindowTileData()`
Fetches and processes the tile data for the window layer.

#### `void fetchSpriteData()`
Fetches and processes the sprite data for the current scanline.

#### `void checkLYCInterrupt()`
Checks if the `LY` register matches the `LYC` register and triggers an LCD interrupt.

#### `void checkSTATInterrupts()`
Checks and triggers `STAT` interrupts based on the current PPU mode.

---

### Inline Methods

#### `inline bool isBGEnabled() const`
Checks if the background is enabled.

#### `inline bool isWDEnabled() const`
Checks if the window is enabled.

#### `inline bool areSpritesEnabled() const`
Checks if sprites are enabled.

---

## PPU Modes

The PPU operates in four modes, which are represented by the `Mode` enum:

- `HBlank`: Horizontal blanking period after rendering a scanline.
- `VBlank`: Vertical blanking period after rendering all scanlines.
- `OAMSearch`: Searching the Object Attribute Memory (OAM) for sprites on the current scanline.
- `Drawing`: Rendering the current scanline.

---

## Framebuffer

The `PPU` maintains a `FrameBuffer` object, which is a 2D array representing the pixel data for the screen. Each pixel is represented by a 2-bit color index (0-3), which corresponds to the GameBoy's grayscale palette.

---

## Memory-Mapped Registers

The `PPU` interacts with the following memory-mapped registers:

- **`LCDC` (0xFF40)**: LCD Control register.
- **`STAT` (0xFF41)**: LCD Status register.
- **`SCY` (0xFF42)**: Scroll Y register.
- **`SCX` (0xFF43)**: Scroll X register.
- **`LY` (0xFF44)**: Current scanline.
- **`LYC` (0xFF45)**: LY Compare register.
- **`BGP` (0xFF47)**: Background palette.
- **`OBP0` (0xFF48)**: Object palette 0.
- **`OBP1` (0xFF49)**: Object palette 1.
- **`WX` (0xFF4B)**: Window X position.
- **`WY` (0xFF4A)**: Window Y position.
