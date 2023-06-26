#include "ReflectiveSurface.h"


ScatterInfo ReflectiveSurface::ScatterRay(const Ray& ray, const Surface& surface, const olc::vd2d& intersectionPoint, Ray& scatteredRay)
{
	scatteredRay = ReflectRay(ray, surface, intersectionPoint);
	return ScatterInfo(true, false);
}
