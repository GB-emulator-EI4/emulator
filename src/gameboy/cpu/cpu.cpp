#include <iostream>

using namespace std;

#include "cpu.hpp"

/*

    Constructors and Destructors

*/

CPU::CPU(Gameboy* gameboy) :  gameboy(gameboy), a(0), f(0), b(0), c(0), d(0), e(0), h(0), l(0), sp(0), pc(0) {
    cout << "CPU Constructor" << endl;
}

CPU::~CPU() {
    cout << "CPU Destructor" << endl;
}

/*

    Functions

*/

void CPU::cycle() {
    cout << "CPU Cycle" << endl;

    // Fetch the next instruction
    this->fetch();
}

void CPU::fetch() {

    // opcode fetched from memory

    cout << "CPU Fetch" << endl;

    // Fetch the next instruction
    char opcode = this->gameboy->memory->fetch8(this->pc);
}


void CPU::decodeAndExecute(uint8_t opcode) {

    cout << "CPU decode and exec" << endl;

    // Decode AND EXEC instruction -> switch case to 
    switch (opcode) {
        case 0x00 :
            this->NOP();
            break;

        case 0x01:
            break;
    }
}


void CPU::NOP() { // 0x00
    cout << "NOP" << endl;
}


void CPU::STOP(uint8_t n8) { // 0x10
    cout << "STOP n8" << endl;
    /* TODO
    enter cpu low power mode 

    ! must be followed by an additical byte to avoid misinterpretation as second instruction https://man.archlinux.org/man/gbz80.7.en#STOP
    */
    // this->pc += 2;
}


void  CPU::JRN(int8_t& e8, char& flag) { // 0x20, 0x30 -> jump to pc + e8 if z flag, c flag RESET respectively
    cout << "JR N_, e8" << endl;
    /*
    if !z/!c, add e8 to pc
    */
    uint8_t mask = flag=='Z' ? 0x80 : 0x10;

    // switch (flag) {
    //     case 'Z':
    //         mask = 0x80;
    //         break;
    //     case 'C':
    //         mask = 0x10;
    //         break;
    // }
    
    if (this->f & mask == 0) {
        this->pc += e8;
    } else {
        this->pc += 2;
    }
    
}

void CPU::JR(int8_t& e8, char& flag) { // 0x18, 0x28 -> jump to pc + e8 if z flag, c flag SET respectively
    cout << "JR _, e8" << endl;
    uint8_t mask = flag=='Z' ? 0x80 : 0x10;
    // switch (flag) {
    //     case "Z":
    //         mask = 0x80;
    //         break;
    //     case "C":
    //         mask = 0x10;
    //         break;

    if (this->f & mask) {
        this->pc += e8;
    } else {
        this->pc += 2;
    }
}


void CPU::LD(uint8_t& reg1, uint8_t& reg2, uint16_t& n16) { // 0x01, 0x11, 0x21, 0x31 ie load immediate 16 bit value into BC, DE, HL, SP respectivement
    cout << "LD reg1, reg2 n16" << endl;
    reg1 = n16 >> 8; // this->b, this->d, etc
    reg2 = n16 & 0xFF; // this->c, this->e, etc
    this->pc += 3;
}

