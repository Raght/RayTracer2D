#pragma once
#include "olcPixelGameEngine.h"
#include "Line.h"
#include "Math.h"
#include "Physics.h"
#include "Ray.h"


struct ScatterInfo
{
	ScatterInfo();
	ScatterInfo(bool reflected, bool refracted);

	bool reflected;
	bool refracted;
};


enum class SurfaceType { REFLECTIVE, REFRACTIVE };

struct Surface : public Line
{
	Surface();
	Surface(olc::vd2d point1, olc::vd2d point2);
	Surface(olc::vd2d point1, olc::vd2d point2, bool isReflective, bool isRefractive, double refractiveIndex);
	Surface(olc::vd2d point1, olc::vd2d point2, SurfaceType surfaceType, double refractiveIndex);

	void Extend(double length);
	Surface Extended(double length) const;

	bool ContainsPoint(olc::vd2d point);

	virtual ScatterInfo ScatterRay(const Ray& ray, const Surface& surface,
		const olc::vd2d& intersectionPoint, Ray& scatteredRay);


	bool is_reflective;
	bool is_refractive;
	double refractive_index;

	double extension = 0;
};