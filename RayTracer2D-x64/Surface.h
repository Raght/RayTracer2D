#pragma once
#include "olcPixelGameEngine.h"
#include "Line.h"
#include "Math.h"


enum class SurfaceType { REFLECTIVE, REFRACTIVE };

struct Surface : Line
{
	Surface();
	Surface(olc::vf2d point1, olc::vf2d point2);
	Surface(olc::vf2d point1, olc::vf2d point2, bool isReflective, bool isRefractive, float refractiveIndex);
	Surface(olc::vf2d point1, olc::vf2d point2, SurfaceType surfaceType);
	Surface(olc::vf2d point1, olc::vf2d point2, SurfaceType surfaceType, float refractiveIndex);


	void Extend(float length);
	Surface Extended(float length) const;

	bool ContainsPoint(olc::vf2d point);

	bool IsContinuationOfAnotherSurface(const Surface& another_surface) const;

	float AngleCoefficient() const;


	bool is_reflective;
	bool is_refractive;
	float refractive_index;

	float extension;
};