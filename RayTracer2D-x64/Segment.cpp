#include "Segment.h"


Segment::Segment()
{
	p1 = olc::vf2d(0, 0);
	p2 = olc::vf2d(1, 0);
}

Segment::Segment(olc::vf2d point1, olc::vf2d point2)
{
	p1 = point1;
	p2 = point2;
}

olc::vf2d Segment::Normal(const olc::vf2d& lightRayDirection) const
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

void Segment::Extend(float length)
{
	if (length == 0)
		return;

	olc::vf2d& p_min_x = (p1.x < p2.x) ? p1 : p2;
	olc::vf2d& p_max_x = (p1.x < p2.x) ? p2 : p1;
	olc::vf2d& p_min_y = (p1.y < p2.y) ? p1 : p2;
	olc::vf2d& p_max_y = (p1.y < p2.y) ? p2 : p1;

	float dx = p_max_x.x - p_min_x.x;
	float dy = p_max_y.y - p_min_y.y;
	float a = dy / dx;

	float surface_horizontal = Equal(a, 0);
	float surface_vertical = isinf(a);

	if (surface_horizontal)
	{
		p_min_x.x -= length;
		p_max_x.x += length;
	}
	else if (surface_vertical)
	{
		p_min_y.y -= length;
		p_max_y.y += length;
	}
	else
	{
		/*
			{ y = a*x
			{ x^2 + y^2 = r^2

			(a^2 + 1) * x^2 = r^2
			x = +-r * sqrt(1 / (a^2 + 1))
		*/
		dx = length * sqrt(1 / (a * a + 1));
		dy = a * dx;

		p_min_x.x -= dx;
		p_max_x.x += dx;
		p_min_y.y -= dy;
		p_max_y.y += dy;
	}
}

Segment Segment::Extended(float length) const
{
	Segment extended_surface = *this;

	extended_surface.Extend(length);

	return extended_surface;
}

bool Segment::ContainsPoint(olc::vf2d point)
{
	olc::vf2d& p_min_x = (p1.x < p2.x) ? p1 : p2;
	olc::vf2d& p_max_x = (p1.x < p2.x) ? p2 : p1;
	olc::vf2d& p_min_y = (p1.y < p2.y) ? p1 : p2;
	olc::vf2d& p_max_y = (p1.y < p2.y) ? p2 : p1;

	float dx = p_max_x.x - p_min_x.x;
	float dy = p_max_y.y - p_min_y.y;
	float a = dy / dx;

	float surface_horizontal = Equal(a, 0);
	float surface_vertical = isinf(a);

	if (surface_horizontal)
	{
		return (p_min_x.x <= point.x && point.x <= p_max_x.x) && Equal(point.y, p_min_x.y);
	}
	else if (surface_vertical)
	{
		return (p_min_y.x <= point.y && point.y <= p_max_y.y) && Equal(point.x, p_min_y.x);
	}
	else
	{
		float b = p_min_y.y - a * p_min_y.x;
		return (p_min_x.x <= point.x && point.x <= p_max_x.x) && Equal(a * point.x + b, point.y);
	}
}

bool Segment::IsContinuationOfAnotherSegment(const Segment& other) const
{
	bool has_common_point = p1 == other.p1 || p1 == other.p2 || p2 == other.p1 || p2 == other.p2;
	olc::vf2d direction1 = p2 - p1;
	olc::vf2d direction2 = other.p2 - other.p1;
	return has_common_point && Equal(direction1.cross(direction2), 0.0f);
}

bool Segment::operator == (const Segment& other)
{
	return (p1 == other.p1 && p2 == other.p2) || (p1 == other.p2 && p2 == other.p1);
}

bool Segment::operator != (const Segment& other)
{
	Segment& line = *this;
	return !(line == other);
}
