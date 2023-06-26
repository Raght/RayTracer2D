#pragma once
#include "Surface.h"


struct ReflectiveSurface : Surface
{
	ScatterInfo ScatterRay(const Ray& ray, const Surface& surface,
		const olc::vd2d& intersectionPoint, Ray& scatteredRay) override;
};