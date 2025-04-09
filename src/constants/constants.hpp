#pragma once

/*

    Boot ROM path

*/

#define BOOT_ROM_PATH "roms/boot/dmg_boot.bin"

// Enable / Disable boot ROM, used for testing
#define ENABLE_BOOT_ROM true

/*

    Game ROM path

*/

#define ROM_PATH "roms/games/Tetris (World) (Rev A).gb"
// #define ROM_PATH "roms/games/Tetris (World) (Rev A) (without credits).gb"

/*

    Test ROMs path, use this #define instead of GAME_ROM_PATH to run test ROMs

*/

// #define ROM_PATH "roms/tests/homemade/ADD/ADD r r.bin"

// LD
// #define ROM_PATH "roms/tests/homemade/LD/LD r n.bin" // OK
// #define ROM_PATH "roms/tests/homemade/LD/LD r r.bin" // OK
// #define ROM_PATH "roms/tests/homemade/LD/LD r [].bin" // OK
// #define ROM_PATH "roms/tests/homemade/LD/LD [] r.bin" // OK
// #define ROM_PATH "roms/tests/homemade/LD/LD [] n.bin" // OK
// #define ROM_PATH "roms/tests/homemade/LD/LD A [nn].bin" // OK
// #define ROM_PATH "roms/tests/homemade/LD/LD [nn] A.bin" // OK

// LDH
// #define ROM_PATH "roms/tests/homemade/LDH/LDH r (r).bin" // OK
// #define ROM_PATH "roms/tests/homemade/LDH/LDH (r) r.bin" // OK

/*

    Downloaded tests ROMs

*/

// #define ROM_PATH "roms/tests/downloaded/blargg/01-special.gb"
// #define ROM_PATH "roms/tests/downloaded/blargg/02-interrupts.gb"
// #define ROM_PATH "roms/tests/downloaded/blargg/03-op sp,hl.gb"
// #define ROM_PATH "roms/tests/downloaded/blargg/04-op r,imm.gb"
// #define ROM_PATH "roms/tests/downloaded/blargg/06-ld r,r.gb"

/*

    Rendering

*/

#define ENABLE_RENDERING true

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144

#define SCREEN_SCALE 5

/*

    Logging

*/

#define ENABLE_LOGGING false