#include <iostream>

using namespace std;

#include "ppu.hpp"
#include "memory.hpp"

PPU::PPU(Memory& memory) : memory(memory), cycleCounter(0), currentScanline(0) {
    // Initialize framebuffer
    framebuffer.fill(0);
}

PPU::~PPU() {
    // Destructor
}

