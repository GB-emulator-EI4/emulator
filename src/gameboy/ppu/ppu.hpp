#pragma once

#include "memory.hpp"
#include <array>
#include <cstdint>



constexpr int TILE_SIZE = 8;



#define LCDC 0xFF40

#define SCY 0xFF42
#define SCX 0xFF43

#define BGP 0xFF47
#define STAT 0xFF41




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
    
    // PPU Modes
    enum class Mode : uint8_t {
        HBlank = 0,
        VBlank = 1,
        OAMSearch = 2,
        Drawing = 3
    };

    Mode getMode() const;

private:
    Memory& memory;  // Pass memory reference for accessing VRAM & OAM

    constexpr int SCREEN_WIDTH = 160;
    constexpr int SCREEN_HEIGHT = 144;

    // PPU internal state
    int cycleCounter; // Tracks cycles within a scanline
    int LY; // current scanline
    Mode currentMode; // Current PPU mode

    // Frame buffer to store pixel data
    std::array<uint8_t, ScreenWidth * ScreenHeight> framebuffer;



    

    //Internal methods TBD
    void fetchBackgroundTileData(int scanline);
    void fetchWindowTileData(int scanline);
    void fetchSpriteData(int scanline);
};

