#include "Math.h"

using namespace std;


bool InRange(double number, double range_min, double range_max)
{
	return range_min <= number && number <= range_max;
}

bool InRange2(double number, double middle, double range_span_from_middle)
{
	return middle - range_span_from_middle < number && number < middle + range_span_from_middle;
}

bool ApproximatelyEqual(double number1, double number2)
{
	return abs(number2 - number1) <= EPSILON;
}

bool Equal(double number1, double number2)
{
	return ApproximatelyEqual(number1, number2);
}

double Cap(double number, double minimum, double maximum)
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

double Radians(double eulerAngle)
{
	return eulerAngle * PI / 180.0f;
}

double AngleBetween(const olc::vd2d& v1, const olc::vd2d& v2)
{
	return acosf((v1.dot(v2)) / (v1.mag() * v2.mag()));
}

olc::vd2d Rotate(const olc::vd2d& v, double radians)
{
	return { v.x * cos(radians) - v.y * sin(radians),
			 v.x * sin(radians) + v.y * cos(radians) };
}