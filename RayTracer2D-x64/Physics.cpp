#include "Physics.h"

using namespace std;


Ray ReflectRay(const Ray& ray, const Surface& surface, const olc::vd2d& intersectionPoint)
{
	Ray reflected_ray = ray;

	reflected_ray.origin = intersectionPoint;

	double angle = AngleBetween(ray.direction, surface.Normal(ray.direction));

	if (ray.direction.cross(surface.Normal(ray.direction)) < 0.0)
	{
		angle = -angle;
}

	reflected_ray.direction = Rotate({ -ray.direction.x, -ray.direction.y }, angle * 2);

	return reflected_ray;
}

bool TryRefractRay(const Ray& ray, const Surface& surface, const olc::vd2d& intersectionPoint, Ray& refractedRay)
{
	refractedRay = ray;

	double theta1 = AngleBetween(ray.direction * (-1.0), surface.Normal(ray.direction));
	double theta2;
	double n1 = ray.refractive_index, n2 = surface.refractive_index;
	if (Equal(ray.refractive_index, surface.refractive_index))
	{
		n2 = 1.0f;
	}

	double sin_theta2 = sinf(theta1) * (n1 / n2);

	if (sin_theta2 > 1.0f)
	{
		return false;
	}

	theta2 = asinf(sin_theta2);

	if (ray.direction.cross(surface.Normal(ray.direction)) < 0.0)
	{
		theta2 = -theta2;
	}

	refractedRay.origin = intersectionPoint;
	refractedRay.direction = Rotate(surface.Normal(ray.direction) * (-1.0), theta2);
	refractedRay.refractive_index = n2;

	return true;
}
