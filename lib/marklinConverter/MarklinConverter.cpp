#include "MarklinConverter.h"
#include <HardwareSerial.h>
#include <Math.h>

uint16_t MarklinConverterClass::encode_speed_package(uint8_t adress, uint8_t speed, uint8_t direction)
{
    return 0;
}

uint16_t MarklinConverterClass::encode_function_package(uint8_t adress, bool f1, bool f2, bool f3, bool f4)
{

    return 0;
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

    int result = first << 6 | second << 4 | third << 2 | fourth;
}

//   operating level   |  E F G H
//  ------------------------------
//     -14 to  -7      |  1 0 1 0
//      -6 to  -0      |  1 0 1 1
//      +0 to  +6      |  0 1 0 1
//      +7 to +14      |  0 1 0 0

uint8_t MarklinConverterClass::encode_part_speed(int8_t speed)
{
    speed = speed % 15;
    speed++;

    speed = Math.reverse(speed);
    speed = speed >> 4;
    return speed;
}

uint8_t MarklinConverterClass::encode_speed(int8_t speed)
{
    uint8_t sub_speed = 0;
    if (speed < -7)
    {
        sub_speed = 0b1010;
    }
    else if (speed < -1)
    {
        sub_speed = 0b1011;
    }
    else if (speed < 6)
    {
        sub_speed = 0b0101;
    }
    else
    {
        sub_speed = 0b0100;
    }
    speed = encode_part_speed(speed);
    return Math.mix(speed, sub_speed);
}

MarklinConverterClass MarklinConverter;