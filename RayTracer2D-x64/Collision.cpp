#include "Collision.h"


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


CollisionInfo LineVsLine(const Line& line1, const Line& line2, olc::vd2d& intersectionPoint)
{
	olc::vd2d line1_p1 = (line1.p1.x < line1.p2.x) ? line1.p1 : line1.p2;
	olc::vd2d line1_p2 = (line1.p1.x < line1.p2.x) ? line1.p2 : line1.p1;
	olc::vd2d line2_p1 = (line2.p1.x < line2.p2.x) ? line2.p1 : line2.p2;
	olc::vd2d line2_p2 = (line2.p1.x < line2.p2.x) ? line2.p2 : line2.p1;

	//if (line1.p1.x > line1.p2.x)
	//{
	//	std::swap(line1.p1, line1.p2);
	//}
	//if (line2.p1.x > line2.p2.x)
	//{
	//	std::swap(line2.p1, line2.p2);
	//}


	

	/*
		{ a1 * x + b1 = y
		{ a2 * x + b2 = y
	*/

	double dx1 = line1_p2.x - line1_p1.x;
	double dx2 = line2_p2.x - line2_p1.x;
	double dy1 = line1_p2.y - line1_p1.y;
	double dy2 = line2_p2.y - line2_p1.y;
	double a1 = dy1 / dx1;
	double a2 = dy2 / dx2;

	bool line1_horizontal = Equal(a1, 0);
	bool line2_horizontal = Equal(a2, 0);
	bool line1_vertical = isinf(a1);
	bool line2_vertical = isinf(a2);

	double b1 = line1_p1.y - a1 * line1_p1.x;
	double b2 = line2_p1.y - a2 * line2_p1.x;

	if (line1_horizontal)
	{
		if (line2_horizontal)
		{
			bool intersect_and_coincide = Equal(line1_p1.y, line2_p1.y);
			return CollisionInfo(intersect_and_coincide, intersect_and_coincide);
		}
		else if (line2_vertical)
		{
			intersectionPoint = { line2_p1.x, line1_p1.y };
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

			double x0 = (line1_p1.y - b2) / a2;
			double y0 = line1_p1.y;

			intersectionPoint = { x0, y0 };
			return CollisionInfo(true, false);
		}
	}
	else if (line1_vertical)
	{
		if (line2_horizontal)
		{
			intersectionPoint = { line1_p1.x, line2_p1.y };
			return CollisionInfo(true, false);
		}
		else if (line2_vertical)
		{
			bool intersect_and_coincide = Equal(line1_p1.x, line2_p1.x);
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

			double x0 = line1_p1.x;
			double y0 = a2 * line1_p1.x + b2;

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

			double x0 = (line2_p1.y - b1) / a1;
			double y0 = line2_p1.y;

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

			double x0 = line2_p1.x;
			double y0 = a1 * line2_p1.x + b1;

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
}

bool PointVsRect(olc::vd2d point, olc::vd2d rectangle_position, olc::vd2d rectangle_size)
{
	return rectangle_position.x <= point.x && point.x <= rectangle_position.x + rectangle_size.x &&
		rectangle_position.y <= point.y && point.y <= rectangle_position.y + rectangle_size.y;
}