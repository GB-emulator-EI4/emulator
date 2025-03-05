#include <iostream>

using namespace std;

#include "constants/constants.hpp"

#include "gameboy/gameboy.hpp"
#include "gameboy/logging/logger/logger.hpp"

int main() {
    cout << "Starting Gameboy Emulator" << endl;

    // Init logger
    Logger* logger = Logger::getInstance();
    logger->setConfig(true, {LOG_LOG, LOG_ERROR}, {"Gameboy", "CPU", "Memory", "PPU"});

    // Get instance (will init the Gameboy)
    Gameboy* gameboy = Gameboy::getInstance();
    
    // Load ROMs
    gameboy->setBootRom(BOOT_ROM_PATH); 

    // Free everything
    delete gameboy;
    delete logger;

    return 0;
}