#include "Surface.h"


Surface::Surface()
	: Line()
{
	is_reflective = false;
	is_refractive = !is_reflective;
	refractive_index = 2.5f;
}

Surface::Surface(olc::vd2d point1, olc::vd2d point2)
	: Surface()
{
	p1 = point1;
	p2 = point2;
}

Surface::Surface(olc::vd2d point1, olc::vd2d point2, bool isReflective, bool isRefractive, float refractionIndex)
	: Surface(point1, point2)
{
	p1 = point1;
	p2 = point2;
	this->is_reflective = isReflective;
	this->is_refractive = isRefractive;
	this->refractive_index = refractionIndex;
}

//olc::vd2d& Surface::PointMinX() const
//{
//	return (p1.x < p2.x) ? p1 : p2;
//}
//olc::vd2d& Surface::PointMaxX() const
//{
//	return (p1.x < p2.x) ? p2 : p1;
//}
//olc::vd2d& Surface::PointMinY() const
//{
//	return (p1.y < p2.y) ? p1 : p2;
//}
//olc::vd2d& Surface::PointMaxY() const
//{
//	return (p1.y < p2.y) ? p2 : p1;
//}

void Surface::Extend(double length)
{
	olc::vd2d& p_min_x = (p1.x < p2.x) ? p1 : p2;
	olc::vd2d& p_max_x = (p1.x < p2.x) ? p2 : p1;
	olc::vd2d& p_min_y = (p1.y < p2.y) ? p1 : p2;
	olc::vd2d& p_max_y = (p1.y < p2.y) ? p2 : p1;

	double dx = p_max_x.x - p_min_x.x;
	double dy = p_max_y.y - p_min_y.y;
	double a = dy / dx;

	double surface_horizontal = Equal(a, 0);
	double surface_vertical = isinf(a);

	if (surface_horizontal)
	{
		p_min_x.x -= length;
		p_max_x.x += length;
	}
	else if (surface_vertical)
	{
		p_min_y.y -= length;
		p_max_y.y += length;
	}
	else
	{
		/*
			{ y = a*x
			{ x^2 + y^2 = r^2

			(a^2 + 1) * x^2 = r^2
			x = +-r * sqrt(1 / (a^2 + 1))
		*/
		dx = length * sqrt(1 / (a * a + 1));
		dy = a * dx;

		p_min_x.x -= dx;
		p_max_x.x += dx;
		p_min_y.y -= dy;
		p_max_y.y += dy;
	}
}

bool Surface::ContainsPoint(olc::vd2d point)
{
	olc::vd2d& p_min_x = (p1.x < p2.x) ? p1 : p2;
	olc::vd2d& p_max_x = (p1.x < p2.x) ? p2 : p1;
	olc::vd2d& p_min_y = (p1.y < p2.y) ? p1 : p2;
	olc::vd2d& p_max_y = (p1.y < p2.y) ? p2 : p1;

	double dx = p_max_x.x - p_min_x.x;
	double dy = p_max_y.y - p_min_y.y;
	double a = dy / dx;

	double surface_horizontal = Equal(a, 0);
	double surface_vertical = isinf(a);

	if (surface_horizontal)
	{
		return (p_min_x.x <= point.x && point.x <= p_max_x.x) && Equal(point.y, p_min_x.y);
	}
	else if (surface_vertical)
	{
		return (p_min_y.x <= point.y && point.y <= p_max_y.y) && Equal(point.x, p_min_y.x);
	}
	else
	{
		double b = p_min_y.y - a * p_min_y.x;
		return (p_min_x.x <= point.x && point.x <= p_max_x.x) && Equal(a * point.x + b, point.y);
	}
}