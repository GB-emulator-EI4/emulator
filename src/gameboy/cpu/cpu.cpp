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

void CPU::decodeAndExecute(uint8_t opcode) {
    logger->log("CPU Decode and Execute");

    switch (opcode) {
        case 0x00 :
            this->NOP();
            break;

        case 0x01:
            break;

        case 0x80: // ADD A, B
            // Execute
            this->ADD(this->a, this->b);

            // Increment PC
            this->pc++;

            break;

        case 0x81: // ADD A, C
            // Execute
            this->ADD(this->a, this->c);

            // Increment PC
            this->pc++;

            break;

        case 0x82: // ADD A, D
            // Execute
            this->ADD(this->a, this->d);

            // Increment PC
            this->pc++;

            break;

        case 0x83: // ADD A, E
            // Execute
            this->ADD(this->a, this->e);

            // Increment PC
            this->pc++;

            break;

        case 0x84: // ADD A, H
            // Execute
            this->ADD(this->a, this->h);

            // Increment PC
            this->pc++;

            break;
        
        case 0x85: // ADD A, L
            // Execute
            this->ADD(this->a, this->l);

            // Increment PC
            this->pc++;

            break;

        case 0x86: // ADD A, (HL)
            // Fetch operand
            uint16_t adress = (this->h << 8) + this->l;
            uint8_t value = this->gameboy->memory->fetch8(adress);

            // Execute
            this->ADD(this->a, value);

            // Read and print memory adress again, throw error if value did not change // TODO remove after proper testing
            uint8_t value2 = this->gameboy->memory->fetch8(adress);
            if(value == value2) logger->error("Memory adress did not change after ADD A, (HL)");

            // Increment PC
            this->pc++;

            break;

        case 0x87: // ADD A, A
            // Execute
            this->ADD(this->a, this->a);

            // Increment PC
            this->pc++;

            break;
    }
}

/*

    NOP

*/

void CPU::NOP() { // 0x00
    cout << "NOP" << endl;
}

/*

    STOP

*/

void CPU::STOP(uint8_t n8) { // 0x10
    cout << "STOP n8" << endl;

    /* TODO
        enter cpu low power mode 

        ! must be followed by an additical byte to avoid misinterpretation as second instruction https://man.archlinux.org/man/gbz80.7.en#STOP
    */
}

/*

    JUMP if and if not flag

*/

bool  CPU::JRN(int8_t& e8, char& flag) { // 0x20, 0x30 -> jump to pc + e8 if z flag, c flag RESET respectively
    uint8_t mask = flag=='Z' ? 0x80 : 0x10; // TODO flag is gathred with getXFlag() function
    
    if (this->f & mask == 0) {
        this->pc += e8;
        return false;
    } else return true;    
}

bool CPU::JR(int8_t& e8, char& flag) { // 0x18, 0x28 -> jump to pc + e8 if z flag, c flag SET respectively
    uint8_t mask = flag=='Z' ? 0x80 : 0x10; // TODO flag is gathred with getXFlag() function

    if (this->f & mask) {
        this->pc += e8;
        return false;
    } else return true;
}


void CPU::LD(uint8_t& r1, uint8_t& r2, uint8_t& r3, uint8_t& r4) { // 0x01, 0x11, 0x21, 0x31 ie load immediate 16 bit value into BC, DE, HL, SP respectivement
    r1 = r3;
    r2 = r4;
}

/*

    get/set/reset carry (bit 4 of f reg)

*/

bool CPU::getCarry() {
    // return (this->f & (1<<4))...
    return (this->f & 0x10) == 0x10;
}

void CPU::setCarry() {
    this->f |= 0x10;
}

void CPU::resetCarry() {
    this->f &= 0xEF;
}

/*

    get/set/reset half carry (bit 5 of f reg)

*/

bool CPU::getHalfCarry() {
    // return (this->f & (1<<5)) != 0;
    return (this->f & 0x20) == 0x20;
}

void CPU::setHalfCarry() {
    this->f |= 0x20;
}

void CPU::resetHalfCarry() {
    this->f &= 0xDF;
}

/*

    get/set/reset sub (bit 6 of f reg)

*/

bool CPU::getSub() {
    // return (this->f & (1<<6))...
    return (this->f & 0x40) == 0x40;
}

void CPU::setSub() {
    this->f |= 0x40;
}

void CPU::resetSub() {
    this->f &= 0xBF;
}


/*

    get/set/reset zero (bit 7 of f reg)

*/

bool CPU::getZero() {
    // return (this->f & (1<<7))...
    return (this->f & 0x80) == 0x80;
}

void CPU::setZero() {
    this->f |= 0x80;
}

void CPU::resetZero() {
    this->f &= 0x7F;
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
    this->resetSub();

    if(r1 == 0) this->setZero();
    else this->resetZero();

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
    this->resetSub();

    if(r1 == 0) this->setZero();
    else this->resetZero();

    if(halfCarryOnAddition(r1, r2)) this->setHalfCarry();
    else this->resetHalfCarry();

    if(r1 < r2) this->setCarry();
    else this->resetCarry();
}

void CPU::ADD(uint8_t &r1, uint8_t &r2, const uint8_t &r3, const uint8_t &r4) {
    // Add r2 to r1
    uint16_t result = (r1 << 8) + r2 + (r3 << 8) + r4;

    // Set result
    r1 = result >> 8;
    r2 = result & 0xFF;

    // Set flags
    this->resetSub();

    if(halfCarryOnAddition(result, (uint16_t) ((r3 << 8) + r4))) this->setHalfCarry();
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
    this->setSub();

    if(r1 == 0) this->setZero();
    else this->resetZero();

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
    this->setSub();

    if(r1 == 0) this->setZero();
    else this->resetZero();

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
    this->setSub();

    if(result == 0) this->setZero();
    else this->resetZero();

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
    this->resetSub();

    if(r1 == 0) this->setZero();
    else this->resetZero();

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
    this->setSub();

    if(r1 == 0) this->setZero();
    else this->resetZero();

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
    this->resetSub();
    this->setHalfCarry();
    this->resetCarry();

    if(r1 == 0) this->setZero();
    else this->resetZero();
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
    this->resetSub();
    this->resetHalfCarry();
    this->resetCarry();

    if(r1 == 0) this->setZero();
    else this->resetZero();
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
    this->resetSub();
    this->resetHalfCarry();
    this->resetCarry();

    if(r1 == 0) this->setZero();
    else this->resetZero();
}

/*

    CCF complement carry flag

*/

void CPU::CCF() {
    // Complement carry flag
    if(this->getCarry() == 0) this->setCarry();
    else this->resetCarry();

    // Reset other flags
    this->resetSub();
    this->resetHalfCarry();
}

/*

    SCF set carry flag

*/

void CPU::SCF() {
    // Set carry flag
    this->setCarry();

    // Reset other flags
    this->resetSub();
    this->resetHalfCarry();
}

/*

    DAA decimal adjust accumulator, see: https://rgbds.gbdev.io/docs/v0.9.1/gbz80.7#DAA

*/

void CPU::DAA() {
    uint8_t adjustment = 0;

    if(this->getSub()) {
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

    if(this->a == 0) this->setZero();
    else this->resetZero();
}

/*

    CPL complement accumulator

*/

void CPU::CPL() {
    // Complement accumulator
    this->a = ~this->a;

    // Set flags
    this->setSub();
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