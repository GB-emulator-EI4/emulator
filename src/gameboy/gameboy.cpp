#include <iostream>
#include <string>

using namespace std;

#include "gameboy.hpp"

#include "cpu/cpu.hpp"
#include "memory/memory.hpp"
#include "logger/logger.hpp"

/*

    Constructors and Destructors

*/

Gameboy::Gameboy(const string &bootromPath) : running(true), CPU(), Memory(bootromPath), Logger() {
    std::cout << "Gameboy Constructor" << std::endl;
}

Gameboy::~Gameboy() {
    // Do nothing
}

/*

    Functions

*/

void Gameboy::run() {
    std::cout << "Gameboy Running" << std::endl;

    while(this->running) {
        CPU::cycle();
    }
}