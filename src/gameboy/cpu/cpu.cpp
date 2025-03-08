#include <iostream>
#include <stdint.h>
#include <string>

using namespace std;

#include "../utils/utils.hpp"
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
    logger->log("CPU Cycle, PC: " + intToHex(this->pc));

    // Fetch the next instruction
    uint8_t opcode = this->fetch();

    // Decode and execute the instruction
    this->decodeAndExecute(opcode);
}

uint8_t CPU::fetch() {
    logger->log("CPU Fetch");

    // Fetch the next instruction
    uint8_t opcode = this->gameboy->memory->fetch8(this->pc);

    logger->log("Fetched opcode: " + intToHex(opcode));

    return opcode;
}

void CPU::decodeAndExecute(const uint8_t opcode) {
    logger->log("CPU Decode and Execute");

    switch (opcode) {
        case 0x00: {
            // Log
            logger->log("NOP");

            // Increment PC
            this->pc++;
        } break;

        case 0x10: { // STOP n8
            // Log
            logger->log("STOP");

            this->gameboy->stop();
        } break;

        case 0x80: { // ADD A, B
            // Log
            logger->log("ADD A, B");

            // Execute
            this->ADD(this->a, this->b);

            // Increment PC
            this->pc++;

        } break;

        case 0x81: { // ADD A, C
            // Log
            logger->log("ADD A, C");

            // Execute
            this->ADD(this->a, this->c);

            // Increment PC
            this->pc++;

        } break;

        case 0x82: { // ADD A, D
            // Log
            logger->log("ADD A, D");

            // Execute
            this->ADD(this->a, this->d);

            // Increment PC
            this->pc++;

        } break;

        case 0x83: { // ADD A, E
            // Log
            logger->log("ADD A, E");

            // Execute
            this->ADD(this->a, this->e);

            // Increment PC
            this->pc++;

        } break;

        case 0x84: { // ADD A, H
            // Log
            logger->log("ADD A, H");

            // Execute
            this->ADD(this->a, this->h);

            // Increment PC
            this->pc++;

        } break;
        
        case 0x85: { // ADD A, L
            // Log
            logger->log("ADD A, L");

            // Execute
            this->ADD(this->a, this->l);

            // Increment PC
            this->pc++;

        } break;

        case 0x86: { // ADD A, (HL)
            // Fetch operand
            uint16_t adress = (this->h << 8) + this->l;
            uint8_t value = this->gameboy->memory->fetch8(adress);

            // Log
            logger->log("ADD A, (HL) at adress " + intToHex(adress) + " with value " + intToHex(value));

            // Execute
            this->ADD(this->a, value);

            // Read and print memory adress again, throw error if value did not change // TODO remove after proper testing
            uint8_t value2 = this->gameboy->memory->fetch8(adress);
            if(value == value2) logger->error("Memory adress did not change after ADD A, (HL)");

            // Increment PC
            this->pc++;

        } break;

        case 0x87: { // ADD A, A
            // Log
            logger->log("ADD A, A");

            // Execute
            this->ADD(this->a, this->a);

            // Increment PC
            this->pc++;

        } break;

        case 0xC6: { // ADD A, n8
            // Fetch operand
            uint8_t value = this->gameboy->memory->fetch8(this->pc + 1);

            // Log
            logger->log("ADD A, n8 with value " + intToHex(value));

            // Execute
            this->ADD(this->a, value);

            // Increment PC
            this->pc += 2;

        } break;

        /*
        
            Customs instructions
        
        */

        case 0xEB: { // DUMPR
            logger->log("Dumping registers");

            this->DUMPR();

            // Increment PC
            this->pc++;

        } break;

        case 0xEC: { // DUMPW
            logger->log("Dumping work RAM");

            this->DUMPW();

            // Increment PC
            this->pc++;

        } break;

        case 0xED: { // DUMPV
            logger->log("Dumping video RAM");

            this->DUMPV();

            // Increment PC
            this->pc++;

        } break;

        default: {
            logger->error("Unknown opcode: " + intToHex(opcode));

            this->gameboy->stop();
        } break;
    }
}

/*

    JUMP if and if not flag

*/

bool  CPU::JRN(int8_t& e8, char& flag) { // 0x20, 0x30 -> jump to pc + e8 if z flag, c flag RESET respectively
    uint8_t mask = flag=='Z' ? 0x80 : 0x10; // TODO flag is gathred with getXFlag() function
    
    if((this->f & mask) == 0) {
        this->pc += e8;
        return false;
    } else return true;    
}

bool CPU::JR(int8_t& e8, char& flag) { // 0x18, 0x28 -> jump to pc + e8 if z flag, c flag SET respectively
    uint8_t mask = flag=='Z' ? 0x80 : 0x10; // TODO flag is gathred with getXFlag() function

    if(this->f & mask) {
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

    DUMP

*/

void CPU::DUMPR() {
    // Dump registers
    logger->log("Dumping registers");

    logger->log("A: " + intToHex(this->a));
    logger->log("F: " + intToHex(this->f));
    logger->log("B: " + intToHex(this->b));
    logger->log("C: " + intToHex(this->c));
    logger->log("D: " + intToHex(this->d));
    logger->log("E: " + intToHex(this->e));
    logger->log("H: " + intToHex(this->h));
    logger->log("L: " + intToHex(this->l));
    logger->log("SP: " + intToHex(this->sp));
    logger->log("PC: " + intToHex(this->pc));
}

void CPU::DUMPW() {
    // Dump work RAM
    logger->log("Dumping work RAM");

    for(uint16_t i = 0xC000; i < 0xE000; i++) {
        uint8_t val = this->gameboy->memory->fetch8(i);
        logger->log(intToHex(i) + ": " + intToHex(val));
    }
}

void CPU::DUMPV() {
    // Dump video RAM
    logger->log("Dumping video RAM");

    for(uint16_t i = 0x8000; i < 0xA000; i++) {
        uint8_t val = this->gameboy->memory->fetch8(i);
        logger->log(intToHex(i) + ": " + intToHex(val));
    }
}