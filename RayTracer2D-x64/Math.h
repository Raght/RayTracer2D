#pragma once
#include "olcPixelGameEngine.h"


inline double const PI = 3.141592653589793238462643383279;
inline double const EPSILON = 1e-6;

bool InRange(double number, double range_min, double range_max);

bool InRange2(double number, double middle, double range_span_from_middle);

bool ApproximatelyEqual(double number1, double number2);

bool Equal(double number1, double number2);

double Cap(double number, double minimum, double maximum);

int Cap(int number, int minimum, int maximum);

double Radians(double eulerAngle);

double AngleBetween(const olc::vd2d& v1, const olc::vd2d& v2);

olc::vd2d Rotate(const olc::vd2d& v, double radians);