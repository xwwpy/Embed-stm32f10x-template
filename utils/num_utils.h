#ifndef __NUM_UTILS_H__
#define __NUM_UTILS_H__

#include "math.h"
#include "stm32f10x.h"
#include "string.h"

char * convert_to_any_base_from_decimal(uint64_t decimal_number, uint8_t base);
uint64_t convert_to_decimal_from_any_base(char * number, uint8_t base);
char * convert_to_any_base_from_any_base(char * number, uint8_t from_base, uint8_t to_base);

#endif // !__NUM_UTILS_H__