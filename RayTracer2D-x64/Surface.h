#pragma once
#include "olcPixelGameEngine.h"
#include "Segment.h"
#include "Math.h"


enum class SurfaceType { REFLECTIVE, REFRACTIVE };

struct Surface : Segment
{
	Surface();
	Surface(olc::vf2d point1, olc::vf2d point2);
	Surface(olc::vf2d point1, olc::vf2d point2, bool isReflective, bool isRefractive, float refractiveIndex);
	Surface(olc::vf2d point1, olc::vf2d point2, SurfaceType surfaceType);
	Surface(olc::vf2d point1, olc::vf2d point2, SurfaceType surfaceType, float refractiveIndex);


	float refractive_index;
	bool is_reflective;
	bool is_refractive;

	float extension;
};