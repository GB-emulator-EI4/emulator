#include <iostream>
#include <filesystem>
#include <iomanip>
#include <stdint.h>

using namespace std;

/*

    Convert an integer to a hexadecimal string

*/

string intToHex(uint8_t i) {
    stringstream stream;
    stream << uppercase << hex << setw(2) << setfill('0') << (uint16_t) i; // Keep the (uint16_t) cast to avoid i being interpreted as a char

    return "0x" + stream.str();
}

string intToHex(uint16_t i) {
    stringstream stream;
    stream << uppercase << hex << setw(4) << setfill('0') << i;

    return "0x" + stream.str();
}

/*

    Check if carry and half carry occured on addition

*/


bool halfCarryOnAddition(uint8_t first_num, uint8_t second_num) {
    return (((first_num & 0x0F) + (second_num & 0x0F)) & 0x10) == 0x10;
}

bool halfCarryOnAddition(uint16_t first_num, uint16_t second_num) {
    return (((first_num & 0x00FF) + (second_num & 0x00FF)) & 0x0100) == 0x0100;
}

bool halfCarryOnSubtration(uint8_t first_num, uint8_t second_num) {
    return ((int) (first_num & 0x0F) - (int) (second_num & 0x0F)) < 0;
}

bool halfCarryOnSubtration(uint16_t first_num, uint16_t second_num) {
    return ((int) (first_num & 0x00FF) - (int) (second_num & 0x00FF)) < 0;
}