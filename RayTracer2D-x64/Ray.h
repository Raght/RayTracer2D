#pragma once
#include "olcPixelGameEngine.h"
#include "Math.h"
#include "Surface.h"


struct Ray
{
	olc::vf2d EndPoint() const;

	Ray OppositeRay() const;

	olc::Pixel Color() const;

	bool InVacuum() const;
	bool InsideMedium() const;


	Ray();
	Ray(olc::vf2d origin, olc::vf2d direction, float distance);
	Ray(olc::vf2d origin, olc::vf2d direction, float distance, olc::Pixel color, float brightness);
	Ray(olc::vf2d origin, olc::vf2d direction, float distance, olc::Pixel color, float brightness, float refractive_index);


	olc::vf2d origin;
	olc::vf2d direction;
	float distance;

	olc::Pixel color_max_brightness;
	float brightness;

	float refractive_index;
};
