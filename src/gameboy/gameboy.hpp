#pragma once

#include <string>
#include <stdint.h>

using namespace std;

#include "cpu/cpu.hpp"
#include "memory/memory.hpp"
#include "logging/logger/logger.hpp"
#include "logging/log/log.hpp"
#include "utils/utils.hpp"
#include "ppu/ppu.hpp"
#include "sdl/sdl.hpp"

// Forward declaration
class CPU;
class Memory;
class PPU;

class Gameboy {
    public:
        // Get instance
        static Gameboy* getInstance();

        // Destructor
        ~Gameboy();

        // Components
        CPU* cpu;
        Memory* memory;
        PPU* ppu;

        // Renderer
        SDLRenderer* renderer;
        inline void setRenderer(SDLRenderer* renderer) { this->renderer = renderer; }

        // Init functions
        void setBootRom(const string &bootRomPath);
        void setGameRom(const string &gameRomPath);

        // Functions
        void init();
        void runMcycle();
        void freeRun();

        inline void pause() { this->running = false; }
        inline void stop() { this->running = false; }

        // Getters and Setters
        inline const int& getMcycles() const { return this->Mcycles; }
        inline const int& getTcycles() const { return this->Tcycles; }

    private:
        // Constructors
        Gameboy();

        // Singleton instance
        static Gameboy* instance;

        // Logger
        Log* logger;
    
        // Vars
        bool running;

        // Cycles counters
        int cyclesCounter;
        int Mcycles;
        int Tcycles;
};