#pragma once
#include "olcPixelGameEngine.h"


struct Line
{
	Line();
	Line(olc::vf2d point1, olc::vf2d point2);

	olc::vf2d Normal(const olc::vf2d& lightRayDirection) const;

	bool operator == (const Line& other);
	bool operator != (const Line& other);

	olc::vf2d p1, p2;
};
