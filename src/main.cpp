#include <iostream>

using namespace std;

#include "gameboy/gameboy.hpp"

// Constants
#define BOOT_ROM_PATH "roms/boot/dmg0_boot.bin"

int main() {
    cout << "Starting Gameboy Emulator" << endl;

    Gameboy gameboy(BOOT_ROM_PATH);
    gameboy.run();

    return 0;
}