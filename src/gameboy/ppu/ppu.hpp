#pragma once

#include <array>
#include <cstdint>

using namespace std;

#include "../gameboy.hpp"

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

typedef array<array<uint8_t, SCREEN_WIDTH>, SCREEN_HEIGHT> FrameBuffer; // Define a type for the framebuffer

// PPU Modes
enum class Mode : uint8_t {
    HBlank = 0,
    VBlank = 1,
    OAMSearch = 2,
    Drawing = 3
};

class Gameboy; // Forward declaration
class SDLRenderer; // Forward declaration

class PPU {
public:
    PPU(Gameboy* gameboy);
    ~PPU();

    //// Advances the PPU by one cycle
    void step(); 

    void renderScanline(); // Renders a single scanline
    void drawBackground(); // Draws the background layer
    void drawWindow(); // Draws the window layer
    void drawSprites(); // Draws sprites

    /*
    
        Getters and Setters
    
    */

    inline const Mode& getMode() const { return this->currentMode; }
    inline const FrameBuffer& getFramebuffer() const { return this->framebuffer; }

    inline const int& getCurrentLY() const { return this->currentLY; }

private:
    Gameboy* gameboy;

    Log* logger; // Logger for PPU

    // Frame buffer to store pixel data
    FrameBuffer framebuffer;
    uint8_t bgBuffer[SCREEN_HEIGHT][SCREEN_WIDTH];

    // PPU internal state
    int currentLY; // current scanline
    Mode currentMode; // Current PPU mode

    //Internal methods tbd
    void fetchBackgroundTileData();
    void fetchWindowTileData();
    void fetchSpriteData();

    

    void checkLYCInterrupt();
    void checkSTATInterrupts();

    //enable methods
    inline bool isBGEnabled() const;
    inline bool isWDEnabled() const;
    inline bool areSpritesEnabled() const;

};

