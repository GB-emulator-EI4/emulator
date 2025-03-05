#include <iostream>
#include <stdint.h>

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

/*

    Opcodes implementation

*/

void CPU::ADD(uint8_t &r1, const uint8_t &r2) {
    // Add r2 to r1
    uint16_t result = r1 + r2 + this->getCarry();

    // Set result
    r1 = result;

    // Set flags
    this->resetFlags();

    if(r1 == 0) this->nullFlag();
    if(halfCarryOnAddition(r1, r2)) this->setHalfCarry();
    if(r1 < r2) this->setCarry();
}




/*

    Utils for addition and subtraction
    // TODO may be a good idea to move to a separate file

*/

static constexpr bool halfCarryOnAddition(uint8_t first_num, uint8_t second_num)
{
    return (((first_num & 0x0F) + (second_num & 0x0F)) & 0x10) == 0x10;
}

static constexpr bool halfCarryOnAddition(uint16_t first_num, uint16_t second_num)
{
    return (((first_num & 0x00FF) + (second_num & 0x00FF)) & 0x0100) == 0x0100;
}

static constexpr bool halfCarryOnSubtration(uint8_t first_num, uint8_t second_num)
{
    return (int)(first_num & 0x0F) - (int)(second_num & 0x0F) < 0;
}

static constexpr bool halfCarryOnSubtration(uint16_t first_num, uint16_t second_num)
{
    return (int)(first_num & 0x00FF) - (int)(second_num & 0x00FF) < 0;
}