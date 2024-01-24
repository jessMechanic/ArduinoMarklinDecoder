#ifndef MATH_H
#define MATH_H

#include <Arduino.h>

class MathClass
{
public:
    uint8_t reverse(uint8_t value);
    uint8_t mix(uint8_t abcd, uint8_t efgh);
};

extern MathClass Math;

#endif // MATH_H
