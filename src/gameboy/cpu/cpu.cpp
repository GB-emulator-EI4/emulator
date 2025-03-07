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

    ADD, 8 bits, 8 bits carry, 16 bits

*/

void CPU::ADD(uint8_t &r1, const uint8_t &r2) {
    // Add r2 to r1
    uint8_t result = r1 + r2;

    // Set result
    r1 = result;

    // Set flags
    this->resetSubFlag();

    if(r1 == 0) this->setZeroFlag();
    else this->resetZeroFlag();

    if(halfCarryOnAddition(r1, r2)) this->setHalfCarry();
    else this->resetHalfCarry();

    if(r1 < r2) this->setCarry();
    else this->resetCarry();
}

void CPU::ADDC(uint8_t &r1, const uint8_t &r2) {
    // Add r2 to r1
    uint8_t result = r1 + r2 + this->getCarry();

    // Set result
    r1 = result;

    // Set flags
    this->resetSubFlag();

    if(r1 == 0) this->setZeroFlag();
    else this->resetZeroFlag();

    if(halfCarryOnAddition(r1, r2)) this->setHalfCarry();
    else this->resetHalfCarry();

    if(r1 < r2) this->setCarry();
    else this->resetCarry();
}

void CPU::ADD(uint16_t &r1, const uint16_t &r2) {
    // Add r2 to r1
    uint8_t result = r1 + r2;

    // Set result
    r1 = result;

    // Set flags
    this->resetSubFlag();

    if(halfCarryOnAddition(r1, r2)) this->setHalfCarry();
    else this->resetHalfCarry();

    if(r1 < r2) this->setCarry();
    else this->resetCarry();
}

/*

    SUB 8 bits, 8 bits carry

*/

void CPU::SUB(uint8_t &r1, const uint8_t &r2) {
    // Subtract r2 from r1
    uint8_t result = r1 - r2;

    // Set result
    r1 = result;

    // Set flags
    this->setSubFlag();

    if(r1 == 0) this->setZeroFlag();
    else this->resetZeroFlag();

    if(halfCarryOnSubtration(r1, r2)) this->setHalfCarry();
    else this->resetHalfCarry();

    if(r1 > r2) this->setCarry();
    else this->resetCarry();
}

void CPU::SUBC(uint8_t &r1, const uint8_t &r2) {
    // Subtract r2 from r1
    uint8_t result = r1 - r2 - this->getCarry();

    // Set result
    r1 = result;

    // Set flags
    this->setSubFlag();

    if(r1 == 0) this->setZeroFlag();
    else this->resetZeroFlag();

    if(halfCarryOnSubtration(r1, r2)) this->setHalfCarry();
    else this->resetHalfCarry();

    if(r1 > r2) this->setCarry();
    else this->resetCarry();
}

/*

    CP 8 bits

*/

void CPU::CP(uint8_t &r1, const uint8_t &r2) {
    // Compare r1 with r2
    uint8_t result = r1 - r2;

    // Set flags
    this->setSubFlag();

    if(result == 0) this->setZeroFlag();
    else this->resetZeroFlag();

    if(halfCarryOnSubtration(r1, r2)) this->setHalfCarry();
    else this->resetHalfCarry();

    if(r1 > r2) this->setCarry();
    else this->resetCarry();
}

/*

    INC 8 bits, 16 bits

*/

void CPU::INC(uint8_t &r1) {
    // Increment r1
    uint8_t result = r1 + 1;

    // Set result
    r1 = result;

    // Set flags
    this->resetSubFlag();

    if(r1 == 0) this->setZeroFlag();
    else this->resetZeroFlag();

    if(halfCarryOnAddition(r1, 1)) this->setHalfCarry();
    else this->resetHalfCarry();
}

void CPU::INC(uint16_t &r1) {
    // Increment r1
    uint16_t result = r1 + 1;

    // Set result
    r1 = result;
}

/*

    DEC 8 bits, 16 bits

*/

void CPU::DEC(uint8_t &r1) {
    // Decrement r1
    uint8_t result = r1 - 1;

    // Set result
    r1 = result;

    // Set flags
    this->setSubFlag();

    if(r1 == 0) this->setZeroFlag();
    else this->resetZeroFlag();

    if(halfCarryOnSubtration(r1, 1)) this->setHalfCarry();
    else this->resetHalfCarry();
}

void CPU::DEC(uint16_t &r1) {
    // Decrement r1
    uint16_t result = r1 - 1;

    // Set result
    r1 = result;
}

/*

    AND bitwise 8 bits

*/

void CPU::AND(uint8_t &r1, const uint8_t &r2) {
    // And r1 with r2
    uint8_t result = r1 & r2;

    // Set result
    r1 = result;

    // Set flags
    this->resetSubFlag();
    this->setHalfCarry();
    this->resetCarry();

    if(r1 == 0) this->setZeroFlag();
    else this->resetZeroFlag();
}

/*

    OR bitwise 8 bits

*/

void CPU::OR(uint8_t &r1, const uint8_t &r2) {
    // Or r1 with r2
    uint8_t result = r1 | r2;

    // Set result
    r1 = result;

    // Set flags
    this->resetSubFlag();
    this->resetHalfCarry();
    this->resetCarry();

    if(r1 == 0) this->setZeroFlag();
    else this->resetZeroFlag();
}

/*

    XOR bitwise 8 bits

*/

void CPU::XOR(uint8_t &r1, const uint8_t &r2) {
    // Xor r1 with r2
    uint8_t result = r1 ^ r2;

    // Set result
    r1 = result;

    // Set flags
    this->resetSubFlag();
    this->resetHalfCarry();
    this->resetCarry();

    if(r1 == 0) this->setZeroFlag();
    else this->resetZeroFlag();
}

/*

    CCF complement carry flag

*/

void CPU::CCF() {
    // Complement carry flag
    if(this->getCarry() == 0) this->setCarry();
    else this->resetCarry();

    // Reset other flags
    this->resetSubFlag();
    this->resetHalfCarry();
}

/*

    SCF set carry flag

*/

void CPU::SCF() {
    // Set carry flag
    this->setCarry();

    // Reset other flags
    this->resetSubFlag();
    this->resetHalfCarry();
}

/*

    DAA decimal adjust accumulator, see: https://rgbds.gbdev.io/docs/v0.9.1/gbz80.7#DAA

*/

void CPU::DAA() {
    uint8_t adjustment = 0;

    if(this->getSubFlag()) {
        if(this->getHalfCarry()) adjustment += 0x06;
        if(this->getCarry()) adjustment += 0x60;

        this->resetCarry();

        this->a -= adjustment;
    } else {
        if(this->getHalfCarry() || (this->a & 0x0F) > 9) adjustment += 0x06;
        if(this->getCarry() || this->a > 0x99) adjustment += 0x60;

        this->setCarry();

        this->a += adjustment;
    }

    // Set flags
    this->resetHalfCarry();

    if(this->a == 0) this->setZeroFlag();
    else this->resetZeroFlag();
}

/*

    CPL complement accumulator

*/

void CPU::CPL() {
    // Complement accumulator
    this->a = ~this->a;

    // Set flags
    this->setSubFlag();
    this->setHalfCarry();
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