#pragma once
#include "olcPixelGameEngine.h"
#include "Math.h"
#include "Ray.h"
#include "Line.h"
#include "Surface.h"


struct CollisionInfo
{
	CollisionInfo();
	CollisionInfo(bool intersect, bool coincide);

	bool intersect;
	bool coincide;
};

CollisionInfo LineVsLine(const Line& line1, const Line& line2, olc::vd2d& intersectionPoint);

CollisionInfo SurfaceVsSurface(const Surface& surface1, const Surface& surface2, olc::vd2d& intersectionPoint);

CollisionInfo SurfaceVsSurface(const Surface& surface1, const Surface& surface2);

CollisionInfo RayVsSurface(const Ray& ray, const Surface& surface, olc::vd2d& intersectionPoint);

bool PointVsRect(olc::vd2d point, olc::vd2d rectangle_position, olc::vd2d rectangle_size);