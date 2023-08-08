#include <vector>
#include <immintrin.h>
#include "Segment.h"
#include "Surface.h"
#include "Ray.h"
#include "Collision.h"
#include "Constants.h"
#include "AVXExtension.h"


struct CollisionInfoAVXRegisters
{
	__m256 _intersect = _mm256_setzero_ps();
	__m256 _coincide = _mm256_setzero_ps();
};


CollisionInfoAVXRegisters _RayLineVsSegmentsLinesAVX(const Ray& ray, const std::vector<Segment>& segments, int index_first_surface, __m256& _inter_x, __m256& _inter_y);