#pragma once
#include "olcPixelGameEngine.h"
#include "Math.h"
#include "Ray.h"
#include "Segment.h"
#include "Surface.h"
#include "Constants.h"


struct CollisionInfo
{
	CollisionInfo();
	CollisionInfo(bool intersect, bool coincide);

	bool intersect;
	bool coincide;
};

CollisionInfo LineVsLine(const Segment& line1, const Segment& line2, olc::vf2d& intersectionPoint);

CollisionInfo LineVsSurface(const Segment& line, const Surface& surface, olc::vf2d& intersectionPoint);

CollisionInfo SegmentVsSegment(const Segment& surface1, const Segment& surface2, olc::vf2d& intersectionPoint);

CollisionInfo SurfaceVsSurface(const Surface& surface1, const Surface& surface2, olc::vf2d& intersectionPoint);

CollisionInfo SurfaceVsSurface(const Surface& surface1, const Surface& surface2);

CollisionInfo RayVsSegment(const Ray& ray, const Segment& segment, olc::vf2d& intersectionPoint);

CollisionInfo RayVsSurface(const Ray& ray, const Surface& surface, olc::vf2d& intersectionPoint);

bool RayLineVsSegment(const Ray& ray, const Segment& surface);

bool RayVsSegment(const Ray& ray, const Segment& segment);

bool PointVsRect(olc::vf2d point, olc::vf2d rectangle_position, olc::vf2d rectangle_size);