#include <iostream>
#include <string>

using namespace std;

#include "gameboy.hpp"

#include "cpu/cpu.hpp"
#include "memory/memory.hpp"
#include "logging/logger/logger.hpp"

/*

    Get instance

*/

Gameboy* Gameboy::getInstance() {
    if (!instance) instance = new Gameboy();
    return instance;
}

Gameboy* Gameboy::instance = nullptr;

/*

    Constructors and Destructors

*/

Gameboy::Gameboy() : cpu(new CPU(this)), memory(new Memory()), running(true), cyclesCount(0) {
    logger = Logger::getInstance()->getLogger("Gameboy");
    logger->log("Gameboy Constructor");
}

Gameboy::~Gameboy() {
    delete cpu;
    delete memory;
    delete logger;

    instance = nullptr;

    logger->log("Gameboy Destructor");
}

/*

    Functions

*/

void Gameboy::run() {
    logger->log("Gameboy starting");

    this->dots = 0;
    while(this->running) {
        logger->log("---> Gameboy dot cycle");

        // LCD cycle
        this->LCDcycle();

        // PPU cycle
        // TODO

        if(this->dots % 4 == 0) {
            // CPU cycle
            this->cpu->cycle();

            // Count cycles
            // if(this->cyclesCount == (3 + 3 * 8191 + 2 + 12 + 3 + 10 + 10)) this->stop();
            this->cyclesCount ++;
        }
        
        this->dots ++;

        // if(this->cpu->pc == 0xa7) this->stop();
    }
}

// Temporary function
void Gameboy::LCDcycle() {
    // Read LCD enable flag
    uint8_t& lcdc =(uint8_t&) this->memory->fetch8(0xFF40);

    // Check if LCD is enabled
    if((lcdc & 0x80) == 0) return;

    // Get and increment LY
    uint8_t& ly = (uint8_t&) this->memory->fetch8(0xFF44);

    if(this->dots >= 456) {
        if(ly == 153) {
            ly = 0;

            this->stop();
        }
        else ly ++;

        this->dots = 0;
    }

    // Log Ly and dots
    logger->log("LY: " + to_string(ly) + " Dots: " + to_string(this->dots));
}

void Gameboy::stop() {
    this->running = false;

    logger->log("Gameboy stopping");
}

void Gameboy::setBootRom(const string &bootRomPath) {
    logger->log("Gameboy setting boot ROM");
    this->memory->loadRom(BOOTROM, BOOTROM_OFFSET, bootRomPath, BOOTROM_SIZE);
}

void Gameboy::setGameRom(const string &gameRomPath) {
    logger->log("Gameboy setting game ROM");
    this->memory->loadRom(ROM_FIXED, ROM_FIXED_OFFSET, gameRomPath, ROM_FIXED_SIZE);
}