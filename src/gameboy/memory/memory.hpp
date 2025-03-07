#pragma once

#include <string>

using namespace std;

// Memory block sizes
#define BOOTROM_SIZE 256

#define ROM_FIXED_SIZE 16384
#define ROM_BANKED_SIZE 16384

#define VRAM_SIZE 8192

#define EXTRAM_SIZE 8192

#define WRAM_FIXED_SIZE 4096
#define WRAM_BANKED_SIZE 4096

#define OAM_SIZE 160

#define HRAM_SIZE 128

// Memory block offsets
#define BOOTROM_OFFSET 0x0000 // 0x0000 - 0x00FF, boot ROM overlaps with the first 256 bytes of the fixed ROM

#define ROM_FIXED_OFFSET 0x0000 // First 256 bytes are ignored
#define ROM_BANKED_OFFSET 0x4000

#define VRAM_OFFSET 0x8000

#define EXTRAM_OFFSET 0xA000

#define WRAM_FIXED_OFFSET 0xC000
#define WRAM_BANKED_OFFSET 0xD000

#define OAM_OFFSET 0xFE00

#define HRAM_OFFSET 0xFF80

class Memory {
    public:
        Memory();
        ~Memory();

        // Memory read functions
        char& fetch8(const int &address); // Fetch 8-bit value from memory
        // int& fetch16(int &address); // Fetch 16-bit value from memory

        // ROM load functions
        void loadBootrom(const string &bootromPath); // Load the boot ROM

    private:
        char bootrom[BOOTROM_SIZE]; // 256B

        char romFixed[ROM_FIXED_SIZE]; // 16KB
        char romBanked[ROM_BANKED_SIZE]; // 16KB

        char vram[VRAM_SIZE]; // 8KB

        char extram[EXTRAM_SIZE]; // 8KB

        char wramFixed[WRAM_FIXED_SIZE]; // 4KB
        char wramBanked[WRAM_BANKED_SIZE]; // 4KB

        char oam[OAM_SIZE]; // 160B

        char hram[HRAM_SIZE]; // 128B

        // Init functions
        // void loadROM(); // Load the ROM
        // void loadROMFixed(); // Load the fixed ROM
        // void loadROMBanked(); // Load the banked ROM
};