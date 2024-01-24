#include "Math.h"

uint8_t MathClass::reverse(uint8_t b)
{
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return b;
}

uint8_t MathClass::mix(uint8_t abcd, uint8_t efgh)
{
    uint8_t result = 0;
    result |= (abcd & 0b00000001) << 1;
    result |= (abcd & 0b00000010) << 2;
    result |= (abcd & 0b00000100) << 3;
    result |= (abcd & 0b00001000) << 4;

    result |= (efgh & 0b00000001);
    result |= (efgh & 0b00000010) << 1;
    result |= (efgh & 0b00000100) << 2;
    result |= (efgh & 0b00001000) << 3;
    return result;
}