#include <vector>
#include <immintrin.h>
#include "Line.h"
#include "Surface.h"
#include "Ray.h"
#include "Collision.h"
#include "Constants.h"


struct CollisionInfoAVXRegisters
{
	__m256 _intersect = _mm256_setzero_ps();
	__m256 _coincide = _mm256_setzero_ps();
};

inline __m256 _mm256_nor_ps(const __m256& _a, const __m256& _b);
inline __m256 _mm256_not_ps(const __m256& _a);
inline __m256 _mm256_eq_zero_ps(const __m256& _a, const __m256& _minus_bound, const __m256& _bound);

CollisionInfoAVXRegisters _RayVsSegmentsAVX(const Ray& ray, const std::vector<Segment>& segments, int index_first_surface, __m256& _inter_x, __m256& _inter_y);