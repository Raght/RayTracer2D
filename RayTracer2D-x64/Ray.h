#pragma once
#include "olcPixelGameEngine.h"
#include "Math.h"
#include "Surface.h"


struct Ray
{
	olc::vd2d EndPoint() const;

	Ray OppositeRay() const;

	olc::Pixel Color() const;

	bool InsideSurface() const;

	void MoveOriginBack(double length)
	{
		origin -= length * direction;
	}

	Ray();
	Ray(olc::vd2d origin, olc::vd2d direction, double distance);
	Ray(olc::vd2d origin, olc::vd2d direction, double distance, olc::Pixel color, double brightness);
	Ray(olc::vd2d origin, olc::vd2d direction, double distance, olc::Pixel color, double brightness, double refractive_index);

	olc::vd2d origin;
	olc::vd2d direction;
	double distance;

	olc::Pixel color_max_brightness;
	double brightness;

	double refractive_index;
};
