#include <stdint.h>
#include "Bits.h"


uint64_t double_as_uint64_t(double a) { return *(uint64_t*)&a; }
double uint64_t_as_double(uint64_t a) { return *(double*)&a; }
uint32_t float_as_uint32_t(float a) { return *(uint32_t*)&a; }
float uint32_t_as_float(uint32_t a) { return *(float*)&a; }