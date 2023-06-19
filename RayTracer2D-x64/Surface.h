#pragma once
#include "olcPixelGameEngine.h"
#include "Line.h"
#include "Math.h"


struct Surface : public Line
{
	Surface();
	Surface(olc::vd2d point1, olc::vd2d point2);
	Surface(olc::vd2d point1, olc::vd2d point2, bool isReflective, bool isRefractive, float refractionIndex);

	//olc::vd2d PointMinX() const;
	//olc::vd2d PointMaxX() const;
	//olc::vd2d PointMinY() const;
	//olc::vd2d PointMaxY() const;

	void Extend(double length);

	bool ContainsPoint(olc::vd2d point);


	bool is_reflective;
	bool is_refractive;
	float refractive_index;
};