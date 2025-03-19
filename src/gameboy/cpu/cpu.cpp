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

CPU::CPU(Gameboy* gameboy) : gameboy(gameboy), logger(Logger::getInstance()->getLogger("CPU")), a(0), f(0), b(0), c(0), d(0), e(0), h(0), l(0), sp(0), pc(0) {
    *logger << "CPU Constructor";
}

CPU::~CPU() {
    *logger << "CPU Destructor";
    
    delete logger;
}

/*

    Functions

*/

void CPU::cycle() {
    logger->log("CPU Cycle, PC: " + intToHex(this->pc));

    // Fetch the next instruction
    const uint8_t& opcode = this->fetch();

    // Check for prefixed instructions
    if(opcode == 0xCB) {
        *logger << "Prefixed instruction";

        this->pc ++;
        return this->decodeAndExecutePrefixed(this->fetch());
    }

    // Decode and execute the instruction
    return this->decodeAndExecute(opcode);
}

const uint8_t& CPU::fetch() const {
    // Fetch the next instruction
    const uint8_t& opcode = (uint8_t&) this->gameboy->memory->fetch8(this->pc);

    *logger << "Fetched opcode: " + intToHex(opcode) + ", PC: " + intToHex(this->pc);
    return opcode;
}

void CPU::decodeAndExecute(const uint8_t& opcode) {
    *logger << "Decoding opcode: " + intToHex(opcode) + ", PC: " + intToHex(this->pc);

    const uint8_t high = opcode >> 4;
    const uint8_t low = opcode & 0xF;

    /*
    
        Switch case for LD on: r r, r [HL], [HL] r (lines 0x40 to 0x7F)
    
    */

    if(high >= 0x4 && high <= 0x7) {
        this->pc ++;

        // Get operand
        const uint8_t& r2 = this->getArith8Operand(low - (low <= 0x7 ? 0 : 0x8));

        switch(high) {
            case 0x4: {
                if(low <= 0x7) { // LD B r
                    logger->log("LD B r with r: " + intToHex(r2));
                    return this->LD(this->b, r2);
                } else { // LD C r
                    logger->log("LD C r with r: " + intToHex(r2));
                    return this->LD(this->c, r2);
                }
            } break;

            case 0x5: {
                if(low <= 0x7)  { // LD D r
                    logger->log("LD D r with r: " + intToHex(r2));
                    return this->LD(this->d, r2);
                } else { // LD E r
                    logger->log("LD E r with r: " + intToHex(r2));
                    return this->LD(this->e, r2);
                }
            } break;

            case 0x6: {
                if(low <= 0x7) { // LD H r
                    logger->log("LD H r with r: " + intToHex(r2));
                    return this->LD(this->h, r2);
                } else { // LD L r
                    logger->log("LD L r with r: " + intToHex(r2));
                    return this->LD(this->l, r2);
                }
            } break;

            case 0x7: {
                if(low <= 0x7) { // LD [HL] r
                    uint16_t adress = (this->h << 8) + this->l;

                    logger->log("LD [HL] r with r: " + intToHex(r2) + ", at adress: " + intToHex(adress));
                    return this->LD((uint8_t&) this->gameboy->memory->fetch8(adress), r2);
                } else { // LD A r
                    logger->log("LD A r with r: " + intToHex(r2));
                    return this->LD(this->a, r2);
                }
            } break;
        }
    }

    /*
    
        Other LD instructions, LD n8 r (0x02, 0x12, 0x22, 0x32, 0x06, 0x16, 0x26, 0x36, 0x0A, 0x1A, 0x2A, 0x3A, 0x0E, 0x1E, 0x2E, 0x3E)
    
    */

    if(high <= 0x3) switch(low) {
        case 0x2: {
            logger->log("LD [rr], A");
            this->pc ++;

            // Fetch memory address reference
            uint16_t address;
            if(high == 0x0) address = ((uint16_t) this->b << 8) + this->c; // BC
            else if(high == 0x1) address = (uint16_t) (this->d << 8) + this->e; // DE
            else if(high == 0x2) { // HL+
                address = ((uint16_t) this->h << 8) + this->l;

                this->h = (address + 1) >> 8;
                this->l = (address + 1) & 0xFF;
            } else if(high == 0x3) { // HL-
                address = ((uint16_t) this->h << 8) + this->l;

                this->h = (address - 1) >> 8;
                this->l = (address - 1) & 0xFF;
            }

            // Execute
            return this->LD((uint8_t&) this->gameboy->memory->fetch8(address), this->a);
        } break;

        case 0x6: {
            const uint8_t& value = (uint8_t&) this->gameboy->memory->fetch8(this->pc + 1);
            this->pc += 2;

            logger->log("LD r, n8 or LD [HL] n8 with n8: " + intToHex(value));

            if(high == 0x0) return this->LD(this->b, value);
            else if(high == 0x1) return this->LD(this->d, value);
            else if(high == 0x2) return this->LD(this->h, value);
            else if(high == 0x3) return this->LD((uint8_t&) this->gameboy->memory->fetch8((this->h << 8) + this->l), value);
        } break;

        case 0xA: {
            this->pc ++;

            // Fetch memory address reference
            uint16_t address;
            if(high == 0x0) address = ((uint16_t) this->b << 8) + this->c; // BC
            else if(high == 0x1) address = ((uint16_t) this->d << 8) + this->e; // DE
            else if(high == 0x2) { // HL+
                address = ((uint16_t) this->h << 8) + this->l;

                this->h = (address + 1) >> 8;
                this->l = (address + 1) & 0xFF;
            } else if(high == 0x3) { // HL-
                address = ((uint16_t) this->h << 8) + this->l;

                this->h = (address - 1) >> 8;
                this->l = (address - 1) & 0xFF;
            }

            // Execute
            logger->log("LD A, [adr] at adress: " + intToHex(address));
            return this->LD(this->a, this->gameboy->memory->fetch8(address));
        } break;

        case 0xE: {
            const uint8_t& value = (uint8_t&) this->gameboy->memory->fetch8(this->pc + 1);
            this->pc += 2;

            logger->log("LD r, n8 with n8: " + intToHex(value));

            if(high == 0x0) return this->LD(this->c, value);
            else if(high == 0x1) return this->LD(this->e, value);
            else if(high == 0x2) return this->LD(this->l, value);
            else if(high == 0x3) return this->LD(this->a, value);
        } break;
    }

    /*
    
        LD 16 instructions, LD rr, n16 (0x01, 0x11, 0x21, 0x31)
    
    */

    if(high <= 0x3) switch(low) {
        case 0x1: {
            const uint8_t& r4 = (uint8_t&) this->gameboy->memory->fetch8(this->pc + 1);
            const uint8_t& r3 = (uint8_t&) this->gameboy->memory->fetch8(this->pc + 2);
            this->pc += 3;

            logger->log("LD rr, n16 with n16: " + intToHex((uint16_t) (r3 << 8) + r4));

            if(high == 0x0) return this->LD(this->b, this->c, r3, r4);
            else if(high == 0x1) return this->LD(this->d, this->e, r3, r4);
            else if(high == 0x2) return this->LD(this->h, this->l, r3, r4);
            else if(high == 0x3) return this->LD(this->sp, r3, r4);
        } break;
    }

    /*
    
        Switch case for ADD, ADC, SUB, SBC, AND, XOR, OR, CP on: r r or r [HL] (lines 0x80 to 0xBF)
    
    */

    if(high >= 0x8 && high <= 0xB) {
        this->pc ++;

        // Get operand
        const uint8_t& r2 = this->getArith8Operand(low - (low <= 0x7 ? 0 : 0x8));

        switch(high) {
            case 0x8: {
                if(low <= 0x7) { // ADD A, r
                    logger->log("ADD A, r with r: " + intToHex(r2));
                    return this->ADD(this->a, r2);
                } else { // ADC A, r
                    logger->log("ADC A, r with r: " + intToHex(r2));
                    return this->ADDC(this->a, r2);
                }
            } break;

            case 0x9: {
                if(low <= 0x7) { // SUB A, r
                    logger->log("SUB A, r with r: " + intToHex(r2));
                    return this->SUB(this->a, r2);
                } else { // SBC A, r
                    logger->log("SBC A, r with r: " + intToHex(r2));
                    return this->SUBC(this->a, r2);
                }
            } break;

            case 0xA: {
                if(low <= 0x7) {// AND A, r
                    logger->log("AND A, r with r: " + intToHex(r2));
                    return this->AND(this->a, r2);
                } else { // XOR A, r
                    logger->log("XOR A, r with r: " + intToHex(r2));
                    return this->XOR(this->a, r2);
                }
            } break;

            case 0xB: {
                if(low <= 0x7) { // OR A, r
                    logger->log("OR A, r with r: " + intToHex(r2));
                    return this->OR(this->a, r2);
                } else { // CP A, r
                    logger->log("CP A, r with r: " + intToHex(r2));
                    return this->CP(this->a, r2);
                }
            } break;
        }
    }

    /*
    
        Switch case for INC, DEC on: r or [HL] (columns 0x03, 0x04, 0x05, 0x0B, 0x0C, 0x0D)
    
    */

    if(high <= 0x3) {
        switch(low) {
            case 0x3: {
                logger->log("INC rr");
                return this->INC((uint8_t&) this->getIncDec8Operand(high), (uint8_t&) this->getIncDec8Operand(high + 0x4));
            } break;

            case 0x4: {
                logger->log("INC r");
                return this->INC((uint8_t&) this->getIncDec8Operand(high));
            } break;

            case 0x5: {
                logger->log("DEC r");
                return this->DEC((uint8_t&) this->getIncDec8Operand(high));
            } break;

            case 0xB: {
                logger->log("DEC rr");
                return this->DEC((uint8_t&) this->getIncDec8Operand(high), (uint8_t&) this->getIncDec8Operand(high + 0x4));
            } break;

            case 0xC: {
                logger->log("INC r");
                return this->INC((uint8_t&) this->getIncDec8Operand(high + 0x4));
            } break;

            case 0xD: {
                logger->log("DEC r");
                return this->DEC((uint8_t&) this->getIncDec8Operand(high + 0x4));
            } break;
        }
    }

    /*
    
        Other instructions
    
    */

    switch(opcode) {
        /*
            0x0x instructions
        */

        case 0x00: {
            logger->log("NOP");
            this->pc++;

            return this->gameboy->stop();
        } break;

        /*
            0x1x instructions
        */

        case 0x10: { // STOP n8
            logger->log("STOP");
            return this->gameboy->stop();
        } break;

        case 0x17: { // RLA
            logger->log("RLA");
            this->pc++;

            return this->RLA();
        } break;

        case 0x18: { // JR e8
            const int8_t& e8 = (int8_t&) this->gameboy->memory->fetch8(this->pc + 1);
            logger->log("JR e8 with value " + intToHex(e8));

            this->pc += 2 + e8;
            return;
        } break;

        /*
            0x2x instructions
        */

        case 0x20: { // JR NZ, e8
            const int8_t& e8 = (int8_t&) this->gameboy->memory->fetch8(this->pc + 1);
            logger->log("JR NZ, e8 with value " + intToHex(e8));

            this->pc += 2;
            if(this->JRN(e8, this->getZero())) return;
            else return;
        } break;

        case 0x28: { // JR Z, e8
            const int8_t& e8 = (int8_t&) this->gameboy->memory->fetch8(this->pc + 1);
            logger->log("JR Z, e8 with value " + intToHex(e8));

            this->pc += 2;
            if(this->JRS(e8, this->getZero())) return;
            else return;
        } break;

        /*
            0x3x instructions
        */

        case 0x30: { // JR NC, e8
            const int8_t& e8 = (int8_t&) this->gameboy->memory->fetch8(this->pc + 1);
            logger->log("JR NC, e8 with value " + intToHex(e8));

            this->pc += 2;
            if(this->JRN(e8, this->getCarry())) return;
            else return;
        } break;

        /*
            0xCx instructions
        */

        case 0xC1: { // POP BC
            logger->log("POP BC");

            this->pc++;
            return this->POP(this->b, this->c);
        } break;

        case 0xC5: { // PUSH rr
            logger->log("PUSH rr");

            this->pc++;
            return this->PUSH(this->b, this->c);
        } break;

        case 0xC6: { // ADD A, n8
            const uint8_t& value = (uint8_t&) this->gameboy->memory->fetch8(this->pc + 1);
            logger->log("ADD A, n8 with value " + intToHex(value));

            this->pc += 2;
            return this->ADD(this->a, value);
        } break;

        case 0xC9: { // RET
            logger->log("RET");

            this->pc++;
            return this->RET();
        } break;

        case 0xCD: { // CALL n16
            const uint8_t& adr_lsb = (uint8_t&) this->gameboy->memory->fetch8(this->pc + 1);
            const uint8_t& adr_msb = (uint8_t&) this->gameboy->memory->fetch8(this->pc + 2);
            const uint16_t adress = ((uint16_t) adr_msb << 8) + adr_lsb;

            logger->log("CALL n16 with adress " + intToHex(adress));

            this->pc += 3;
            return this->CALL(adress);
        } break;

        case 0xCE: { // ADC A, n8
            const uint8_t& value = (uint8_t&) this->gameboy->memory->fetch8(this->pc + 1);
            logger->log("ADC A, n8 with value " + intToHex(value));

            this->pc += 2;
            return this->ADDC(this->a, value);
        } break;

        /*
            0xDx instructions
        */

        case 0xD6: { // SUB A, n8
            const uint8_t& value = (uint8_t&) this->gameboy->memory->fetch8(this->pc + 1);
            logger->log("SUB A, n8 with value " + intToHex(value));

            this->pc += 2;
            return this->SUB(this->a, value);
        } break;

        case 0xDE: { // SBC A, n8
            const uint8_t& value = (uint8_t&) this->gameboy->memory->fetch8(this->pc + 1);
            logger->log("SBC A, n8 with value " + intToHex(value));

            this->pc += 2;
            return this->SUBC(this->a, value);
        } break;

        /*
            0xEx instructions
        */

        case 0xE0: { // LD [FF00 + n8], A
            const uint8_t& value = (uint8_t&) this->gameboy->memory->fetch8(this->pc + 1);
            logger->log("LD [FF00 + n8], A with n8 value " + intToHex(value));

            this->pc += 2;
            return this->LD((uint8_t&) this->gameboy->memory->fetch8(0xFF00 + value), this->a);
        } break;

        case 0xE2: { // LD [FF00 + C], A
            logger->log("LD [FF00 + C], A with C value " + intToHex(this->c) + ", A: " + intToHex(this->a));

            this->pc++;
            return this->LD((uint8_t&) this->gameboy->memory->fetch8(0xFF00 + this->c), this->a);
        } break;

        case 0xE5: { // PUSH rr
            logger->log("PUSH rr");

            this->pc++;
            return this->PUSH(this->h, this->l);
        } break;

        case 0xE6: { // AND A, n8
            const uint8_t& value = (uint8_t&) this->gameboy->memory->fetch8(this->pc + 1);
            logger->log("AND A, n8 with value " + intToHex(value));

            this->pc += 2;
            return this->AND(this->a, value);
        } break;

        case 0xEA: { // LD [adr], A
            const uint8_t& adr_lsb = (uint8_t&) this->gameboy->memory->fetch8(this->pc + 1);
            const uint8_t& adr_msb = (uint8_t&) this->gameboy->memory->fetch8(this->pc + 2);
            const uint16_t adress = ((uint16_t) adr_msb << 8) + adr_lsb;

            logger->log("LD [adr], A with adress " + intToHex(adress));

            this->pc += 3;
            return this->LD((uint8_t&) this->gameboy->memory->fetch8(adress), this->a);
        } break;

        case 0xEE: { // XOR A, n8
            const uint8_t& value = (uint8_t&) this->gameboy->memory->fetch8(this->pc + 1);
            logger->log("XOR A, n8 with value " + intToHex(value));

            this->pc += 2;
            return this->XOR(this->a, value);
        } break;

        /*
            0xFx instructions
        */

        case 0xF0: { // LD A, [FF00 + a8]
            const uint8_t& value = (uint8_t&) this->gameboy->memory->fetch8(this->pc + 1);
            logger->log("LD A, [FF00 + a8] with value " + intToHex(value));

            this->pc += 2;
            return this->LD(this->a, this->gameboy->memory->fetch8(0xFF00 + value));
        } break;

        case 0xF2: { // LD A, [FF00 + C]
            logger->log("LD A, [FF00 + C] with C value " + intToHex(this->c) + ", A: " + intToHex(this->a));

            this->pc++;
            return this->LD(this->a, this->gameboy->memory->fetch8(0xFF00 + this->c));
        } break;

        case 0xF6: { // OR A, n8
            const uint8_t& value = (uint8_t&) this->gameboy->memory->fetch8(this->pc + 1);
            logger->log("OR A, n8 with value " + intToHex(value));

            this->pc += 2;
            return this->OR(this->a, value);
        } break;

        case 0xFA: { // LD A [adr]
            const uint8_t& adr_lsb = (uint8_t&) this->gameboy->memory->fetch8(this->pc + 1);
            const uint8_t& adr_msb = (uint8_t&) this->gameboy->memory->fetch8(this->pc + 2);
            const uint16_t adress = ((uint16_t) adr_msb << 8) + adr_lsb;

            logger->log("LD A, [adr] with adress " + intToHex(adress));

            this->pc += 3;
            return this->LD(this->a, this->gameboy->memory->fetch8(adress));
        } break;

        case 0xFE: { // CP A, n8
            const uint8_t& value = (uint8_t&) this->gameboy->memory->fetch8(this->pc + 1);
            logger->log("CP A, n8 with n value " + intToHex(value) + ", A: " + intToHex(this->a));

            this->pc += 2;
            return this->CP(this->a, value);
        } break;
    }

    /*
    
        Switch case for custom instructions, DUMPR, DUMPW, DUMPV (0xE3, 0xE4, 0xEB, 0xEC, 0xED)
    
    */

    switch(high) {
        case 0xE: {
            if(low == 0xB) {
                this->pc++;
                return this->DUMPR();
            } else if(low == 0xC) {
                // Read PC + 1 and PC + 2 to get the adress
                const uint8_t& r2 = (uint8_t&) this->gameboy->memory->fetch8(this->pc + 1);
                const uint8_t& r3 = (uint8_t&) this->gameboy->memory->fetch8(this->pc + 2);

                const uint16_t adress = ((uint16_t) r2 << 8) + r3;

                this->pc += 3;
                return logger->log("\033[34mAdress: " + intToHex(adress) + " Value: " + intToHex((uint8_t&) this->gameboy->memory->fetch8(adress)) + "\033[0m");
            }
        } break;
    }

    logger->error("Unknown opcode: " + intToHex(opcode));
    this->gameboy->stop();
}

/*

    Decode and execute prefixed instructions

*/

void CPU::decodeAndExecutePrefixed(const uint8_t& opcode) {
    *logger << "Decoding prefixed opcode: " + intToHex(opcode) + ", PC: " + intToHex(this->pc);

    const uint8_t high = opcode >> 4;
    const uint8_t low = opcode & 0xF;

    if(high >= 0x4 && high <= 0x7) {
        this->pc ++;
        const uint8_t& r = this->getArith8Operand(low - (low <= 0x7 ? 0 : 0x8));
        
        if(low <= 0x7) {
            logger->log("BIT " + to_string((high - 0x4)) + ", r with r: " + intToHex(r));
            return this->BIT(high - 0x4, r);
        } else {
            logger->log("BIT " + to_string((high - 3)) + ", r with r: " + intToHex(r));
            return this->BIT(high - 0x3, r);
        }
    }
    
    if(high == 0x1) {
        if(low <= 0x7) { // RL r
            uint8_t& r = this->getArith8Operand(low);
            logger->log("RL r with r: " + intToHex(r));

            this->pc ++;
            return this->RL(r);
        }
    }

    logger->error("Unknown prefiexed opcode: " + intToHex(opcode));
    this->gameboy->stop();
}

/*

    Util to select the r operand for ADD, ADC, SUB, SBC, AND, XOR, OR, CP

*/

uint8_t& CPU::getArith8Operand(const uint8_t& opcode) {
    switch(opcode) {
        case 0x0: return this->b;
        case 0x1: return this->c;
        case 0x2: return this->d;
        case 0x3: return this->e;
        case 0x4: return this->h;
        case 0x5: return this->l;

        case 0x6: {
            const uint16_t adress = ((uint16_t) this->h << 8) + this->l;
            return (uint8_t&) this->gameboy->memory->fetch8(adress);
        }

        case 0x7: return this->a;

        default: {
            logger->error("Unknown operand: " + intToHex(opcode));
            this->gameboy->stop();

            return this->a; // Return default value
        }
    }
}

/*

    Util to select the r operand for INC, DEC

*/

uint8_t& CPU::getIncDec8Operand(const uint8_t& opcode) {
    switch(opcode) {
        case 0x0: return this->b;
        case 0x1: return this->d;
        case 0x2: return this->h;

        case 0x3: {
            const uint16_t adress = ((uint16_t) this->h << 8) + this->l;
            return (uint8_t&) this->gameboy->memory->fetch8(adress);
        }

        case 0x4: return this->c;
        case 0x5: return this->e;
        case 0x6: return this->l;
        case 0x7: return this->a;

        default: {
            logger->error("Unknown operand: " + intToHex(opcode));
            this->gameboy->stop();

            return this->a; // Return default value
        }
    }
}

/*

    JUMP if and if not flag

*/

bool CPU::JRN(const int8_t& e8, const uint8_t& flag) { // 0x20, 0x30 -> jump to pc + e8 if z flag, c flag RESET respectively
    if(flag == 0) {
        this->pc += e8;
        return true;
    } else return false;    
}

bool CPU::JRS(const int8_t& e8, const uint8_t& flag) { // 0x18, 0x28 -> jump to pc + e8 if z flag, c flag SET respectively
    if(flag == 1) {
        this->pc += e8;
        return true;
    } else return false;
}

/*

    LD 8 bits, 16 bits

*/

void CPU::LD(uint8_t &r1, const uint8_t &r2) {
    r1 = r2;
}

/*

    ADD, 8 bits, 8 bits carry, 16 bits

*/

void CPU::ADD(uint8_t &r1, const uint8_t &r2) {
    // Add r2 to r1
    const uint8_t result = r1 + r2;
    r1 = result;

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
    const uint8_t result = r1 + r2 + this->getCarry();
    r1 = result;

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
    const uint16_t result = ((uint16_t) r1 << 8) + r2 + ((uint16_t) r3 << 8) + r4;

    r1 = result >> 8;
    r2 = result & 0xFF;

    this->resetSub();

    if(halfCarryOnAddition(result, ((uint16_t) r3 << 8) + r4)) this->setHalfCarry();
    else this->resetHalfCarry();

    if(r1 < r2) this->setCarry();
    else this->resetCarry();
}

/*

    SUB 8 bits, 8 bits carry

*/

void CPU::SUB(uint8_t &r1, const uint8_t &r2) {
    // Subtract r2 from r1
    const uint8_t result = r1 - r2;
    r1 = result;

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
    const uint8_t result = r1 - r2 - this->getCarry();
    r1 = result;

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

void CPU::CP(const uint8_t &r1, const uint8_t &r2) {
    // Compare r1 with r2
    const uint8_t result = r1 - r2;

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
    const uint8_t result = r1 + 1;
    r1 = result;

    this->resetSub();

    if(r1 == 0) this->setZero();
    else this->resetZero();

    if(halfCarryOnAddition(r1, 1)) this->setHalfCarry();
    else this->resetHalfCarry();

    this->pc ++;
}

void CPU::INC(uint8_t &r1, uint8_t &r2) {
    // Increment r1
    const uint16_t result = ((uint16_t) r1 << 8) + r2 + 1;

    r1 = result >> 8;
    r2 = result & 0xFF;

    this->pc ++;
}

/*

    DEC 8 bits, 16 bits

*/

void CPU::DEC(uint8_t &r1) {
    // Decrement r1
    const uint8_t result = r1 - 1;
    r1 = result;

    this->setSub();

    if(r1 == 0) this->setZero();
    else this->resetZero();

    if(halfCarryOnSubtration(r1, 1)) this->setHalfCarry();
    else this->resetHalfCarry();

    this->pc ++;
}

void CPU::DEC(uint8_t &r1, uint8_t &r2) {
    // Decrement r1
    const uint16_t result = ((uint16_t) r1 << 8) + r2 - 1;

    r1 = result >> 8;
    r2 = result & 0xFF;

    this->pc ++;
}

/*

    AND bitwise 8 bits

*/

void CPU::AND(uint8_t &r1, const uint8_t &r2) {
    // And r1 with r2
    const uint8_t result = r1 & r2;
    r1 = result;

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
    const uint8_t result = r1 | r2;
    r1 = result;

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
    const uint8_t result = r1 ^ r2;
    r1 = result;

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

    this->resetHalfCarry();

    if(this->a == 0) this->setZero();
    else this->resetZero();
}

/*

    CPL complement accumulator

*/

void CPU::CPL() {
    this->a = ~this->a;

    this->setSub();
    this->setHalfCarry();
}

/*

    LD

*/

void CPU::LD(uint16_t &r1, const uint8_t &r3, const uint8_t &r4) { // 0x01, 0x11, 0x21, 0x31 ie load immediate 16 bit value into BC, DE, HL, SP respectivement
    r1 = ((uint16_t) r3 << 8) + r4;
}

void CPU::LD(uint8_t& r1, uint8_t& r2, const uint8_t& r3, const uint8_t& r4) { // 0x01, 0x11, 0x21, 0x31 ie load immediate 16 bit value into BC, DE, HL, SP respectivement
    r1 = r3;
    r2 = r4;
}

/*

    Bit

*/

void CPU::BIT(const uint8_t &bit, const uint8_t &r) {
    // Check if bit is set
    if((r & (1 << bit)) == 0) this->setZero();
    else this->resetZero();

    this->resetSub();
    this->setHalfCarry();
}

/*

    Push

*/

void CPU::PUSH(const uint8_t &r1, const uint8_t &r2) {
    // Push r1 and r2 onto the stack
    this->sp--;
    uint8_t& m1 = (uint8_t&) this->gameboy->memory->fetch8(this->sp);
    m1 = r1;

    this->sp--;
    uint8_t& m2 = (uint8_t&) this->gameboy->memory->fetch8(this->sp);
    m2 = r2;
}

/*

    CALL

*/

void CPU::CALL(const uint16_t &adr) {
    this->PUSH(this->pc >> 8, this->pc & 0xFF);
    this->pc = adr;
}

/*

    RET

*/

void CPU::RET() {
    const uint8_t& m1 = (uint8_t&) this->gameboy->memory->fetch8(this->sp);
    this->sp++;

    const uint8_t& m2 = (uint8_t&) this->gameboy->memory->fetch8(this->sp);
    this->sp++;

    this->pc = ((uint16_t) m2 << 8) + m1;
}

/*

    RL

*/

void CPU::RL(uint8_t &r) {
    // Rotate r left
    const uint8_t carry = this->getCarry();
    this->resetCarry();

    if(r & 0x80) this->setCarry();

    r = (r << 1) + carry;
}

/*

    POP

*/

void CPU::POP(uint8_t &r1, uint8_t &r2) {
    const uint8_t& m1 = (uint8_t&) this->gameboy->memory->fetch8(this->sp);
    this->sp++;

    const uint8_t& m2 = (uint8_t&) this->gameboy->memory->fetch8(this->sp);
    this->sp++;

    r1 = m1;
    r2 = m2;
}

/*

    RLA

*/

void CPU::RLA() {
    // Rotate a left
    const uint8_t carry = this->getCarry();
    this->resetCarry();

    if(this->a & 0x80) this->setCarry();

    this->a = (this->a << 1) + carry;
}

/*

    DUMP

*/

void CPU::DUMPR() {
    // Dump registers
    logger->log("\033[34mDumping registers\033[0m");
    logger->log("\033[34mA: " + intToHex(this->a) + " F: " + intToHex(this->f) + " B: " + intToHex(this->b) + " C: " + intToHex(this->c) + " D: " + intToHex(this->d) + " E: " + intToHex(this->e) + " H: " + intToHex(this->h) + " L: " + intToHex(this->l) + " SP: " + intToHex(this->sp) + " PC: " + intToHex(this->pc) + "\033[0m");
}

void CPU::DUMPW() {
    // Dump work RAM
    logger->log("Dumping work RAM");

    // TODO
}

void CPU::DUMPV() {
    // Dump video RAM
    logger->log("Dumping video RAM");

    // TODO
}

/*

    Interrupts

*/

void CPU::enableInterrupt(const Interrupt interrupt) {
    this->gameboy->memory->fetch8(0xFFFF) |= (uint8_t) interrupt;
}

void CPU::disableInterrupt(const Interrupt interrupt) {
    this->gameboy->memory->fetch8(0xFFFF) &= ~(uint8_t) interrupt;
}

void CPU::triggerInterrupt(const Interrupt interrupt) {
    this->gameboy->memory->fetch8(0xFF0F) |= (uint8_t) interrupt;

    *logger << "Triggering interrupt: " + intToHex(interrupt) + ", PC: " + intToHex(this->pc);
    this->gameboy->pause();
}

void CPU::clearInterrupt(const Interrupt interrupt) {
    this->gameboy->memory->fetch8(0xFF0F) &= ~(uint8_t) interrupt;
}