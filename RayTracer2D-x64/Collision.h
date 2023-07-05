#pragma once
#include "olcPixelGameEngine.h"
#include "Math.h"
#include "Ray.h"
#include "Line.h"
#include "Surface.h"
#include "Constants.h"


struct CollisionInfo
{
	CollisionInfo();
	CollisionInfo(bool intersect, bool coincide);

	bool intersect;
	bool coincide;
};

CollisionInfo LineVsLine(const Line& line1, const Line& line2, olc::vf2d& intersectionPoint);

CollisionInfo SurfaceVsSurface(const Surface& surface1, const Surface& surface2, olc::vf2d& intersectionPoint);

CollisionInfo SurfaceVsSurface(const Surface& surface1, const Surface& surface2);

CollisionInfo RayVsSurface(const Ray& ray, const Surface& surface, olc::vf2d& intersectionPoint);

bool PointVsRect(olc::vf2d point, olc::vf2d rectangle_position, olc::vf2d rectangle_size);