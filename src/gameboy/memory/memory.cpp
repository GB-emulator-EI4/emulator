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

Memory::Memory(Gameboy* gameboy) : gameboy(gameboy), bootrom(), romFixed(), romBanked(), vram(), extram(), wramFixed(), wramBanked(), oam(), noRam(), io(), hram(), interruptEnable(0) {
    logger = Logger::getInstance()->getLogger("Memory");
    logger->log("Memory Constructor");

    this->preloadValues();
}

void Memory::preloadValues() {

}

Memory::~Memory() {
    logger->log("Memory Destructor");
}

/*

    Functions

*/

void Memory::loadRom(const int &memoryBlock, const string &romPath, const int readOffset, const int &memorySize) {
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
    logger->log("Loading ROM at address " + to_string(readOffset) + " with size " + to_string(fileSize));

    // Move file cursor to the read offset
    romFile.seekg(readOffset);

    // Read ROM file
    if(memoryBlock == BOOTROM) romFile.read(this->bootrom, memorySize);
    else if(memoryBlock == ROM_FIXED) romFile.read(this->romFixed, memorySize);
    else if(memoryBlock == ROM_BANKED) romFile.read(this->romBanked, memorySize);
    else {
        logger->error("Error: Invalid memory block, loading ROM at address " + to_string(memoryBlock));
        exit(1);
    }

    // Close ROM file
    romFile.close();

    logger->log("ROM loaded successfully");
}

char& Memory::fetch8(const uint16_t &address) {
    // Check if the address is in the boot ROM
    if(ENABLE_BOOT_ROM && address < BOOTROM_OFFSET + BOOTROM_SIZE) {
        // Read at address 0xFF50 to check if the boot ROM is disabled
        const char& bootRomEnabled = this->fetch8(0xFF50);
        if(bootRomEnabled == 0) return this->bootrom[address];
    }

    // Check if the address is in the fixed ROM
    if(address < ROM_FIXED_OFFSET + ROM_FIXED_SIZE) return this->romFixed[address - ROM_FIXED_OFFSET];

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

    else if(address >= ECHO_RAM_OFFSET && address < ECHO_RAM_OFFSET + ECHO_RAM_SIZE) return this->fetch8(address - ECHO_RAM_OFFSET + WRAM_FIXED_OFFSET);

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
    else if(address >= HRAM_OFFSET && address < HRAM_OFFSET + HRAM_SIZE) return this->hram[address - HRAM_OFFSET];

    // Log warning if reading interrupts infos
    if(address == 0xFFFF) {
        //logger->warning("Warning: Reading interrupts infos at address " + intToHex(address));
        return this->interruptEnable;
    }

    // Out of bounds
    else {
        // If the address is not in any of the memory blocks, throw an error
        logger->error("Error: Invalid memory address, reading at address " + intToHex(address));
        exit(123);
    }
}

char& Memory::fetchIOs(const uint16_t &address) {
    // Log reading joypad
    if(address == 0xFF00) {
        logger->log("Warning: Reading joypad at address " + intToHex(address));

        // Reset registrer to FF
        // this->io[0xFF00 - IO_OFFSET] = (char) 0xFF;

        // Key mapping:
        // up -> up arrow on qwerty
        // down -> down arrow on qwerty
        // left -> left arrow on qwerty
        // right -> right arrow on qwerty

        // a -> z on qwerty
        // b -> x on qwerty
        // start -> enter on qwerty
        // select -> s on qwerty

        // Check if bits 4 and 5 are set
        if((this->io[0xFF00 - IO_OFFSET] & 0x30) == 0x30) {
            // Set bits 0-3 to 1
            this->io[0xFF00 - IO_OFFSET] |= 0x0F;
        } else {
            const Uint8* keyStates = this->gameboy->renderer->getKeyStates();

            // If select buttons enable
            if((this->io[0xFF00 - IO_OFFSET] & 0x20) == 0) {
                // Set bits 0-3 to 1
                this->io[0xFF00 - IO_OFFSET] |= 0x0F;

                // Fill the register with the key states
                if(keyStates[SDL_SCANCODE_RETURN]) this->io[0xFF00 - IO_OFFSET] &= ~0x08; // Start
                if(keyStates[SDL_SCANCODE_S]) this->io[0xFF00 - IO_OFFSET] &= ~0x04; // Select
                if(keyStates[SDL_SCANCODE_X]) this->io[0xFF00 - IO_OFFSET] &= ~0x02; // B
                if(keyStates[SDL_SCANCODE_Z]) this->io[0xFF00 - IO_OFFSET] &= ~0x01; // A
            } else {
                // Set bits 0-3 to 1
                this->io[0xFF00 - IO_OFFSET] |= 0x0F;

                // Fill the register with the key states
                if(keyStates[SDL_SCANCODE_UP]) this->io[0xFF00 - IO_OFFSET] &= ~0x08; // Up
                if(keyStates[SDL_SCANCODE_DOWN]) this->io[0xFF00 - IO_OFFSET] &= ~0x04; // Down
                if(keyStates[SDL_SCANCODE_LEFT]) this->io[0xFF00 - IO_OFFSET] &= ~0x02; // Left
                if(keyStates[SDL_SCANCODE_RIGHT]) this->io[0xFF00 - IO_OFFSET] &= ~0x01; // Right
            }
        }
    }

    // just logging timer reg access and freq values
    else if(address - IO_OFFSET == DIVIDER_REGISTER) {
        logger->log("Reading divider register at addr " + intToHex(address));
        // logger->log("Divider Register Value: " + to_string(timer->getDividerRegister()));

        // TODO return ref to memory adress
        // Note: timer class should not hold the values of it's registers but rather read the memory adress asign to it each time it needs to use it.
    }

    else if(address - IO_OFFSET == TIMER_COUNTER) {
        logger->log("Reading timer counter at addr " + intToHex(address));
        // logger->log("Timer Counter Value: " + to_string(timer->getTimerCounter()));

        // TODO return ref to memory adress
    }

    else if(address - IO_OFFSET == TIMER_MODULO) {
        logger->log("Reading timer modulo at addr " + intToHex(address));
        // logger->log("Timer Modulo Value: " + to_string(timer->getTimerModulo()));

        // TODO return ref to memory adress
    }

    else if(address - IO_OFFSET == TIMER_CONTROL) {
        logger->log("Reading timer control at addr " + intToHex(address));
        // logger->log("Timer Control Value: " + to_string(timer->getTimerControl()));

        // TODO return ref to memory adress
    }

    // Log reading serial
    else if(address == 0xFF01 || address == 0xFF02) logger->log("Warning: Reading serial at address " + intToHex(address));

    // Log reading timer
    else if(address >= 0xFF04 && address <= 0xFF07) {
        logger->log("Warning: Reading timer at address " + intToHex(address));
        // this->gameboy->stop();
    }

    // Log warning if reading interrupts infos
    //else if(address == 0xFF0F) logger->warning("Warning: Reading interrupts infos at address " + intToHex(address));

    // Log reading sound
    else if(address >= 0xFF10 && address <= 0xFF3F) logger->log("Warning: Reading sound at address " + intToHex(address));

    // Log if reading LCD status
    else if(address >= 0xFF40 && address <= 0xFF4B) logger->log("Warning: Reading LCD status at address " + intToHex(address));

    // Log if reading to select VRAM bank
    else if(address == 0xFF4F) logger->log("Warning: Writing to select VRAM bank at address " + intToHex(address));

    // Log if reading to boot ROM enable
    else if(address == 0xFF50) logger->log("Warning: Writing to boot ROM enable at address " + intToHex(address));

    // Log if reading to VRAM DMA
    else if(address >= 0xFF51 && address <= 0xFF55) logger->log("Warning: Writing to VRAM DMA at address " + intToHex(address));

    // Log if reading to BG / OBJ palette
    else if(address >= 0xFF68 && address <= 0xFF6B) logger->log("Warning: Writing to BG / OBJ palette at address " + intToHex(address));    

    // Log if reading to WRAM bank select
    else if(address == 0xFF70) logger->log("Warning: Writing to WRAM bank select at address " + intToHex(address));
    
    // Log warning if accessing other IOs
    // else logger->warning("Warning: Accessing IO at address " + intToHex(address));

    return this->io[address - IO_OFFSET];
}