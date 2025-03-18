#include <iostream>

using namespace std;

#include "constants/constants.hpp"

#include "gameboy/gameboy.hpp"
#include "gameboy/logging/logger/logger.hpp"

int main() {
    // Init logger
    Logger* masterLogger = Logger::getInstance();

    // Available levels: LOG_LOG, LOG_ERROR
    // Available domains: Main, Gameboy, CPU, Memory, PPU
    masterLogger->setConfig(true, {LOG_LOG, LOG_ERROR, LOG_WARNING}, {"Gameboy", "CPU", "PPU", "Memory"}, {"Constructor", "Destructor", "CPU Fetch"});

    // Get logger for main
    Log* logger = masterLogger->getLogger("Main");

    // Log
    logger->log("Logger configured, initializing Gameboy");

    // Get instance (will init the Gameboy)
    Gameboy* gameboy = Gameboy::getInstance();
    
    // Load ROMs
    logger->log("Set BOOT ROM");
    gameboy->setBootRom(BOOT_ROM_PATH);

    // Load game ROM
    logger->log("Set game ROM");
    gameboy->setGameRom(ROM_PATH);

    // Run the emulator
    logger->log("Running emulator");
    gameboy->run();

    // Free everything
    delete gameboy;
    
    delete logger;
    delete masterLogger;

    return 0;
}