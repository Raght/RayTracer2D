#include "Collision.h"
#include <vector>
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

CollisionInfo LineVsLine(const Segment& line1, const Segment& line2, olc::vf2d& intersectionPoint)
{
	/*
		{ a1 * x + b1 = y
		{ a2 * x + b2 = y
	*/

	float dx1 = line1.p2.x - line1.p1.x;
	float dx2 = line2.p2.x - line2.p1.x;
	float dy1 = line1.p2.y - line1.p1.y;
	float dy2 = line2.p2.y - line2.p1.y;
	float a1 = dy1 / dx1;
	float a2 = dy2 / dx2;
	float b1 = line1.p1.y - a1 * line1.p1.x;
	float b2 = line2.p1.y - a2 * line2.p1.x;

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

			float x0 = (line1.p1.y - b2) / a2;
			float y0 = line1.p1.y;

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

			float x0 = line1.p1.x;
			float y0 = a2 * line1.p1.x + b2;

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

			float x0 = (line2.p1.y - b1) / a1;
			float y0 = line2.p1.y;

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

			float x0 = line2.p1.x;
			float y0 = a1 * line2.p1.x + b1;

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

			if (Equal(a1, a2))
			{
				bool intersect_and_coincide = Equal(b1, b2);
				return CollisionInfo(intersect_and_coincide, intersect_and_coincide);
			}

			float x0 = (b2 - b1) / (a1 - a2);
			float y0 = a1 * x0 + b1;

			intersectionPoint = { x0, y0 };
			return CollisionInfo(true, false);
		}
	}
}

CollisionInfo LineVsSurface(const Segment& line, const Surface& surface, olc::vf2d& intersectionPoint)
{
	CollisionInfo collision_info = LineVsLine(line, surface, intersectionPoint);

	if (collision_info.coincide)
	{
		return collision_info;
	}

	float surface_length = (surface.p2 - surface.p1).mag();
	float surface_limit_dot_product = surface.extension * (surface_length + surface.extension);
	bool point_lies_on_surface = (intersectionPoint - surface.p1).dot(intersectionPoint - surface.p2) < surface_limit_dot_product;

	if (collision_info.intersect && point_lies_on_surface)
	{
		return collision_info;
	}

	return CollisionInfo(false, false);
}

CollisionInfo SegmentVsSegment(const Segment& surface1, const Segment& surface2, olc::vf2d& intersectionPoint)
{
	CollisionInfo collision_info = LineVsLine(surface1, surface2, intersectionPoint);

	if (collision_info.coincide)
	{
		return collision_info;
	}

	float surface1_length = (surface1.p2 - surface1.p1).mag();
	float surface2_length = (surface2.p2 - surface2.p1).mag();
	float surface1_limit_dot_product = SURFACES_EXTENSION * (surface1_length + SURFACES_EXTENSION);
	float surface2_limit_dot_product = SURFACES_EXTENSION * (surface2_length + SURFACES_EXTENSION);
	bool point_lies_on_surface1 = (intersectionPoint - surface1.p1).dot(intersectionPoint - surface1.p2) < surface1_limit_dot_product;
	bool point_lies_on_surface2 = (intersectionPoint - surface2.p1).dot(intersectionPoint - surface2.p2) < surface2_limit_dot_product;

	if (collision_info.intersect && point_lies_on_surface1 && point_lies_on_surface2)
	{
		return collision_info;
	}

	return CollisionInfo(false, false);
}

CollisionInfo SurfaceVsSurface(const Surface& surface1, const Surface& surface2, olc::vf2d& intersectionPoint)
{
	CollisionInfo collision_info = LineVsLine(surface1, surface2, intersectionPoint);

	if (collision_info.coincide)
	{
		return collision_info;
	}

	float surface1_length = (surface1.p2 - surface1.p1).mag();
	float surface2_length = (surface2.p2 - surface2.p1).mag();
	float surface1_limit_dot_product = surface1.extension * (surface1_length + surface1.extension);
	float surface2_limit_dot_product = surface2.extension * (surface2_length + surface2.extension);
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
	olc::vf2d intersection_point;
	return SurfaceVsSurface(surface1, surface2, intersection_point);
}

#if OPTIMIZED_COLLISION_INTERSECTIONS
CollisionInfo RayVsSegment(const Ray& ray, const Segment& segment, olc::vf2d& intersectionPoint)
{
	olc::vf2d ray_origin_to_p1 = segment.p1 - ray.origin;
	olc::vf2d p1_to_ray_origin = ray.origin - segment.p1;
	olc::vf2d p1_to_p2 = segment.p2 - segment.p1;
	float length_squared = ray_origin_to_p1.mag2();
	float cross1 = ray.direction.cross_abs(ray_origin_to_p1);
	float dot1 = ray.direction.dot(ray_origin_to_p1);
	float cross2 = p1_to_p2.cross_abs(p1_to_ray_origin);
	float dot2 = p1_to_p2.dot(p1_to_ray_origin);
	float length_squared_times_cross2 = length_squared * cross2;
	float dot1_times_sum_of_tangents = cross1 * dot2 + cross2 * dot1;
	if (Equal(dot1_times_sum_of_tangents, 0.0f))
	{
		bool intersect_and_coincide = Equal(length_squared_times_cross2, 0.0f);
		return CollisionInfo(intersect_and_coincide, intersect_and_coincide);
	}
	else
	{
		float distance_to_intersection = length_squared_times_cross2 / dot1_times_sum_of_tangents;
		intersectionPoint = ray.origin + ray.direction * distance_to_intersection;
		return CollisionInfo(true, false);
	}
}
#else
CollisionInfo RayVsSegment(const Ray& ray, const Segment& segment, olc::vf2d& intersectionPoint)
{
	Segment ray_as_segment = Segment(ray.origin, ray.EndPoint());

	CollisionInfo collision_info = LineVsLine(ray_as_segment, segment, intersectionPoint);

	if (collision_info.coincide)
	{
		return collision_info;
	}

	float surface1_length = (ray_as_segment.p2 - ray_as_segment.p1).mag();
	float surface2_length = (segment.p2 - segment.p1).mag();
	float surface1_limit_dot_product = 0;
	float surface2_limit_dot_product = SURFACES_EXTENSION * (surface2_length + SURFACES_EXTENSION);
	bool point_lies_on_surface1 = (intersectionPoint - ray_as_segment.p1).dot(intersectionPoint - ray_as_segment.p2) < surface1_limit_dot_product;
	bool point_lies_on_surface2 = (intersectionPoint - segment.p1).dot(intersectionPoint - segment.p2) < surface2_limit_dot_product;

	if (collision_info.intersect && point_lies_on_surface1 && point_lies_on_surface2)
	{
		return collision_info;
	}

	return CollisionInfo(false, false);
}
#endif

CollisionInfo RayVsSurface(const Ray& ray, const Surface& surface, olc::vf2d& intersectionPoint)
{
	Surface ray_as_surface = Surface(ray.origin, ray.EndPoint());
	ray_as_surface.extension = 0.0;
	return SurfaceVsSurface(ray_as_surface, surface, intersectionPoint);
}

bool RayLineVsSegment(const Ray& ray, const Segment& segment)
{
	olc::vf2d ray_origin_to_p1 = segment.p1 - ray.origin;
	olc::vf2d ray_origin_to_p2 = segment.p2 - ray.origin;
	bool line_intersects_segment = ray_origin_to_p1.cross(ray.direction) * ray_origin_to_p2.cross(ray.direction) <= 0.0f;

	return line_intersects_segment;
}

bool RayVsSegment(const Ray& ray, const Segment& segment)
{
	olc::vf2d ray_origin_to_p1 = segment.p1 - ray.origin;
	olc::vf2d ray_origin_to_p2 = segment.p2 - ray.origin;
	float cross1 = ray.direction.cross(ray_origin_to_p1);
	float cross2 = ray.direction.cross(ray_origin_to_p2);
	bool line_intersects_segment = cross1 * cross2 <= 0.0f;
	float cross3 = ray_origin_to_p1.cross(ray_origin_to_p2);
	bool ray_points_towards_segment = cross3 * cross1 <= 0.0f; // && cross3 * cross2 >= 0.0f;
	return line_intersects_segment && ray_points_towards_segment;
	//if (!line_intersects_segment)
	//{
	//	return false;
	//}
	//else
	//{
	//	float cross3 = ray_origin_to_p1.cross(ray_origin_to_p2);
	//	if (cross3 >= 0.0f)
	//		return cross1 <= 0.0f;
	//	else
	//		return cross1 >= 0.0f;
	//	// This can be converted to
	//	//return cross3 * cross1 <= 0.0f;
	//}
}

bool PointVsRect(olc::vf2d point, olc::vf2d rectangle_position, olc::vf2d rectangle_size)
{
	return rectangle_position.x <= point.x && point.x <= rectangle_position.x + rectangle_size.x &&
		rectangle_position.y <= point.y && point.y <= rectangle_position.y + rectangle_size.y;
}