#pragma once
#include "olcPixelGameEngine.h"
#include "Math.h"


constexpr int NUMBERS_PER_AVX_REGISTER = 8;

const float SURFACES_EXTENSION = 10 * EPSILON;


#define OPTIMIZED_COLLISION_INTERSECTIONS 1

#define SINGLE_THREAD_NO_AVX 1
#define MT_AVX 0