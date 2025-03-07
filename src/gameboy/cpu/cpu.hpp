#pragma once

#include "../gameboy.hpp"

// // struct instructions

// struct instr {
//     uint8_t opcode;
//     auto* exec_func();
//     uint8_t parameters[2];
// };


// Forward declaration
class Gameboy;

class CPU {
    public:
        CPU(Gameboy* gameboy);
        ~CPU();

        void cycle(); // Will run a single cycle of the CPU, call in order the following functions: fetch, decode, fetchOperands, executes

    private:
        // Gameboy ref
        Gameboy* gameboy;

        // Registers
        uint8_t a, f, b, c, d, e, h, l; // 8-bit registers
        uint16_t sp, pc; // 16-bit registers

        // Execution steps
        void fetch(); // Fetch the next instruction

        // void decode(); // Decode the instruction
        void decodeAndExecute(uint8_t opcode);

        // void fetchOperand();

        // void execute();

        void NOP(); // 0x00
        void STOP(uint8_t n8); // 0x10
        void JRN(int8_t& e8, char& flag); // 0x20, 0x30
        void JR(int8_t& e8, char& flag); // 0x18, 0x28
    

        void LD(uint8_t& reg1, uint8_t& reg2, uint16_t& n16); // 0x01, 0x11, 0x21, 0x31 ie load immediate 16 bit value into BC, DE, HL, SP respectivement
        

        // void LDBCrA(); // 0x02


};