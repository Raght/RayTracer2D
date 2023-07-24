#pragma once
#include "olcPixelGameEngine.h"
#include "Math.h"


constexpr int NUMBERS_PER_AVX_REGISTER = 8;

const float SURFACES_EXTENSION = EPSILON;

#define MT_AVX 0
#define OPTIMIZED 1
#define OPTIMIZED_AVX 0