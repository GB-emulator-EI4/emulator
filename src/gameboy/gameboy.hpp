#pragma once

#include <string>

using namespace std;

#include "cpu/cpu.hpp"
#include "memory/memory.hpp"
#include "logger/logger.hpp"

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

        Logger* logger;

        // Init functions
        inline void setBootRom(const string &bootRomPath) { this->memory->loadBootrom(bootRomPath); }

        // Functions
        void run();

    private:
        static Gameboy* instance;    

        // Constructors
        Gameboy();
    
        // Vars
        bool running = true;
};