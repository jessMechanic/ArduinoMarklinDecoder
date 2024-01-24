#ifndef MARKLINCONVERTER_H
#define MARKLINCONVERTER_H
#include <Arduino.h>

class MarklinConverterClass
{
public:
    uint16_t encode_speed_package(uint8_t adress, uint8_t speed, uint8_t direction);
    uint16_t encode_function_package(uint8_t adress, bool f1, bool f2, bool f3, bool f4);

private:
    uint8_t encode_address(uint8_t adress);
    uint8_t encode_speed(int8_t speed);
    uint8_t encode_part_speed(int8_t speed);
};
extern MarklinConverterClass MarklinConverter;
#endif // MARKLINCONVERTER_H