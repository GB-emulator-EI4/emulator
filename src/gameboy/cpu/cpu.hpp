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
        void decodeAndExecute(uint8_t opcode);

        /*
        
            Instructions

        */

        // GETTERS
        bool getCarry();
        bool getHalfCarry();
        bool getSub();
        bool getZero(); 

        // SETTERS
        void setCarry();
        void setHalfCarry();
        void setSub();
        void setZero();

        // RESETTERS
        void resetCarry();
        void resetHalfCarry();
        void resetSub();
        void resetZero();

        // NOP
        void NOP(); // 0x00

        // STOP
        void STOP(uint8_t n8); // 0x10

        // JUMP
        bool JRN(int8_t& e8, char& flag); // 0x20, 0x30
        bool JR(int8_t& e8, char& flag); // 0x18, 0x28

        // LD 
        void CPU::LD(uint8_t& r1, uint8_t& r2, uint8_t& r3, uint8_t& r4); // 0x01, 0x11, 0x21, 0x31 ie load immediate 16 bit value into BC, DE, HL, SP respectivement

        // ADD, ADDC
        void ADD(uint8_t &r1, const uint8_t &r2);
        void ADDC(uint8_t &r1, const uint8_t &r2);
        void ADD(uint8_t &r1, uint8_t &r2, const uint8_t &r3, const uint8_t &r4);

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