#include <iostream>

using namespace std;

#include "../logging/logger/logger.hpp"

#include "cpu.hpp"

/*

    Constructors and Destructors

*/

CPU::CPU(Gameboy* gameboy) :  gameboy(gameboy), a(0), f(0), b(0), c(0), d(0), e(0), h(0), l(0), sp(0), pc(0) {
    logger = Logger::getInstance()->getLogger("CPU");
    logger->log("CPU Constructor");
}

CPU::~CPU() {
    logger->log("CPU Destructor");
}

/*

    Functions

*/

void CPU::cycle() {
    logger->log("CPU Cycle");

    // Fetch the next instruction
    this->fetch();
}

void CPU::fetch() {
    logger->log("CPU Fetch");

    // Fetch the next instruction
    char opcode = this->gameboy->memory->fetch8(this->pc);
}