#include <iostream>

using namespace std;

#include "constants/constants.hpp"

#include "gameboy/gameboy.hpp"

Gameboy* gameboy = nullptr;
SDLRenderer* sdl = nullptr;
Log* logger = nullptr;
Logger* masterLogger = nullptr;

bool runMinishell = true;

void cleanup() {
    // Clean SDL
    sdl->cleanup();

    delete gameboy;
    delete sdl;
    
    delete logger;
    delete masterLogger;
}

void minishell() {
    string command;
    
    while(runMinishell) {
        cout << "> ";
        cin >> command;

        if(command == "q") break; // Exit
        else if(command == "m") gameboy->runMcycle(); // Run one M cycle
        else if(command == "mx") { // Run n cycles
            int cycles;

            cout << "Enter number of cycles: ";
            cin >> cycles;

            for(int i = 0; i < cycles; i++) gameboy->runMcycle();
        }
        else if(command == "f") gameboy->freeRun(); // Free run
        else if(command == "dr") gameboy->cpu->DUMPR(); // Dump all registers
        else if(command == "df") gameboy->cpu->DUMPFlags(); // Dump all flags
        else if(command == "ra") { // Run until PC reaches address
            uint16_t address;

            cout << "Enter address: ";
            cin >> hex >> address;

            while(gameboy->cpu->getPC() != address) gameboy->runMcycle();
        } else if(command == "rd") { // Read address
            uint16_t address;

            cout << "Enter address: ";
            cin >> hex >> address;

            cout << "Value at address " << intToHex(address) << ": " << intToHex((uint8_t&) gameboy->memory->fetch8(address)) << endl;
        } else if(command == "help") cout << "Available commands: q (quit), m (run one M cycle), mx (run n cycles, ask n), f (free run), dr (dump registers), df (dump flags), ra (run until PC reaches address), rd (read address)" << endl;
        else break; // Unknown command
    }
}

#include "SDL2/SDL.h"

int main() {
    // Init logger
    masterLogger = Logger::getInstance();

    // Available levels: LOG_LOG, LOG_ERROR, LOG_WARNING
    // Available domains: "Main", "Gameboy", "CPU", "Memory", "PPU"
    // Filters: "Constructor", "Destructor", "LCD infos", "CPU Fetch", "Decoding opcode", "CPU Cycle", "Fetched", "with", "DEC", "RLA", "POP", "RET"
    masterLogger->setConfig({LOG_LOG, LOG_ERROR, LOG_WARNING}, {"Memory", "CPU"}, {"LCD", "ROM", "unusable", "sound", "Constructor", "CPU Fetch", "Decoding opcode", "Cycle", "with", "DEC", "RLA", "POP", "RET" });

    // Get logger for main
    logger = masterLogger->getLogger("Main");

    // Log
    *logger << "Logger configured, initializing Gameboy";

    // Init SDL
    sdl = new SDLRenderer();
    if(!sdl->initialize()) {
        *logger << "SDL initialization failed, exiting";

        cleanup();
        return -1;
    }

    // Get instance (will init the Gameboy)
    gameboy = Gameboy::getInstance();
    gameboy->setRenderer(sdl);
    
    // Load ROMs
    logger->log("\nSet BOOT ROM");
    gameboy->setBootRom(BOOT_ROM_PATH);

    // Load game ROM
    logger->log("\nSet game ROM");
    gameboy->setGameRom(ROM_PATH);

    // Enter minishell
    logger->log("\nEntering minishell");
    minishell();

    // Free everything
    cleanup();

    return 0;
}