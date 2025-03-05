#include <iostream>
#include <string>

using namespace std;

#include "gameboy.hpp"

#include "cpu/cpu.hpp"
#include "memory/memory.hpp"
#include "logging/logger/logger.hpp"

/*

    Get instance

*/

Gameboy* Gameboy::getInstance() {
    if (!instance) instance = new Gameboy();
    return instance;
}

Gameboy* Gameboy::instance = nullptr;

/*

    Constructors and Destructors

*/

Gameboy::Gameboy() : running(true), cpu(new CPU(this)), memory(new Memory()) {
    logger = Logger::getInstance()->getLogger("Gameboy");
    logger->log("Gameboy Constructor");
}

Gameboy::~Gameboy() {
    delete cpu;
    delete memory;

    instance = nullptr;

    logger->log("Gameboy Destructor");
}

/*

    Init functions

*/




/*

    Functions

*/

void Gameboy::run() {
    logger->log("Gameboy starting");

    while(this->running) {
        this->cpu->cycle();
    }
}