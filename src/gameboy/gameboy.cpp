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

Gameboy::Gameboy() : cpu(new CPU(this)), memory(new Memory()), running(true), cyclesCount(0) {
    logger = Logger::getInstance()->getLogger("Gameboy");
    logger->log("Gameboy Constructor");
}

Gameboy::~Gameboy() {
    delete cpu;
    delete memory;
    delete logger;

    instance = nullptr;

    logger->log("Gameboy Destructor");
}

/*

    Functions

*/

void Gameboy::run() {
    logger->log("Gameboy starting");

    while(this->running) {
        logger->log("---> Gameboy cycle");

        this->cpu->cycle();

        if(this->cyclesCount == (3 + 3 * 8191 + 2 + 12 + 3 + 10 + 10)) this->stop();
        this->cyclesCount ++;
    }
}

void Gameboy::stop() {
    this->running = false;

    logger->log("Gameboy stopping");
}