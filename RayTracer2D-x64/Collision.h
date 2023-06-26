#pragma once
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

struct ScatterInfo
{
	ScatterInfo();
	ScatterInfo(bool reflected, bool refracted);

	bool reflected;
	bool refracted;
};

CollisionInfo LineVsLine(Line line1, Line line2, olc::vd2d& intersectionPoint);

CollisionInfo SurfaceVsSurface(const Surface& surface1, const Surface& surface2, olc::vd2d& intersectionPoint);

CollisionInfo SurfaceVsSurface(const Surface& surface1, const Surface& surface2);

CollisionInfo RayVsSurface(const Ray& ray, const Surface& surface, olc::vd2d& intersectionPoint);