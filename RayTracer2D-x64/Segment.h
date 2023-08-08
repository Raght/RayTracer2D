#pragma once
#include "olcPixelGameEngine.h"
#include "Math.h"

struct Segment
{
	Segment();
	Segment(olc::vf2d point1, olc::vf2d point2);

	olc::vf2d Normal(const olc::vf2d& lightRayDirection) const;

	bool operator == (const Segment& other);
	bool operator != (const Segment& other);

	void Extend(float length);
	Segment Extended(float length) const;

	bool ContainsPoint(olc::vf2d point);

	bool IsContinuationOfAnotherSegment(const Segment& other) const;

	olc::vf2d p1, p2;
};
