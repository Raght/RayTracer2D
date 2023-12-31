#pragma once
#include "olcPixelGameEngine.h"
#include "Math.h"
#include "Ray.h"
#include "Line.h"
#include "Surface.h"


struct ScatterInfo
{
	ScatterInfo();
	ScatterInfo(bool reflected, bool refracted);

	bool reflected;
	bool refracted;
};

Ray ReflectRay(const Ray& ray, const Surface& surface, const olc::vd2d& intersectionPoint);

bool TryRefractRay(const Ray& ray, const Surface& surface, const olc::vd2d& intersectionPoint, Ray& refractedRay);

ScatterInfo ScatterRay(const Ray& ray, const Surface& surface, const olc::vd2d& intersectionPoint, Ray& scatteredRay);