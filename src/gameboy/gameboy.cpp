#include <iostream>
#include <string>

using namespace std;

#include "gameboy.hpp"

#include "cpu/cpu.hpp"
#include "memory/memory.hpp"
#include "logger/logger.hpp"

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

Gameboy::Gameboy() : running(true), cpu(new CPU(this)), memory(new Memory()), logger(Logger::getInstance()) {
    std::cout << "Gameboy Constructor" << std::endl;
}

Gameboy::~Gameboy() {
    delete cpu;
    delete memory;

    // Delete logger
    delete logger;

    instance = nullptr;

    std::cout << "Gameboy Destructor" << std::endl;
}

/*

    Init functions

*/




/*

    Functions

*/

void Gameboy::run() {
    std::cout << "Gameboy Running" << std::endl;

    while(this->running) {
        this->cpu->cycle();
    }
}