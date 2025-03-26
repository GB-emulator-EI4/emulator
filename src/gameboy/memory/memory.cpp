#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <stdint.h>

using namespace std;

#include "../../constants/constants.hpp"
#include "../logging/logger/logger.hpp"
#include "../utils/utils.hpp"

#include "memory.hpp"

/*

    Constructors and Destructors

*/

Memory::Memory() : bootrom(), romFixed(), romBanked(), vram(), extram(), wramFixed(), wramBanked(), oam(), noRam(), io(), hram() {
    logger = Logger::getInstance()->getLogger("Memory");
    logger->log("Memory Constructor");
}

Memory::~Memory() {
    logger->log("Memory Destructor");
}

/*

    Functions

*/

void Memory::loadRom(const int &memoryBlock, const int &startAdress, const string &romPath, const int &memorySize) {
    // Open ROM file
    ifstream romFile(romPath, ios::binary);

    // Check if the file was opened successfully
    if(!romFile.is_open()) {
        logger->error("Error: Could not open ROM file : " + romPath); 
        exit(1);
    } else logger->log("ROM at path " + romPath + " opened successfully");

    // Determine size
    auto fileSize = std::filesystem::file_size(romPath);

    // Log
    logger->log("Loading ROM at address " + to_string(startAdress) + " with size " + to_string(fileSize));

    // Read ROM file
    if(memoryBlock == BOOTROM) romFile.read(this->bootrom, memorySize);
    else if(memoryBlock == ROM_FIXED) romFile.read(this->romFixed, memorySize);
    else if(memoryBlock == ROM_BANKED) romFile.read(this->romBanked, memorySize);
    else {
        logger->error("Error: Invalid memory block, loading ROM at address " + to_string(startAdress));
        exit(1);
    }

    // Close ROM file
    romFile.close();

    logger->log("ROM loaded successfully");
}

char& Memory::fetch8(const uint16_t &address) {
    // Check if the address is in the boot ROM
    if(ENABLE_BOOT_ROM && address < BOOTROM_OFFSET + BOOTROM_SIZE) return this->bootrom[address];

    // Check if the address is in the fixed ROM
    else if(address < ROM_FIXED_OFFSET + ROM_FIXED_SIZE) return this->romFixed[address - ROM_FIXED_OFFSET];

    // Check if the address is in the banked ROM
    else if(address >= ROM_BANKED_OFFSET && address < ROM_BANKED_OFFSET + ROM_BANKED_SIZE) return this->romBanked[address - ROM_BANKED_OFFSET];

    // Check if the address is in the VRAM
    else if(address >= VRAM_OFFSET && address < VRAM_OFFSET + VRAM_SIZE) return this->vram[address - VRAM_OFFSET];

    // Check if the address is in the EXTRAM
    else if(address >= EXTRAM_OFFSET && address < EXTRAM_OFFSET + EXTRAM_SIZE) return this->extram[address - EXTRAM_OFFSET];

    // Check if the address is in the fixed WRAM
    else if(address >= WRAM_FIXED_OFFSET && address < WRAM_FIXED_OFFSET + WRAM_FIXED_SIZE) return this->wramFixed[address - WRAM_FIXED_OFFSET];

    // Check if the address is in the banked WRAM
    else if(address >= WRAM_BANKED_OFFSET && address < WRAM_BANKED_OFFSET + WRAM_BANKED_SIZE) return this->wramBanked[address - WRAM_BANKED_OFFSET];

    // Check if the address is in the OAM
    else if(address >= OAM_OFFSET && address < OAM_OFFSET + OAM_SIZE) return this->oam[address - OAM_OFFSET];

    // Check if the address is in the IO
    else if(address >= IO_OFFSET && address < IO_OFFSET + IO_SIZE) return this->fetchIOs(address);

    else if(address >= NO_RAM_OFFSET && address < NO_RAM_OFFSET + NO_RAM_SIZE) {
        // Log warning if reading unusable memory
        logger->warning("Warning: Reading unusable memory at address " + intToHex(address));

        // Write 0xF to the address
        this->noRam[address - NO_RAM_OFFSET] = (char) 0xFF;

        // Return ref to adress, match expected behavior of this unmapped memory section
        return this->noRam[address - NO_RAM_OFFSET];
    }

    // Check if the address is in the HRAM
    else if(address >= HRAM_OFFSET && address < HRAM_OFFSET + HRAM_SIZE) {
        // Log warning if reading interrupts infos
        if(address == 0xFFFF) logger->warning("Warning: Reading interrupts infos at address " + intToHex(address));
        
        return this->hram[address - HRAM_OFFSET];
    }

    // Out of bounds
    else {
        // If the address is not in any of the memory blocks, throw an error
        logger->error("Error: Invalid memory address, reading at address " + intToHex(address));
        exit(123);
    }
}

char& Memory::fetchIOs(const uint16_t &address) {
    // TODO add better filter and log / warning for IOs reading, seprate log for controller, timers, lcd ...

    // Log warning if reading interrupts infos
    // if(address == 0xFF0F) logger->warning("Warning: Reading interrupts infos at address " + intToHex(address));

    // Log if reading LCD status
    // else if(address >= 0xFF40 && address <= 0xFF45) logger->log("Warning: Reading LCD infos at address " + intToHex(address));
    
    // Log warning if accessing other IOs
    // else logger->warning("Warning: Accessing IO at address " + intToHex(address));

    return this->io[address - IO_OFFSET];
}