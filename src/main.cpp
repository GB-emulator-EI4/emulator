#include <iostream>

using namespace std;

#include "constants/constants.hpp"

#include "gameboy/gameboy.hpp"

Gameboy* gameboy = nullptr;
SDLRenderer* sdl = nullptr;
Log* logger = nullptr;
Logger* masterLogger = nullptr;

void cleanup() {
    delete gameboy;
    delete sdl;
    
    delete logger;
    delete masterLogger;
}

int main() {
    // Init logger
    masterLogger = Logger::getInstance();

    // Available levels: LOG_LOG, LOG_ERROR
    // Available domains: Main, Gameboy, CPU, Memory, PPU
    masterLogger->setConfig(true, {LOG_LOG, LOG_ERROR, LOG_WARNING}, {"CPU", "PPU"}, {"Constructor", "Destructor", "CPU Fetch", "Decoding opcode", "CPU Cycle"});

    // Get logger for main
    logger = masterLogger->getLogger("Main");

    // Log
    *logger << "Logger configured, initializing Gameboy";

    // Init SDL
    sdl = new SDLRenderer(2);
    if(!sdl->initialize()) {
        *logger << "SDL initialization failed, exiting";

        cleanup();
        return -1;
    }

    // Get instance (will init the Gameboy)
    gameboy = Gameboy::getInstance();
    gameboy->setRenderer(sdl);
    
    // Load ROMs
    logger->log("Set BOOT ROM");
    gameboy->setBootRom(BOOT_ROM_PATH);

    // Load game ROM
    logger->log("Set game ROM");
    gameboy->setGameRom(ROM_PATH);

    // Run the emulator
    logger->log("Running emulator");
    gameboy->run();

    // Clean SDL
    sdl->cleanup();

    // Free everything
    cleanup();

    return 0;
}