#pragma once

#include <string>
#include <stdint.h>

using namespace std;

#include "../logging/log/log.hpp"

#include "../gameboy.hpp"

// Forward declaration
class Gameboy;

// Memory block
#define BOOTROM 0

#define ROM_FIXED 1
#define ROM_BANKED 2

#define VRAM 3

#define EXTRAM 4

#define WRAM_FIXED 5
#define WRAM_BANKED 6

#define OAM 7

#define IO 8

#define HRAM 9

// Memory block sizes
#define BOOTROM_SIZE 256

#define ROM_FIXED_SIZE 16384
#define ROM_BANKED_SIZE 16384

#define VRAM_SIZE 8192

#define EXTRAM_SIZE 8192

#define WRAM_FIXED_SIZE 4096
#define WRAM_BANKED_SIZE 4096

#define OAM_SIZE 160

#define IO_SIZE 128

#define HRAM_SIZE 128

// Memory block offsets
#define BOOTROM_OFFSET 0x0000 // 0x0000 - 0x00FF, boot ROM overlapps with the first 256 bytes of the fixed ROM

#define ROM_FIXED_OFFSET 0x0000 // First 256 bytes are ignored
#define ROM_BANKED_OFFSET 0x4000

#define VRAM_OFFSET 0x8000

#define EXTRAM_OFFSET 0xA000

#define WRAM_FIXED_OFFSET 0xC000
#define WRAM_BANKED_OFFSET 0xD000

#define OAM_OFFSET 0xFE00

#define IO_OFFSET 0xFF00

#define HRAM_OFFSET 0xFF80

class Memory {
    public:
        Memory();
        ~Memory();

        // Memory read function, fetch 8-bit value from memory
        char& fetch8(const uint16_t &address);

        // ROM load functions
        void loadRom(const int &memoryBlock, const int &startAdress, const string &bootromPath, const int &size);

    private:
        // Gameboy ref
        Gameboy* gameboy;

        Log* logger;

        char bootrom[BOOTROM_SIZE]; // 256B

        char romFixed[ROM_FIXED_SIZE]; // 16KB
        char romBanked[ROM_BANKED_SIZE]; // 16KB

        char vram[VRAM_SIZE]; // 8KB

        char extram[EXTRAM_SIZE]; // 8KB

        char wramFixed[WRAM_FIXED_SIZE]; // 4KB
        char wramBanked[WRAM_BANKED_SIZE]; // 4KB

        char oam[OAM_SIZE]; // 160B

        char io[IO_SIZE]; // 128B

        char hram[HRAM_SIZE]; // 128B

        char& fetchIOs(const uint16_t &address); // Fetch 8-bit value from IO memory
};