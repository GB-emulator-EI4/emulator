#pragma once

#include "../memory/memory.hpp"
#include "../cpu/cpu.hpp"
#include <array>
#include <cstdint>

#define TILE_SIZE 8
#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144

#define LCDC 0xFF40

#define SCY 0xFF42
#define SCX 0xFF43

#define BGP 0xFF47
#define STAT 0xFF41

#define WX 0xFF4B
#define WY 0xFF4A

#define OBP0 0xFF48
#define OBP1 0xFF49

#define LY 0xFF44
#define LYC 0xFF45


class PPU {
public:
    PPU(Memory& memory);
    ~PPU();

    //// Advances the PPU by one cycle
    void step(); 
    //enable methods
    bool isBGEnabled() const;
    bool isWDEnabled() const;
    bool areSpritesEnabled() const;

    void renderScanline(); // Renders a single scanline
    void drawBackground(); // Draws the background layer
    void drawWindow(); // Draws the window layer
    void drawSprites(); // Draws sprites

    
    // PPU Modes
    enum class Mode : uint8_t {
        HBlank = 0,
        VBlank = 1,
        OAMSearch = 2,
        Drawing = 3
    };

    Mode getMode() const;

    // method to get the framebuffer
    const std::array<std::array<uint8_t, SCREEN_WIDTH>, SCREEN_HEIGHT>& getFramebuffer() const;

private:
    Memory& memory;  // Pass memory reference for accessing VRAM & OAM



    // PPU internal state
    int cycleCounter; // Tracks cycles within a scanline
    int currentLY; // current scanline
    Mode currentMode; // Current PPU mode

    // Frame buffer to store pixel data
    std::array<std::array<uint8_t, SCREEN_WIDTH>, SCREEN_HEIGHT> framebuffer;

    //Internal methods tbd
    void fetchBackgroundTileData();
    void fetchWindowTileData();
    void fetchSpriteData();
       
  
    uint8_t getColor(uint8_t palette, uint8_t colorID);

    void checkLYCInterrupt();

};

