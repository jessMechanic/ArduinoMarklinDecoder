#include "MarklinConverter.h"
#include <HardwareSerial.h>
#include <Math.h>

#define fUNCTION1 0b1100
#define fUNCTION2 0b0010
#define fUNCTION3 0b0110
#define fUNCTION4 0b1110

#define SUBSPEEDUNDERN7 0b1010
#define SUBSPEEDUNDER1 0b1011
#define SUBSPEEDUNDER6 0b0101
#define SUBSPEEDUNDER14 0b0100

uint8_t tris[] = {0b0, 0b11, 0b10};

uint32_t MarklinConverterClass::encode_speed_package(uint8_t adress, int8_t speed)
{
    uint16_t result = 0;
    result = encode_address(adress) << 10;
    result |= encode_speed(speed);
    return result;
}

//                  |   f1    |   f2    |   f3    |   f4    |
//  ----------------------------------------------------------
//                  | E F G H | E F G H | E F G H | E F G H |
//  ----------------------------------------------------------
//  standard values | 1 1 0 f | 0 0 1 f | 0 1 1 f | 1 1 1 f |
//  ----------------------------------------------------------
uint32_t MarklinConverterClass::encode_function_update_package(uint8_t adress, uint8_t speed, uint8_t function_index, bool function_status)
{
    uint8_t function_part = 0;

    switch (function_index)
    {
    case 1:
    {
        function_part |= fUNCTION1 | (function_status ? 0b1 : 0);
        break;
    }
    case 2:
    {
        function_part |= fUNCTION2 | (function_status ? 0b1 : 0);
        break;
    }
    case 3:
    {
        function_part |= fUNCTION3 | (function_status ? 0b1 : 0);
        break;
    }
    case 4:
    {
        function_part |= fUNCTION4 | (function_status ? 0b1 : 0);
        break;
    }
    default:
        break;
    }
    Math.mix(function_part, encode_part_speed(speed));
    uint16_t result = 0;
    result = encode_address(adress) << 10;
    result |= function_part;

    return result;
}

uint8_t MarklinConverterClass::encode_address(uint8_t adress)
{
    int first = adress % 3;
    adress = adress / 3;

    int second = adress % 3;
    adress = adress / 3;

    int third = adress % 3;
    adress = adress / 3;

    int fourth = adress % 3;

    int result = tris[first] << 6 | tris[second] << 4 | tris[third] << 2 | tris[fourth];
    return result;
}

uint8_t MarklinConverterClass::encode_part_speed(int8_t speed)
{
    speed = speed % 15;
    speed++;

    speed = Math.reverse(speed);
    speed = speed >> 4;
    return speed;
}

//   operating level   |  E F G H
//  ------------------------------
//     -14 to  -7      |  1 0 1 0
//      -6 to  -0      |  1 0 1 1
//      +0 to  +6      |  0 1 0 1
//      +7 to +14      |  0 1 0 0

uint8_t MarklinConverterClass::encode_speed(int8_t speed)
{
    uint8_t sub_speed = 0;
    if (speed < -7)
    {
        sub_speed = SUBSPEEDUNDERN7;
    }
    else if (speed < -1)
    {
        sub_speed = SUBSPEEDUNDER1;
    }
    else if (speed < 6)
    {
        sub_speed = SUBSPEEDUNDER6;
    }
    else
    {
        sub_speed = SUBSPEEDUNDER14;
    }
    speed = encode_part_speed(speed);
    return Math.mix(speed, sub_speed);
}

MarklinConverterClass MarklinConverter;