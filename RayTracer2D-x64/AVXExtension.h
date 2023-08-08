#pragma once
#include <immintrin.h>
#include "Bits.h"


inline __m256 _mm256_xnor_ps(const __m256& _a, const __m256& _b)
{
	return _mm256_cmp_ps(_a, _b, _CMP_EQ_OQ);
}

inline __m256 _mm256_nor_ps(const __m256& _a, const __m256& _b)
{
	return _mm256_xor_ps(_mm256_and_ps(_a, _b), _mm256_cmp_ps(_a, _b, _CMP_EQ_OQ));
}

inline __m256 _mm256_not_ps(const __m256& _a)
{
	return _mm256_nor_ps(_a, _a);
}

inline __m256 _mm256_in_range_ps(const __m256& _a, const __m256& _minus_bound, const __m256& _bound)
{
	__m256 _gt_m_bound = _mm256_cmp_ps(_minus_bound, _a, _CMP_LT_OQ);
	__m256 _lt_bound = _mm256_cmp_ps(_a, _bound, _CMP_LT_OQ);
	return _mm256_and_ps(_gt_m_bound, _lt_bound);
}

inline __m256 _mm256_abs_ps(const __m256& _a)
{
	__m256 _mask = _mm256_set1_ps(uint32_t_as_float(0x7FFFFFFF));
	__m256 _abs = _mm256_and_ps(_a, _mask);
	return _abs;
}

inline __m256 _mm256_dot_ps(const __m256& _x1, const __m256& _y1, const __m256& _x2, const __m256& _y2)
{
	__m256 _x1_times_x2 = _mm256_mul_ps(_x1, _x2);
	__m256 _y1_times_y2 = _mm256_mul_ps(_y1, _y2);
	__m256 _dot = _mm256_add_ps(_x1_times_x2, _y1_times_y2);
	return _dot;
}

inline __m256 _mm256_cross_ps(const __m256& _x1, const __m256& _y1, const __m256& _x2, const __m256& _y2)
{
	__m256 _x1_times_y2 = _mm256_mul_ps(_x1, _y2);
	__m256 _x2_times_y1 = _mm256_mul_ps(_x2, _y1);
	__m256 _cross = _mm256_sub_ps(_x1_times_y2, _x2_times_y1);
	return _cross;
}

inline __m256 _mm256_cross_abs_ps(const __m256& _x1, const __m256& _y1, const __m256& _x2, const __m256& _y2)
{
	__m256 _cross_abs = _mm256_cross_ps(_x1, _y1, _x2, _y2);
	return _cross_abs;
}

inline __m256 _mm256_mag_sqr_ps(const __m256& _x, const __m256& _y)
{
	__m256 _x_sqr = _mm256_mul_ps(_x, _x);
	__m256 _y_sqr = _mm256_mul_ps(_y, _y);
	__m256 _mag_sqr = _mm256_add_ps(_x_sqr, _y_sqr);
	return _mag_sqr;
}

inline __m256 _mm256_mag_ps(const __m256& _x, const __m256& _y)
{
	__m256 _mag = _mm256_sqrt_ps(_mm256_mag_sqr_ps(_x, _y));
	return _mag;
}