#pragma once

#include "../logging/log/log.hpp"

#include "../gameboy.hpp"

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

        // Personnal logger ref
        Log* logger;

        // Registers
        char a, f, b, c, d, e, h, l; // 8-bit registers
        int sp, pc; // 16-bit registers

        // Execution steps
        void fetch(); // Fetch the next instruction
        // void decode(); // Decode the instruction
        // void fetchOperands(); // Fetch the operands
        // void execute(); // Execute the instruction

        /*
        
            Instructions

        */

        // ADD, ADDC
        void ADD(uint8_t &r1, const uint8_t &r2);
        void ADDC(uint8_t &r1, const uint8_t &r2);
        void ADD(uint16_t &r1, const uint16_t &r2);

        // SUB, SUBC
        void SUB(uint8_t &r1, const uint8_t &r2);
        void SUBC(uint8_t &r1, const uint8_t &r2);

        // CP
        void CP(uint8_t &r1, const uint8_t &r2);

        // INC
        void INC(uint8_t &r1);
        void INC(uint16_t &r1);

        // DEC
        void DEC(uint8_t &r1);
        void DEC(uint16_t &r1);

        // AND
        void AND(uint8_t &r1, const uint8_t &r2);

        // OR
        void OR(uint8_t &r1, const uint8_t &r2);

        // XOR
        void XOR(uint8_t &r1, const uint8_t &r2);

        // CCF complement carry flag
        void CCF();

        // SCF set carry flag
        void SCF();

        // DAA decimal adjust accumulator
        void DAA();

        // CPL complement accumulator
        void CPL();
};