#include <iostream>

using namespace std;

#include "constants/constants.hpp"

#include "gameboy/gameboy.hpp"
#include "gameboy/logging/logger/logger.hpp"

int main() {
    // Init logger
    Logger* masterLogger = Logger::getInstance();
    masterLogger->setConfig(true, {LOG_LOG, LOG_ERROR}, {"Main", "Gameboy", "CPU", "Memory", "PPU"});

    Log* logger = masterLogger->getLogger("Main");

    // Get instance (will init the Gameboy)
    Gameboy* gameboy = Gameboy::getInstance();
    
    // Load ROMs
    logger->log("Loading ROMs");
    gameboy->setBootRom(BOOT_ROM_PATH);

    // Load game ROM
    logger->log("Loading game ROM");
    gameboy->setGameRom(GAME_ROM_PATH);

    // Free everything
    delete gameboy;
    
    delete logger;
    delete masterLogger;

    return 0;
}