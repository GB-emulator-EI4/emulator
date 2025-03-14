#pragma once

#include <string>
#include <stdint.h>

using namespace std;

#include "cpu/cpu.hpp"
#include "memory/memory.hpp"
#include "logging/logger/logger.hpp"
#include "logging/log/log.hpp"

// Forward declaration
class CPU;

class Gameboy {
    public:
        // Get instance
        static Gameboy* getInstance();

        // Destructor
        ~Gameboy();

        // Components
        CPU* cpu;
        Memory* memory;

        // Init functions
        inline void setBootRom(const string &bootRomPath) { this->memory->loadRom(BOOTROM, BOOTROM_OFFSET, bootRomPath, BOOTROM_SIZE); }
        inline void setGameRom(const string &gameRomPath) { this->memory->loadRom(ROM_FIXED, ROM_FIXED_OFFSET, gameRomPath, ROM_FIXED_SIZE); }

        // Functions
        void run();
        void stop();

    private:
        // Singleton instance
        static Gameboy* instance;

        // Constructors
        Gameboy();

        Log* logger;
    
        // Vars
        bool running;

        // Cycle status
        int cyclesCount;
};