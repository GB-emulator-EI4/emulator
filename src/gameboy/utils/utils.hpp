#pragma once

#include <string>
#include <stdint.h>

using namespace std;

string intToHex(uint8_t i);
string intToHex(uint16_t i);

string intToHex(int8_t i);

bool halfCarryOnAddition(uint8_t first_num, uint8_t second_num);
bool halfCarryOnAddition(uint16_t first_num, uint16_t second_num);
bool halfCarryOnSubtration(uint8_t first_num, uint8_t second_num);
bool halfCarryOnSubtration(uint16_t first_num, uint16_t second_num);