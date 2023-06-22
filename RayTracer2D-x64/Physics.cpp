#include "Physics.h"

using namespace std;


CollisionInfo::CollisionInfo()
{
	intersect = false;
	coincide = false;
}

CollisionInfo::CollisionInfo(bool intersect, bool coincide)
{
	this->intersect = intersect;
	this->coincide = coincide;
}

ScatterInfo::ScatterInfo()
{
	reflected = false;
	refracted = false;
}

ScatterInfo::ScatterInfo(bool reflected, bool refracted)
{
	this->reflected = reflected;
	this->refracted = refracted;
}

CollisionInfo LineVsLine(Line line1, Line line2, olc::vd2d& intersectionPoint)
{
	if (line1.p1.x > line1.p2.x)
	{
		swap(line1.p1, line1.p2);
	}
	if (line2.p1.x > line2.p2.x)
	{
		swap(line2.p1, line2.p2);
	}

	/*
		{ a1 * x + b1 = y
		{ a2 * x + b2 = y
	*/

	double dx1 = line1.p2.x - line1.p1.x;
	double dx2 = line2.p2.x - line2.p1.x;
	double dy1 = line1.p2.y - line1.p1.y;
	double dy2 = line2.p2.y - line2.p1.y;
	double a1 = dy1 / dx1;
	double a2 = dy2 / dx2;

	bool line1_horizontal = Equal(a1, 0);
	bool line2_horizontal = Equal(a2, 0);
	bool line1_vertical = isinf(a1);
	bool line2_vertical = isinf(a2);

	//if ((line2_horizontal || line2_vertical) && (!line1_horizontal && !line1_vertical))
	//{
	//	swap(line1, line2);
	//	swap(dx1, dx2);
	//	swap(dy1, dy2);
	//	swap(a1, a2);
	//	swap(line1_horizontal, line2_horizontal);
	//	swap(line1_vertical, line2_vertical);
	//}

	double b1 = line1.p1.y - a1 * line1.p1.x;
	double b2 = line2.p1.y - a2 * line2.p1.x;

	if (line1_horizontal)
	{
		if (line2_horizontal)
		{
			bool intersect_and_coincide = Equal(line1.p1.x, line2.p1.x);
			return CollisionInfo(intersect_and_coincide, intersect_and_coincide);
		}
		else if (line2_vertical)
		{
			intersectionPoint = { line2.p1.x, line1.p1.y };
			return CollisionInfo(true, false);
		}
		else
		{
			/*
				{ y = c
				{ a2 * x + b2 = y

				x0 = (c - b2) / a2
				y0 = c
			*/

			double x0 = (line1.p1.y - b2) / a2;
			double y0 = line1.p1.y;

			intersectionPoint = { x0, y0 };
			return CollisionInfo(true, false);
		}
	}
	else if (line1_vertical)
	{
		if (line2_horizontal)
		{
			intersectionPoint = { line1.p1.x, line2.p1.y };
			return CollisionInfo(true, false);
		}
		else if (line2_vertical)
		{
			bool intersect_and_coincide = Equal(line1.p1.y, line2.p1.y);
			return CollisionInfo(intersect_and_coincide, intersect_and_coincide);
		}
		else
		{
			/*
				{ x = c
				{ a2 * x + b2 = y

				x0 = c
				y0 = a2 * c + b2
			*/

			double x0 = line1.p1.x;
			double y0 = a2 * line1.p1.x + b2;

			intersectionPoint = { x0, y0 };
			return CollisionInfo(true, false);
		}
	}
	else
	{
		if (line2_horizontal)
		{
			/*
				{ a1 * x + b1 = y
				{ y = c

				x0 = (c - b1) / a1
				y0 = c
			*/

			double x0 = (line2.p1.y - b1) / a1;
			double y0 = line2.p1.y;

			intersectionPoint = { x0, y0 };
			return CollisionInfo(true, false);
		}
		else if (line2_vertical)
		{
			/*
				{ a1 * x + b1 = y
				{ x = c

				x0 = c
				y0 = a2 * c + b2
			*/

			double x0 = line2.p1.x;
			double y0 = a1 * line2.p1.x + b1;

			intersectionPoint = { x0, y0 };
			return CollisionInfo(true, false);
		}
		else
		{
			/*
				{ a1 * x + b1 = y
				{ a2 * x + b2 = y

				x0 = (b2 - b1) / (a1 - a2)
				y0 = a1 * x0 + b1
			*/

			double x0 = (b2 - b1) / (a1 - a2);
			double y0 = a1 * x0 + b1;

			intersectionPoint = { x0, y0 };
			return CollisionInfo(true, false);
		}
	}
}

CollisionInfo LineVsSurface(const Line& line, const Surface& surface, olc::vd2d& intersectionPoint)
{
	CollisionInfo collision_info = LineVsLine(line, surface, intersectionPoint);

	if (collision_info.coincide)
	{
		return collision_info;
	}

	bool point_lies_on_surface = (intersectionPoint - surface.Extended(surface.extension).p1).dot(intersectionPoint - surface.p2) < 0.0;

	if (collision_info.intersect && point_lies_on_surface)
	{
		return collision_info;
	}

	return CollisionInfo(false, false);
}

CollisionInfo SurfaceVsSurface(const Surface& surface1, const Surface& surface2, olc::vd2d& intersectionPoint)
{
	CollisionInfo collision_info = LineVsLine(surface1, surface2, intersectionPoint);

	if (collision_info.coincide)
	{
		return collision_info;
	}

	Surface extended_surface1 = surface1.Extended(surface1.extension);
	Surface extended_surface2 = surface2.Extended(surface2.extension);
	bool point_lies_on_surface1 = (intersectionPoint - extended_surface1.p1).dot(intersectionPoint - extended_surface1.p2) < 0.0;
	bool point_lies_on_surface2 = (intersectionPoint - extended_surface2.p1).dot(intersectionPoint - extended_surface2.p2) < 0.0;

	if (collision_info.intersect && point_lies_on_surface1 && point_lies_on_surface2)
	{
		return collision_info;
	}

	return CollisionInfo(false, false);
}

CollisionInfo SurfaceVsSurface(const Surface& surface1, const Surface& surface2)
{
	olc::vd2d intersection_point;
	return SurfaceVsSurface(surface1, surface2, intersection_point);
}

CollisionInfo RayVsSurface(const Ray& ray, const Surface& surface, olc::vd2d& intersectionPoint)
{
	return SurfaceVsSurface(Surface(ray.origin, ray.EndPoint()), surface, intersectionPoint);
	
	//if (!collision_info.intersect || collision_info.coincide)
	//{
	//	return collision_info;
	//}
	//
	//if (ray.direction.dot(intersectionPoint - ray.origin) > 0 && (intersectionPoint - ray.origin).mag2() <= ray.distance * ray.distance)
	//{
	//	return collision_info;
	//}
	//
	//return CollisionInfo(false, false);
}

Ray ReflectRay(const Ray& ray, const Surface& surface, const olc::vd2d& intersectionPoint)
{
	Ray reflected_ray = ray;

	//olc::vd2d intersection_point;
	//RayVsSurface(ray, surface, intersection_point);
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

ScatterInfo ScatterRay(const Ray& ray, const Surface& surface,
	const olc::vd2d& intersectionPoint, Ray& scatteredRay)
{
	ScatterInfo scatter_info = ScatterInfo();
	if (surface.is_reflective)
	{
		scatteredRay = ReflectRay(ray, surface, intersectionPoint);
		scatter_info.reflected = true;
	}
	else if (surface.is_refractive)
	{
		if (!TryRefractRay(ray, surface, intersectionPoint, scatteredRay))
		{
			scatteredRay = ReflectRay(ray, surface, intersectionPoint);
			scatter_info.reflected = true;
		}
		scatter_info.refracted = true;
	}

	return scatter_info;
}