#pragma once
#include "olcPixelGameEngine.h"


struct Line
{
	Line();
	Line(olc::vd2d point1, olc::vd2d point2);

	olc::vd2d Normal(const olc::vd2d& lightRayDirection) const;

	bool operator == (const Line& other);
	bool operator != (const Line& other);

	olc::vd2d p1, p2;
};
