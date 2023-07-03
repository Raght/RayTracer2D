#include <vector>
#include <immintrin.h>
#include "Line.h"
#include "Surface.h"
#include "Ray.h"
#include "Collision.h"
#include "Constants.h"


struct CollisionInfoAVXRegisters
{
	__m256i intersect;
	__m256i coincide;
};

std::vector<CollisionInfo> _Ray1VsSurface4(const Ray& ray1, const std::vector<Surface>& lines2, int index_first_surface, std::vector<olc::vd2d>& intersectionPoints);