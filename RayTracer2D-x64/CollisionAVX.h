#include <vector>
#include <immintrin.h>
#include "Line.h"
#include "Surface.h"
#include "Ray.h"
#include "Collision.h"
#include "Constants.h"


struct CollisionInfoAVXRegisters
{
	__m256d _intersect = _mm256_setzero_pd();
	__m256d _coincide = _mm256_setzero_pd();
};

inline __m256d _mm256_nor_pd(const __m256d& _a, const __m256d& _b);
inline __m256d _mm256_not_pd(const __m256d& _a);
inline __m256d _mm256_eq_zero_pd(const __m256d& _a, const __m256d& _minus_bound, const __m256d& _bound);

CollisionInfoAVXRegisters _Ray1VsSurface4(const Ray& ray1, const std::vector<Surface>& lines2, int index_first_surface, __m256d& _inter_x, __m256d& _inter_y);