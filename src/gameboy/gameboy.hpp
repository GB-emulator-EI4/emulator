#pragma once

#include <string>

using namespace std;

#include "cpu/cpu.hpp"
#include "memory/memory.hpp"
#include "logger/logger.hpp"

class Gameboy {
    public:
        // Constructors and Destructors
        Gameboy(const string &bootromPath);
        ~Gameboy();

        // Functions
        void run();

    private:
        // Components
        CPU* cpu;
        Memory* memory;

        Logger* logger;
    
        // Vars
        bool running = true;
};