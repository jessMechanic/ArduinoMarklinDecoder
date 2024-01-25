#ifndef MARKLINCONVERTER_H
#define MARKLINCONVERTER_H
#include <Arduino.h>
#include <Math.h>

class MarklinConverterClass
{
public:
    uint32_t encode_speed_package(uint8_t adress, int8_t speed);
    uint32_t encode_function_update_package(uint8_t adress, uint8_t speed, uint8_t function_index, bool f2);

private:
    uint8_t encode_address(uint8_t adress);
    uint8_t encode_speed(int8_t speed);
    uint8_t encode_part_speed(int8_t speed);
};
extern MarklinConverterClass MarklinConverter;
#endif // MARKLINCONVERTER_H