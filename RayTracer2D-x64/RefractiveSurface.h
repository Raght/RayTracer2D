#pragma once
#include "Surface.h"


struct RefractiveSurface : Surface
{
	ScatterInfo RefractRay(const Ray& ray, const Surface& surface,
		const olc::vd2d& intersectionPoint, Ray& scatteredRay) override;
};