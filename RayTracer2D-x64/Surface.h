#pragma once
#include "olcPixelGameEngine.h"
#include "Line.h"
#include "Math.h"


enum class SurfaceType { REFLECTIVE, REFRACTIVE };

struct Surface : Line
{
	Surface();
	Surface(olc::vd2d point1, olc::vd2d point2);
	Surface(olc::vd2d point1, olc::vd2d point2, bool isReflective, bool isRefractive, double refractiveIndex);
	Surface(olc::vd2d point1, olc::vd2d point2, SurfaceType surfaceType);
	Surface(olc::vd2d point1, olc::vd2d point2, SurfaceType surfaceType, double refractiveIndex);


	void Extend(double length);
	Surface Extended(double length) const;

	bool ContainsPoint(olc::vd2d point);


	bool is_reflective;
	bool is_refractive;
	double refractive_index;

	double extension;
};