#include "RefractiveSurface.h"


ScatterInfo RefractiveSurface::ScatterRay(const Ray& ray, const Surface& surface, const olc::vd2d& intersectionPoint, Ray& scatteredRay)
{
	if (!TryRefractRay(ray, surface, intersectionPoint, scatteredRay))
	{
		scatteredRay = ReflectRay(ray, surface, intersectionPoint);
		return ScatterInfo(true, false);
	}
	return ScatterInfo(false, true);
}