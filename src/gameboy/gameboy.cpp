#include <iostream>
#include <string>

using namespace std;

#include "gameboy.hpp"

#include "cpu/cpu.hpp"
#include "memory/memory.hpp"
#include "logging/logger/logger.hpp"

#include "sdl/sdl.hpp"

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

Gameboy::Gameboy() : cpu(new CPU(this)), memory(new Memory()), ppu(new PPU(this)), running(true), cyclesCounter(0), Mcycles(0), Tcycles(0) {
    logger = Logger::getInstance()->getLogger("Gameboy");
    logger->log("Gameboy Constructor");
}

Gameboy::~Gameboy() {
    delete cpu;
    delete memory;
    delete ppu;

    delete logger;

    instance = nullptr;

    logger->log("Gameboy Destructor");
}

/*

    Functions

*/

void Gameboy::init() {
    logger->log("Gameboy initializing");

    // Reset counters
    this->cyclesCounter = 0;
    this->Mcycles = 0;
    this->Tcycles = 0;
}

void Gameboy::runMcycle() {
    // Run one M - cycle
    logger->log("---> Gameboy cycle");

    this->cyclesCounter = 0;
    while(this->cyclesCounter < 4) {
        /*
        
            PPU T cycle
        
        */

        // Check if LCD is enabled
        if((this->memory->fetch8(0xFF40) & 0x80) >> 7 == 1) {
            *logger << "PPU cycle, Tcycles: " + to_string(this->Tcycles) + ", Mcycles: " + to_string(this->Mcycles) + ", LY: " + to_string(this->ppu->getCurrentLY());
            
            // PPU cycle
            this->ppu->step();

            // Count cycles
            this->Tcycles ++;
            if(this->Tcycles >= 456) this->Tcycles = 0; // Reset cycles
        } else this->Tcycles = 0; // Reset cycles

        /*
        
            CPU conditinnal M cycle
        
        */
    
        if(this->cyclesCounter == 3) {
            // CPU cycle
            this->cpu->cycle();

            // Count cycles
            this->Mcycles ++;
        }
        
        this->cyclesCounter ++;
    }
}

void Gameboy::freeRun() {
    logger->log("Gameboy starting");

    while(1) {
        // Handle SDL events
        if(!this->renderer->handleEvents()) {
            this->stop();
        }

        // Do not excute PPU or CPU cycles when on pause
        if(!this->running) break;

        // Run one M - cycle
        this->runMcycle();
    }
}

void Gameboy::setBootRom(const string &bootRomPath) {
    logger->log("Gameboy setting boot ROM");
    this->memory->loadRom(BOOTROM, BOOTROM_OFFSET, bootRomPath, BOOTROM_SIZE);
}

void Gameboy::setGameRom(const string &gameRomPath) {
    logger->log("Gameboy setting game ROM");
    this->memory->loadRom(ROM_FIXED, ROM_FIXED_OFFSET, gameRomPath, ROM_FIXED_SIZE);
}