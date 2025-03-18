#pragma once

#include <stdint.h>
#include <vector>

#include "../logging/log/log.hpp"

#include "../gameboy.hpp"

// Forward declaration
class Gameboy;

class CPU {
    public:
        CPU(Gameboy* gameboy);
        ~CPU();

        void cycle(); // Will run a single cycle of the CPU, call in order the following functions: fetch, decode, fetchOperands, executes

        // Gameboy ref
        Gameboy* gameboy;

        // Registers
        uint8_t a, f, b, c, d, e, h, l; // 8-bit registers
        uint16_t sp, pc; // 16-bit registers

    private:

        // Personnal logger ref
        Log* logger;

        // Execution steps
        uint8_t fetch(); // Fetch the next instruction
        void decodeAndExecute(const uint8_t opcode);
        void decodeAndExecutePrefixed(const uint8_t opcode);

        // Operand fetchers
        const uint8_t& getArith8Operand(const uint8_t opcode);
        uint8_t& getIncDec8Operand(const uint8_t opcode);

        /*
        
            Instructions

        */

        // JUMP
        bool JRN(int8_t& e8, const uint8_t& flag); // 0x20, 0x30
        bool JRS(int8_t& e8, const uint8_t& flag); // 0x18, 0x28

        // LD
        void LD(uint8_t& r1, const uint8_t& r2);
        void LD(uint16_t& r1, const uint8_t& r3, const uint8_t& r4);
        void LD(uint8_t& r1, uint8_t& r2, const uint8_t& r3, const uint8_t& r4); // 0x01, 0x11, 0x21, 0x31 ie load immediate 16 bit value into BC, DE, HL, SP respectivement

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
        void INC(uint8_t &r1, uint8_t &r2);

        // DEC
        void DEC(uint8_t &r1);
        void DEC(uint8_t &r1, uint8_t &r2);

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

        // PUSH
        void PUSH(const uint8_t &r1, const uint8_t &r2);

        // CALL
        void CALL(const uint16_t &adr);

        // RET
        void RET();

        // RL
        void RL(uint8_t &r);

        // POP
        void POP(uint8_t &r1, uint8_t &r2);

        // RLA
        void RLA();

        /*
        
            Prefixed instructions
        
        */

        void BIT(const uint8_t &bit, const uint8_t &r);

        /*
        
            Custom instructions, used for homemade test ROMS
            It uses empty instructions slots
        
        */

        // DUMP
        void DUMPR(); // Registers
        void DUMPW(); // WRAM, Banked WRAM, HRAM
        void DUMPV(); // VRAM, OAM

        /*

            Flags getters and setters
        
        */

        bool getCarry(); // TODO flags functions should be declared as inline
        bool getHalfCarry();
        bool getSub();
        bool getZero(); 

        void setCarry();
        void setHalfCarry();
        void setSub();
        void setZero();

        void resetCarry();
        void resetHalfCarry();
        void resetSub();
        void resetZero();
};