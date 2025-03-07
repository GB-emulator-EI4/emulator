#include <iostream>
#include "ppu.hpp"
#include "memory.hpp"

using namespace std;

PPU::PPU(Memory& memory) : memory(memory), cycleCounter(0), LY(0) {
    // Initialize framebuffer
    framebuffer.fill(0);
}

PPU::~PPU() {
    // Destructor
}

void PPU::step(unsigned int cycles) {
    cycleCounter += cycles;

    switch (currentMode) {
        case Mode::HBlank:
            if (cycleCounter >= 204) {
                cycleCounter -= 204;
                LY++;
                if (LY == 143) {
                    currentMode = Mode::VBlank;
                    // Trigger VBlank interrupt
                } else {
                    currentMode = Mode::OAMSearch;
                }
            }
            break;

        case Mode::VBlank:
            if (cycleCounter >= 456) {
                cycleCounter -= 456;
                LY++;
                if (LY > 153) {
                    LY = 0;
                    currentMode = Mode::OAMSearch;
                }
            }
            break;

        case Mode::OAMSearch:
            if (cycleCounter >= 80) {
                cycleCounter -= 80;
                currentMode = Mode::Drawing;
            }
            break;

        case Mode::Drawing:
            if (cycleCounter >= 172) {
                cycleCounter -= 172;
                currentMode = Mode::HBlank;
                // Render the current scanline
                renderScanline();
            }
            break;
    }
}

PPU::Mode PPU::getMode() const {
    return currentMode;
}


PPU::Mode PPU::getMode() const {
    return currentMode;
}

void PPU::renderScanline() {
    // Render the background, window, and sprites for the current scanline
    drawBackground();
    drawWindow();
    drawSprites();
}

void PPU::drawBackground() {
    // Draw the background layer for the current scanline
    fetchBackgroundTileData(LY);
}

void PPU::drawWindow() {
    // Draw the window layer for the current scanline
    fetchWindowTileData(LY);
}

void PPU::drawSprites() {
    // Draw the sprites for the current scanline
    fetchSpriteData(LY);
}

void PPU::fetchBackgroundTileData(int scanline) {
    uint8_t lcdc = memory.fetch8(LCDC); // LCDC register
    uint8_t scx = memory.fetch8(SCX);  // Scroll X
    uint8_t scy = memory.fetch8(SCY);  // Scroll Y
    uint8_t bgp = memory.fetch8(BGP);  // Background palette

    // determine tile map base addres (bit 3 of lcdc)    
    uint16_t tileMapBase = (lcdc & 0x08) ? 0x9C00 : 0x9800;

    // determine tile data mode (bit 4 of lcdc)
    bool tileDataMode = (lcdc & 0x10) != 0; // True -> 0x8000 mode, False -> 0x8800 mode

    // Compute tile row index
    int tileRow = (((scanline + scy) & 0xFF) / 8) & 0x1F;//0 &x1F it's like doing mode 32 (pour rester dasn la bonne plage 0-31)


    for (int x = 0; x < ScreenWidth; x++) {
        int tileCol = ((x + scx) / 8) & 0x1F;


        // Compute the address of the tile index
        uint16_t tileAddress = tileMapBase + (tileRow * 32) + tileCol;
        uint8_t tileIndex = memory.fetch8(tileAddress);

        // Resolve tile memory address
        uint16_t tileDataAddress;
        if (tileDataMode) {
            tileDataAddress = 0x8000 + (tileIndex * 16);
        } else {
            int8_t signedTileIndex = static_cast<int8_t>(tileIndex);
            tileDataAddress = 0x9000 + (signedTileIndex * 16);
        }

        // Compute pixel row inside the tile
        int tileY = 2*(((scanline + scy) & 7)); // c'est comme faire mod 8

        uint16_t rowAddress = tileDataAddress + tileY;

        // Fetch the two bytes representing the pixel row
        uint8_t lowByte = memory.fetch8(rowAddress);
        uint8_t highByte = memory.fetch8(rowAddress + 1);

        // Compute the pixel's color for the current screen X
        int tileX = (x + scx) & 7;
        int bit = 7 - tileX; // Pixels are stored in MSB order
        uint8_t colorIndex = ((highByte >> bit) & 1) << 1 | ((lowByte >> bit) & 1);

        // Apply the BGP palette
        uint8_t color = (bgp >> (colorIndex * 2)) & 0x03;

        // Store the pixel in the framebuffer
        framebuffer[(scanline * ScreenWidth) + x] = color;
    }
}


void PPU::fetchWindowTileData(int scanline) {

}

void PPU::fetchSpriteData(int scanline) {
    // Fetch and render sprite data for the given scanline
    // Implementation details go here
}