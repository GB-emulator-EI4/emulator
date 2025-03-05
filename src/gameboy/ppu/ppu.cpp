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

    // Determine the background tile map location
    // In order to set which tiles should be displayed in the Background / Window grids, background maps are used. 
    // The VRAM sections $9800-$9BFF and $9C00-$9FFF each contain one of these background maps.

    // A background map consists of 32x32 bytes representing tile numbers organized row by row. 
    // This means that the first byte in a background map is the Tile Number of the Tile at the very top left. 
    // The byte after is the Tile Number of the Tile to the right of it and so on. 
    // The 33rd byte would represent the Tile Number of the leftmost tile in the second tile row.
    uint16_t tileMapBase = (lcdc & 0x08) ? 0x9C00 : 0x9800;



    //Graphics data encoded in the 2BPP format (explained above) is stored in VRAM at addresses $8000-$97FF and is 
    // usually referred to by so-called “Tile Numbers”. As each tile takes up 16 bytes of memory, a “Tile Number” is 
    // essentially just an index of a 16-byte-block within this section of VRAM. However, there are two different addressing 
    // methods the PPU uses:

    //The 8000 method uses $8000 as a base pointer and adds (TILE_NUMBER * 16) to it, where TILE_NUMBER is an unsigned 8-bit integer.
    // Thus, the Tile Number 0 would refer to address $8000, 1 would refer to $8010, 2 to $8020 and so on.

    //The 8800 method uses $9000 as a base pointer and adds (SIGNED_TILE_NUMBER * 16) to it, where SIGNED_TILE_NUMBER is a 
    //signed 8-bit integer. Thus, the tile number 0 would refer to address $9000, 1 would refer to $9010, 2 to $9020 and so on.
    // However, 0xFF would refer to $8FF0, 0xFE to $8FE0 and so on.
    // Determine tile data mode
    bool tileDataMode = (lcdc & 0x10) != 0; // True -> 0x8000 mode, False -> 0x8800 mode

    // Compute tile row index
    // 32 * (((LY + SCY) & 0xFF) / 8)???
    int tileRow = ((scanline + scy) / 8) % 32;

    for (int x = 0; x < ScreenWidth; x++) {
        int tileCol = ((x + scx) / 8) % 32;

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
        int tileY = (scanline + scy) % 8;
        uint16_t rowAddress = tileDataAddress + (tileY * 2);

        // Fetch the two bytes representing the pixel row
        uint8_t lowByte = memory.fetch8(rowAddress);
        uint8_t highByte = memory.fetch8(rowAddress + 1);

        // Compute the pixel's color for the current screen X
        int tileX = (x + scx) % 8;
        int bit = 7 - tileX; // Pixels are stored in MSB order
        uint8_t colorIndex = ((highByte >> bit) & 1) << 1 | ((lowByte >> bit) & 1);

        // Apply the BGP palette
        uint8_t color = (bgp >> (colorIndex * 2)) & 0x03;

        // Store the pixel in the framebuffer
        framebuffer[(scanline * ScreenWidth) + x] = color;
    }
}


void PPU::fetchWindowTileData(int scanline) {
    // Fetch and render window tile data for the given scanline
    // Implementation details go here
}

void PPU::fetchSpriteData(int scanline) {
    // Fetch and render sprite data for the given scanline
    // Implementation details go here
}