#include <iostream>
#include <string>
#include <fstream>

using namespace std;

#include "memory.hpp"

/*

    Constructors and Destructors

*/

Memory::Memory(const string &bootromPath) : bootrom{}, romFixed{}, romBanked{}, vram{}, extram{}, wramFixed{}, wramBanked{}, oam{}, hram{} {
    // Load boot ROM
    this->loadBootrom(bootromPath);
}

Memory::~Memory() {
    // Do nothing
}

/*

    Functions

*/

void Memory::loadBootrom(const string &bootromPath) {
    // Open boot ROM file
    ifstream bootromFile(bootromPath, ios::binary);

    // Check if the file was opened successfully
    if (!bootromFile.is_open()) {
        cout << "Error: Could not open boot ROM file" << endl;
        exit(1);
    }

    // Read boot ROM file
    bootromFile.read(this->bootrom, BOOTROM_SIZE);

    // Close boot ROM file
    bootromFile.close();
}

char& Memory::fetch8(const int &address) {
    // Check if the address is in the boot ROM
    if (address >= BOOTROM_OFFSET && address < ROM_FIXED_OFFSET) return this->bootrom[address];

    // Check if the address is in the fixed ROM
    if (address >= ROM_FIXED_OFFSET && address < ROM_BANKED_OFFSET) return this->romFixed[address - ROM_FIXED_OFFSET];

    // Check if the address is in the banked ROM
    if (address >= ROM_BANKED_OFFSET && address < VRAM_OFFSET) return this->romBanked[address - ROM_BANKED_OFFSET];

    // Check if the address is in the VRAM
    if (address >= VRAM_OFFSET && address < EXTRAM_OFFSET) return this->vram[address - VRAM_OFFSET];

    // Check if the address is in the EXTRAM
    if (address >= EXTRAM_OFFSET && address < WRAM_FIXED_OFFSET) return this->extram[address - EXTRAM_OFFSET];

    // Check if the address is in the fixed WRAM
    if (address >= WRAM_FIXED_OFFSET && address < WRAM_BANKED_OFFSET) return this->wramFixed[address - WRAM_FIXED_OFFSET];

    // Check if the address is in the banked WRAM
    if (address >= WRAM_BANKED_OFFSET && address < OAM_OFFSET) return this->wramBanked[address - WRAM_BANKED_OFFSET];

    // Check if the address is in the OAM
    if (address >= OAM_OFFSET && address < HRAM_OFFSET) return this->oam[address - OAM_OFFSET];

    // Check if the address is in the HRAM
    if (address >= HRAM_OFFSET) return this->hram[address - HRAM_OFFSET];

    // If the address is not in any of the memory blocks, throw an error
    cout << "Error: Invalid memory address, reading at address " << address << endl;
    exit(1);
}