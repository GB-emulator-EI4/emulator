#pragma once

/*

    Boot ROM path

*/

#define BOOT_ROM_PATH "roms/boot/dmg0_boot.bin"

// Enable / Disable boot ROM, used for testing
#define ENABLE_BOOT_ROM false

/*

    Game ROM path

*/

// #define ROM_PATH "roms/games/Tetris (World) (Rev A).gb"

/*

    Test ROMs path, use this #define instead of GAME_ROM_PATH to run test ROMs

*/

// #define ROM_PATH "roms/tests/homemade/ADD/ADD r r.bin"

// #define ROM_PATH "roms/tests/homemade/LD/LD r n8.bin"
// #define ROM_PATH "roms/tests/homemade/LD/LD r [].bin"
#define ROM_PATH "roms/tests/homemade/LD/LD [] r.bin"