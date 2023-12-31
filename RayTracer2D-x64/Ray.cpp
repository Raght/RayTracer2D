#include "Ray.h"


olc::vd2d Ray::EndPoint() const
{
	return origin + direction * distance;
}

Ray Ray::OppositeRay() const
{
	Ray opposite_ray = (*this);
	opposite_ray.direction = opposite_ray.direction * (-1.0);

	return opposite_ray;
}

olc::Pixel Ray::Color() const
{
	return color_max_brightness * brightness;
}

bool Ray::InVacuum() const
{
	return Equal(refractive_index, 1.0);
}

bool Ray::InsideMedium() const
{
	return !InsideMedium();
}


Ray::Ray()
{
	origin = { 0, 0 };
	direction = { 1, 0 };
	distance = 1;
	color_max_brightness = olc::MAGENTA;
	brightness = 1;
	refractive_index = 1;
}

Ray::Ray(olc::vd2d origin, olc::vd2d direction, double distance)
	: Ray()
{
	this->origin = origin;
	this->direction = direction;
	this->distance = distance;
}

Ray::Ray(olc::vd2d origin, olc::vd2d direction, double distance, olc::Pixel color, double brightness)
	: Ray(origin, direction, distance)
{
	this->color_max_brightness = color;
	this->brightness = brightness;
}

Ray::Ray(olc::vd2d origin, olc::vd2d direction, double distance, olc::Pixel color, double brightness, double refractive_index)
	: Ray(origin, direction, distance, color, brightness)
{
	this->refractive_index = refractive_index;
}