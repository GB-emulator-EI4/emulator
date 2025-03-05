#pragma once

#include <string>

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
        inline void setBootRom(const string &bootRomPath) { this->memory->loadBootrom(bootRomPath); }

        // Functions
        void run();

    private:
        static Gameboy* instance;

        // Constructors
        Gameboy();

        Log* logger;
    
        // Vars
        bool running = true;
};