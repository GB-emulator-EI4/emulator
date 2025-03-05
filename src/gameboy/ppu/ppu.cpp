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


void PPU::fetchBackgroundTileData() {
    switch (fetcherState) {
        case FetcherState::FetchTileNumber:
            // Fetch tile number from VRAM
            // Address calculation based on SCX, SCY, and current scanline
            tileNumber = memory.fetch8(//);
            fetcherState = FetcherState::FetchTileDataLow;
            break;

        case FetcherState::FetchTileDataLow:
            // Fetch low byte of tile data from VRAM
            tileDataLow = memory.read(/* calculated address */);
            fetcherState = FetcherState::FetchTileDataHigh;
            break;

        case FetcherState::FetchTileDataHigh:
            // Fetch high byte of tile data from VRAM
            tileDataHigh = memory.read(/* calculated address */);
            fetcherState = FetcherState::PushToFIFO;
            break;

        case FetcherState::PushToFIFO:
            // Decode tile data into pixels and push to backgroundFIFO
            for (int i = 0; i < TILE_SIZE; ++i) {
                Pixel pixel;
                pixel.color = ((tileDataHigh >> (7 - i)) & 1) << 1 | ((tileDataLow >> (7 - i)) & 1);
                pixel.palette = 0; // Set palette based on LCDC register
                pixel.spritePriority = false;
                pixel.backgroundPriority = false;
                backgroundFIFO.push(pixel);
            }
            fetcherState = FetcherState::FetchTileNumber;
            fetcherXPos++;
            break;
    }
}