#pragma once
#include "olcPixelGameEngine.h"


inline float const PI = 3.141592653589793238462643383279f;
inline float const EPSILON = 1e-5f;

bool InRange(float number, float range_min, float range_max);

bool InRange2(float number, float middle, float range_span_from_middle);

bool ApproximatelyEqual(float number1, float number2);

bool Equal(float number1, float number2);

float Cap(float number, float minimum, float maximum);

int Cap(int number, int minimum, int maximum);

float Radians(float eulerAngle);

float AngleBetween(const olc::vf2d& v1, const olc::vf2d& v2);

olc::vf2d Rotate(const olc::vf2d& v, float radians);