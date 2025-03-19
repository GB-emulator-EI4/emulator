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

// Minisheel, available commands: q (exit), help (show available commands), m (run one M cycle), f (free run), dr (dump registers), ra (run until pc reaches address)

void minishell() {
    string command;
    
    while(true) {
        // Handle SDL events
        if(!sdl->handleEvents()) return;

        cout << "> ";
        cin >> command;

        if(command == "q") break;
        else if(command == "m") {
            gameboy->runMcycle();
        }
        else if(command == "f") {
            gameboy->freeRun();
        }
        else if(command == "dr") {
            gameboy->cpu->DUMPR();
        }
        else if(command == "ra") {
            uint16_t address;

            cout << "Enter address: ";
            cin >> hex >> address;

            while(gameboy->cpu->getPC() != address) gameboy->runMcycle();
        }
        else if(command == "help") cout << "Available commands: q (exit), help" << endl;
        else break;
    }
}

int main() {
    // Init logger
    masterLogger = Logger::getInstance();

    // Available levels: LOG_LOG, LOG_ERROR
    // Available domains: Main, Gameboy, CPU, Memory, PPU
    masterLogger->setConfig(true, {LOG_LOG, LOG_ERROR, LOG_WARNING}, {"Main", "Gameboy", "Memory", "CPU", "PPU"}, {"Constructor", "Destructor", "CPU Fetch", "Decoding opcode", "CPU Cycle", "LCD infos"});

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
    cout << endl;
    logger->log("Set BOOT ROM");
    gameboy->setBootRom(BOOT_ROM_PATH);

    // Load game ROM
    cout << endl;
    logger->log("Set game ROM");
    gameboy->setGameRom(ROM_PATH);

    // Enter minishell
    cout << endl;
    logger->log("Entering minishell");
    minishell();

    // Clean SDL
    sdl->cleanup();

    // Free everything
    cleanup();

    return 0;
}