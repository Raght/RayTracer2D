#include "Surface.h"


Surface::Surface()
	: Line()
{
	is_reflective = false;
	is_refractive = !is_reflective;
	refractive_index = 2.5f;

	extension = 2 * EPSILON;
}

Surface::Surface(olc::vf2d point1, olc::vf2d point2)
	: Surface()
{
	p1 = point1;
	p2 = point2;
}

Surface::Surface(olc::vf2d point1, olc::vf2d point2, bool isReflective, bool isRefractive, float refractiveIndex)
	: Surface(point1, point2)
{
	is_reflective = isReflective;
	is_refractive = isRefractive;
	refractive_index = refractiveIndex;
}

Surface::Surface(olc::vf2d point1, olc::vf2d point2, SurfaceType surfaceType)
	: Surface(point1, point2)
{
	if (surfaceType == SurfaceType::REFLECTIVE)
	{
		is_reflective = true;
		is_refractive = false;
	}
	else if (surfaceType == SurfaceType::REFRACTIVE)
	{
		is_reflective = false;
		is_refractive = true;
	}
}

Surface::Surface(olc::vf2d point1, olc::vf2d point2, SurfaceType surfaceType, float refractiveIndex)
	: Surface(point1, point2, surfaceType)
{
	refractive_index = refractiveIndex;
}

void Surface::Extend(float length)
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

Surface Surface::Extended(float length) const
{
	Surface extended_surface = *this;

	extended_surface.Extend(length);
	
	return extended_surface;
}

bool Surface::ContainsPoint(olc::vf2d point)
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