#include "Math.h"

using namespace std;


bool InRange(float number, float range_min, float range_max)
{
	return range_min <= number && number <= range_max;
}

bool InRange2(float number, float middle, float range_span_from_middle)
{
	return middle - range_span_from_middle < number && number < middle + range_span_from_middle;
}

bool ApproximatelyEqual(float number1, float number2)
{
	return abs(number2 - number1) <= EPSILON;
}

bool Equal(float number1, float number2)
{
	return ApproximatelyEqual(number1, number2);
}

float Cap(float number, float minimum, float maximum)
{
	if (number < minimum)
	{
		return minimum;
	}
	else if (number > maximum)
	{
		return maximum;
	}
	return number;
}

int Cap(int number, int minimum, int maximum)
{
	if (number < minimum)
	{
		return minimum;
	}
	else if (number > maximum)
	{
		return maximum;
	}
	return number;
}

float Radians(float eulerAngle)
{
	return eulerAngle * PI / 180.0f;
}

float AngleBetween(const olc::vf2d& v1, const olc::vf2d& v2)
{
	return acosf((v1.dot(v2)) / (v1.mag() * v2.mag()));
}

olc::vf2d Rotate(const olc::vf2d& v, float radians)
{
	return { v.x * cos(radians) - v.y * sin(radians),
			 v.x * sin(radians) + v.y * cos(radians) };
}