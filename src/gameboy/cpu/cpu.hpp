#pragma once

class CPU {
    public:
        CPU();
        ~CPU();

        void cycle(); // Will run a single cycle of the CPU, call in order the following functions: fetch, decode, fetchOperands, executes

    private:
        // Registers
        char a, f, b, c, d, e, h, l; // 8-bit registers
        int sp, pc; // 16-bit registers

        // Execution steps
        void fetch(); // Fetch the next instruction
        void decode(); // Decode the instruction
        void fetchOperands(); // Fetch the operands
        void execute(); // Execute the instruction

    protected:
        // Memory read functions
        virtual char& fetch8(int &address); // Fetch 8-bit value from memory
        virtual int& fetch16(int &address); // Fetch 16-bit value from memory
};