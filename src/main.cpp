#include <iostream>

using namespace std;

#include "constants/constants.hpp"

#include "gameboy/gameboy.hpp"

int main() {
    cout << "Starting Gameboy Emulator" << endl;

    // Get instance (will init the Gameboy)
    Gameboy* gameboy = Gameboy::getInstance();
    
    // Load ROMs
    gameboy->setBootRom(BOOT_ROM_PATH); 

    // Free everything
    delete gameboy;

    return 0;
}