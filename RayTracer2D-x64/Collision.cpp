#include "Collision.h"
#include <immintrin.h>


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

#define __AVX2 1
#if __AVX2
CollisionInfo LineVsLine(const Line& line1, const Line& line2, olc::vd2d& intersectionPoint)
{
	/*
		{ a1 * x + b1 = y
		{ a2 * x + b2 = y
	*/

	//double dx1 = line1.p2.x - line1.p1.x;
	//double dx2 = line2.p2.x - line2.p1.x;
	//double dy1 = line1.p2.y - line1.p1.y;
	//double dy2 = line2.p2.y - line2.p1.y;
	//
	//double a1 = dy1 / dx1;
	//double a2 = dy2 / dx2;
	//double b1 = line1.p1.y - a1 * line1.p1.x;
	//double b2 = line2.p1.y - a2 * line2.p1.x;

	__m128d _p1_x, _p2_x, _p1_y, _p2_y, _dx, _dy, _a, _b, _minus_one;
	
	_minus_one = _mm_set1_pd(-1.0);
	
	_p1_x = _mm_set_pd(line1.p1.x, line2.p1.x);
	_p2_x = _mm_set_pd(line1.p2.x, line2.p2.x);
	_p1_y = _mm_set_pd(line1.p1.y, line2.p1.y);
	_p2_y = _mm_set_pd(line1.p2.y, line2.p2.y);
	_dx = _mm_sub_pd(_p2_x, _p1_x);
	_dy = _mm_sub_pd(_p2_y, _p1_y);
	_a = _mm_div_pd(_dy, _dx);
	//_b = _mm_fnmadd_pd(_a, _p1_x, _p1_y);
	_b = _mm_mul_pd(_a, _p1_x);
	_b = _mm_mul_pd(_b, _minus_one);
	_b = _mm_add_pd(_b, _p1_y);

#if defined(__linux__)
	double a1 = _a[1];
	double a2 = _a[0];
	double b1 = _b[1];
	double b2 = _b[0];
#endif

#if defined(_WIN32)
	double a1 = _a.m128d_f64[1];
	double a2 = _a.m128d_f64[0];
	double b1 = _b.m128d_f64[1];
	double b2 = _b.m128d_f64[0];
#endif

	bool line1_horizontal = Equal(a1, 0);
	bool line2_horizontal = Equal(a2, 0);
	bool line1_vertical = isinf(a1);
	bool line2_vertical = isinf(a2);

	if (line1_horizontal)
	{
		if (line2_horizontal)
		{
			bool intersect_and_coincide = Equal(line1.p1.y, line2.p1.y);
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
			bool intersect_and_coincide = Equal(line1.p1.x, line2.p1.x);
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
#else
CollisionInfo LineVsLine(const Line& line1, const Line& line2, olc::vd2d& intersectionPoint)
{
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
	double b1 = line1.p1.y - a1 * line1.p1.x;
	double b2 = line2.p1.y - a2 * line2.p1.x;

	bool line1_horizontal = Equal(a1, 0);
	bool line2_horizontal = Equal(a2, 0);
	bool line1_vertical = isinf(a1);
	bool line2_vertical = isinf(a2);

	if (line1_horizontal)
	{
		if (line2_horizontal)
		{
			bool intersect_and_coincide = Equal(line1.p1.y, line2.p1.y);
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
			bool intersect_and_coincide = Equal(line1.p1.x, line2.p1.x);
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
#endif

CollisionInfo LineVsSurface(const Line& line, const Surface& surface, olc::vd2d& intersectionPoint)
{
	CollisionInfo collision_info = LineVsLine(line, surface, intersectionPoint);

	if (collision_info.coincide)
	{
		return collision_info;
	}

	double surface_length = (surface.p2 - surface.p1).mag();
	double surface_limit_dot_product = surface.extension * (surface_length + surface.extension);
	bool point_lies_on_surface = (intersectionPoint - surface.p1).dot(intersectionPoint - surface.p2) < surface_limit_dot_product;

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

	double surface1_length = (surface1.p2 - surface1.p1).mag();
	double surface2_length = (surface2.p2 - surface2.p1).mag();
	double surface1_limit_dot_product = surface1.extension * (surface1_length + surface1.extension);
	double surface2_limit_dot_product = surface2.extension * (surface2_length + surface2.extension);
	bool point_lies_on_surface1 = (intersectionPoint - surface1.p1).dot(intersectionPoint - surface1.p2) < surface1_limit_dot_product;
	bool point_lies_on_surface2 = (intersectionPoint - surface2.p1).dot(intersectionPoint - surface2.p2) < surface2_limit_dot_product;

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
}

bool PointVsRect(olc::vd2d point, olc::vd2d rectangle_position, olc::vd2d rectangle_size)
{
	return rectangle_position.x <= point.x && point.x <= rectangle_position.x + rectangle_size.x &&
		rectangle_position.y <= point.y && point.y <= rectangle_position.y + rectangle_size.y;
}