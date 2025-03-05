#pragma once

#include "memory.hpp"
#include <array>
#include <cstdint>

constexpr int TILE_SIZE = 8;

constexpr int VRAM_SIZE = 8192;
constexpr int OAM_SIZE = 160;


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
        PixelTransfer = 3
    };

    Mode getMode() const;

private:
    Memory& memory;  // Pass memory reference for accessing VRAM & OAM

    constexpr int SCREEN_WIDTH = 160;
    constexpr int SCREEN_HEIGHT = 144;

    // PPU internal state
    int cycleCounter; // Tracks cycles within a scanline
    int currentScanline;

    // Frame buffer to store pixel data
    std::array<uint8_t, ScreenWidth * ScreenHeight> framebuffer;

    //Internal methods TBD

};

