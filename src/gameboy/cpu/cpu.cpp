#include <iostream>

using namespace std;

#include "cpu.hpp"

/*

    Constructors and Destructors

*/

CPU::CPU() : a(0), f(0), b(0), c(0), d(0), e(0), h(0), l(0), sp(0), pc(0) {
    cout << "CPU Constructor" << endl;
}

CPU::~CPU() {
    // Do nothing
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
    cout << "CPU Fetch" << endl;

    // Fetch the next instruction
    char opcode = fetch8(this->pc);
}