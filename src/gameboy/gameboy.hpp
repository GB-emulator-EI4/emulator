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
class Memory;

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
        void setBootRom(const string &bootRomPath);
        void setGameRom(const string &gameRomPath);

        // Functions
        void run();
        void stop();

        void LCDcycle();

    private:
        // Singleton instance
        static Gameboy* instance;

        int dots;

        // Constructors
        Gameboy();

        Log* logger;
    
        // Vars
        bool running;

        // Cycle status
        int cyclesCount;
};