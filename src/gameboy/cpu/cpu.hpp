#pragma once

#include <stdint.h>
#include <vector>

#include "../logging/log/log.hpp"

#include "../gameboy.hpp"

// Forward declaration
class Gameboy;

// Interrupts
enum class Interrupt : uint8_t {
    VBlank = 0x1,
    LCD = 0x1 << 1,
    Timer = 0x1 << 2,
    Serial = 0x1 << 3,
    Joypad = 0x1 << 4,
};

class CPU {
    public:
        CPU(Gameboy* gameboy);
        ~CPU();

        void cycle(); // Will run a single cycle of the CPU, call in order the following functions: fetch, decode, fetchOperands, executes

        void enableInterrupt(const Interrupt interrupt); 
        void disableInterrupt(const Interrupt interrupt);

        void triggerInterrupt(const Interrupt interrupt);
        void clearInterrupt(const Interrupt interrupt);

        /*
        
            Custom instructions, used for homemade test ROMS
            It uses empty instructions slots
        
        */

        // DUMP
        void DUMPR(); // Registers
        void DUMPFlags(); // Flags
        void DUMPW(); // WRAM, Banked WRAM, HRAM
        void DUMPV(); // VRAM, OAM

        /*
        
            Getters and Setters
        
        */

        inline const uint16_t& getPC() const { return this->pc; }

    private:
        // Gameboy ref
        Gameboy* gameboy;

        // Personnal logger ref
        Log* logger;

        // Registers
        uint8_t a, f, b, c, d, e, h, l; // 8-bit registers
        uint16_t sp, pc; // 16-bit registers
        uint8_t ime; // 8-bit interrupt master enable flag

        // Interrupts
        void checkInterrupts(); // Check if an interrupt is pending and execute it

        // Execution steps
        const uint8_t& fetch() const; // Fetch the next instruction
        void decodeAndExecute(const uint8_t& opcode);
        void decodeAndExecutePrefixed(const uint8_t& opcode);

        // Operand fetchers
        uint8_t& getArith8Operand(const uint8_t& opcode);
        uint8_t& getIncDec8Operand(const uint8_t& opcode);

        /*
        
            Instructions

        */

        // JUMP
        void JRN(const int8_t& e8, const uint8_t& flag); // 0x20, 0x30
        void JRS(const int8_t& e8, const uint8_t& flag); // 0x18, 0x28
        void JPN(const uint16_t& adr, const uint8_t& flag); // 0xC2, 0xD2, 0xDA
        void JPS(const uint16_t& address, const uint8_t& flag); //0xCA, 0xDA

        // LD
        void LD(uint8_t& r1, const uint8_t& r2);
        void LD(uint8_t& r1, uint8_t& r2, const uint16_t& r3);
        void LD(uint16_t& r1, const uint8_t& r3, const uint8_t& r4);
        void LD(uint8_t& r1, uint8_t& r2, const uint8_t& r3, const uint8_t& r4); // 0x01, 0x11, 0x21, 0x31 ie load immediate 16 bit value into BC, DE, HL, SP respectivement

        // ADD, ADDC
        void ADD(uint8_t &r1, const uint8_t &r2);
        void ADD(uint16_t &r1, const int8_t &r2);
        void ADDC(uint8_t &r1, const uint8_t &r2);
        void ADD(uint8_t &r1, uint8_t &r2, const uint8_t &r3, const uint8_t &r4);

        // SUB, SUBC
        void SUB(uint8_t &r1, const uint8_t &r2);
        void SUBC(uint8_t &r1, const uint8_t &r2);

        // CP
        void CP(const uint8_t &r1, const uint8_t &r2);

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
        void CALLN(const uint16_t &adr, const uint8_t &flag);
        void CALLS(const uint16_t &adr, const uint8_t &flag);

        // RET
        void RET();

        // RL
        void RL(uint8_t &r);

        // POP
        void POP(uint8_t &r1, uint8_t &r2);

        // RLA
        void RLA();

        //RLCA
        void RLCA();

        // SWAP
        void SWAP(uint8_t &r);

        // RRA
        void RRA();

        // SRL
        void SRL(uint8_t &r);

        // RR
        void RR(uint8_t &r);

        // RES
        void RES(const uint8_t &bit, uint8_t &r);

        // RST
        void RST(const uint8_t &adr); 

        // SLA
        void SLA(uint8_t &r);

        // SET
        void SET(const uint8_t &bit, uint8_t &r);

        /*
        
            Prefixed instructions
        
        */

        void BIT(const uint8_t &bit, const uint8_t &r);

        /*

            Flags getters and setters
        
        */

        inline bool getCarry() const { return (this->f & 0x10) == 0x10; }
        inline bool getHalfCarry() const { return (this->f & 0x20) == 0x20; }
        inline bool getSub() const { return (this->f & 0x40) == 0x40; }
        inline bool getZero() const { return (this->f & 0x80) == 0x80; }

        inline void setCarry() { this->f |= 0x10; }
        inline void setHalfCarry() { this->f |= 0x20; }
        inline void setSub() { this->f |= 0x40; }
        inline void setZero() { this->f |= 0x80; }

        inline void resetCarry() { this->f &= 0xEF; }
        inline void resetHalfCarry() { this->f &= 0xDF; }
        inline void resetSub() { this->f &= 0xBF; }
        inline void resetZero() { this->f &= 0x7F; }
};