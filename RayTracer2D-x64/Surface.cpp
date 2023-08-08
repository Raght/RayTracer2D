#include "Surface.h"


Surface::Surface()
	: Segment()
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