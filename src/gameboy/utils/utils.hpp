#pragma once

#include <iostream>
#include <string>
#include <filesystem>

using namespace std;

/*

    Convert an integer to a hexadecimal string

*/

template <typename T>
string intToHex(T i) {
    stringstream stream;
    stream << uppercase << hex << setw(sizeof(T)) << setfill('0') << (uint16_t) i;

    return "0x" + stream.str();
}

/*

    Check if carry and half carry occured on addition

*/

inline bool halfCarryOnAddition(uint8_t v1, uint8_t v2) { return (((v1 & 0x0F) + (v2 & 0x0F)) & 0x10) == 0x10; };
inline bool halfCarryOnAddition(uint16_t v1, uint16_t v2) { return (((v1 & 0x00FF) + (v2 & 0x00FF)) & 0x0100) == 0x0100; };
inline bool halfCarryOnSubtration(uint8_t v1, uint8_t v2) { return ((int) (v1 & 0x0F) - (int) (v2 & 0x0F)) < 0; };
inline bool halfCarryOnSubtration(uint16_t v1, uint16_t v2) { return ((int) (v1 & 0x00FF) - (int) (v2 & 0x00FF)) < 0; };