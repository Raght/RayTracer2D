#include "Line.h"


Line::Line()
{
	p1 = olc::vf2d(0, 0);
	p2 = olc::vf2d(1, 0);
}

Line::Line(olc::vf2d point1, olc::vf2d point2)
{
	p1 = point1;
	p2 = point2;
}

olc::vf2d Line::Normal(const olc::vf2d& lightRayDirection) const
{
	olc::vf2d line_vector = p1 - p2;

	olc::vf2d normal = { line_vector.y, -line_vector.x };

	if (normal.dot(lightRayDirection) > 0)
	{
		normal = { -line_vector.y, line_vector.x };
	}

	normal = normal.norm();

	return normal;
}

bool Line::operator == (const Line& other)
{
	return (p1 == other.p1 && p2 == other.p2) || (p1 == other.p2 && p2 == other.p1);
}

bool Line::operator != (const Line& other)
{
	Line& line = *this;
	return !(line == other);
}