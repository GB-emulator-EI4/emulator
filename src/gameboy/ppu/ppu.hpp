#pragma once

#include "memory.hpp"

constexpr int SCREEN_WIDTH = 160;
constexpr int SCREEN_HEIGHT = 144;
constexpr int TILE_SIZE = 8;

constexpr int VRAM_SIZE = 8192;
constexpr int OAM_SIZE = 160;

class PPU {
public:
    PPU(Memory& memory); // Pass memory reference for accessing VRAM & OAM
    ~PPU();

    void step();  // Advances the PPU one cycle
    void renderFrame();  // Renders a full frame

    std::array<uint8_t, SCREEN_WIDTH * SCREEN_HEIGHT> framebuffer; // Stores pixel data

private:
    Memory& memory; // Reference to memory for VRAM & OAM access

    uint8_t lcdControl; // LCD Control Register (0xFF40)
    uint8_t scrollX, scrollY; // Scrolling registers

    uint8_t wx;
    uint8_t wy;

    void renderScanline();  // Render a single scanline
    void drawBackground();  // Draw background layer
    void drawSprites();  // Draw sprites
};




class PPU {
public:
    PPU(Memory& memory);
    ~PPU();

    void step(); // Advances the PPU by one cycle
    void renderScanline(); // Renders a single scanline
    void drawBackground(); // Draws the background layer
    void drawWindow(); // Draws the window layer
    void drawSprites(); // Draws sprites
    void renderFrame();  // Renders a full frame


private:
    Memory& memory;  // Pass memory reference for accessing VRAM & OAM

    // PPU Registers
    uint8_t lcdControl; // LCD Control Register
    
    uint8_t lcdStat;  // LCDC Status Register 
    
    uint8_t scrollX, scrollY; // Scrolling registers


    uint8_t ly;    // LCDC Y-Coordinate ($FF44)
    uint8_t lyc;   // LY Compare ($FF45)
    uint8_t dma;   // DMA Transfer ($FF46)
    uint8_t bgp;   // BG Palette Data ($FF47)
    uint8_t obp0;  // Object Palette 0 ($FF48)
    uint8_t obp1;  // Object Palette 1 ($FF49)
    uint8_t wy;    // Window Y Position ($FF4A)
    uint8_t wx;    // Window X Position ($FF4B)

    // PPU Internal States
    int cycleCounter; // Tracks cycles within a scanline
    enum Mode { HBLANK, VBLANK, OAM_SCAN, DRAWING } mode;
};

