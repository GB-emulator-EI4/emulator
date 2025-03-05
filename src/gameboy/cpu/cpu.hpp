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

        Log* logger;

        // Registers
        char a, f, b, c, d, e, h, l; // 8-bit registers
        int sp, pc; // 16-bit registers

        // Execution steps
        void fetch(); // Fetch the next instruction
        // void decode(); // Decode the instruction
        // void fetchOperands(); // Fetch the operands
        // void execute(); // Execute the instruction
};