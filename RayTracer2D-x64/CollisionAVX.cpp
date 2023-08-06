#include "CollisionAVX.h"


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

inline __m256 _mm256_eq_zero_ps(const __m256& _a, const __m256& _minus_bound, const __m256& _bound)
{
	__m256 _gt_m_bound = _mm256_cmp_ps(_minus_bound, _a, _CMP_LT_OQ);
	__m256 _lt_bound = _mm256_cmp_ps(_a, _bound, _CMP_LT_OQ);
	return _mm256_and_ps(_gt_m_bound, _lt_bound);
}


CollisionInfoAVXRegisters _RayLineVsSegmentsLinesAVX(const Ray& ray, const std::vector<Segment>& segments, int index_first_surface, __m256& _inter_x, __m256& _inter_y)
{
	/*
		{ a1 * x + b1 = y
		{ a2 * x + b2 = y
	*/

	//float dx1 = line1.p2.x - line1.p1.x;
	//float dx2 = line2.p2.x - line2.p1.x;
	//float dy1 = line1.p2.y - line1.p1.y;
	//float dy2 = line2.p2.y - line2.p1.y;
	//
	//float a1 = dy1 / dx1;
	//float a2 = dy2 / dx2;
	//float b1 = line1.p1.y - a1 * line1.p1.x;
	//float b2 = line2.p1.y - a2 * line2.p1.x;

	__m256 _minus_one, _MINUS_EPSILON, _EPSILON, _MINUS_INFINITY, _INFINITY;

	_minus_one = _mm256_set1_ps(-1.0);
	_MINUS_EPSILON = _mm256_set1_ps(-EPSILON);
	_EPSILON = _mm256_set1_ps(EPSILON);
	_MINUS_INFINITY = _mm256_set1_ps((float)-INFINITY);
	_INFINITY = _mm256_set1_ps((float)INFINITY);

	__m256 _l1p1x, _l1p1y, _l1p2x, _l1p2y, _l2p1x, _l2p1y, _l2p2x, _l2p2y;
	__m256 _dx1, _dx2, _dy1, _dy2;
	__m256 _a1, _a2, _b1, _b2;
	__m256 _a1eqpINF_mask, _a1eqmINF_mask, _a2eqpINF_mask, _a2eqmINF_mask;
	__m256 _l1horizontal, _l2horizontal, _l1vertical, _l2vertical;

	int j = index_first_surface;
	_l1p1x = _mm256_set1_ps(ray.origin.x);
	_l1p1y = _mm256_set1_ps(ray.origin.y);
	_l1p2x = _mm256_set1_ps(ray.EndPoint().x);
	_l1p2y = _mm256_set1_ps(ray.EndPoint().y);
	_l2p1x = _mm256_set_ps(segments[j].p1.x, segments[j + 1].p1.x,
		segments[j + 2].p1.x, segments[j + 3].p1.x,
		segments[j + 4].p1.x, segments[j + 5].p1.x,
		segments[j + 6].p1.x, segments[j + 7].p1.x);
	_l2p1y = _mm256_set_ps(segments[j].p1.y, segments[j + 1].p1.y,
		segments[j + 2].p1.y, segments[j + 3].p1.y,
		segments[j + 4].p1.y, segments[j + 5].p1.y,
		segments[j + 6].p1.y, segments[j + 7].p1.y);
	_l2p2x = _mm256_set_ps(segments[j].p2.x, segments[j + 1].p2.x,
		segments[j + 2].p2.x, segments[j + 3].p2.x,
		segments[j + 4].p2.x, segments[j + 5].p2.x,
		segments[j + 6].p2.x, segments[j + 7].p2.x);
	_l2p2y = _mm256_set_ps(segments[j].p2.y, segments[j + 1].p2.y,
		segments[j + 2].p2.y, segments[j + 3].p2.y,
		segments[j + 4].p2.y, segments[j + 5].p2.y,
		segments[j + 6].p2.y, segments[j + 7].p2.y);

	_dx1 = _mm256_sub_ps(_l1p2x, _l1p1x);
	_dx2 = _mm256_sub_ps(_l2p2x, _l2p1x);
	_dy1 = _mm256_sub_ps(_l1p2y, _l1p1y);
	_dy2 = _mm256_sub_ps(_l2p2y, _l2p1y);

	_a1 = _mm256_div_ps(_dy1, _dx1);
	_a2 = _mm256_div_ps(_dy2, _dx2);

	//_b1 = _mm256_fnmadd_pd(_a1, _l1p1x, _l1p1y);
	//_b2 = _mm256_fnmadd_pd(_a2, _l2p1x, _l2p1y);

	_b1 = _mm256_mul_ps(_a1, _l1p1x);
	_b1 = _mm256_mul_ps(_b1, _minus_one);
	_b1 = _mm256_add_ps(_b1, _l1p1y);
	_b2 = _mm256_mul_ps(_a2, _l2p1x);
	_b2 = _mm256_mul_ps(_b2, _minus_one);
	_b2 = _mm256_add_ps(_b2, _l2p1y);

	_l1horizontal = _mm256_eq_zero_ps(_a1, _MINUS_EPSILON, _EPSILON);
	_l2horizontal = _mm256_eq_zero_ps(_a2, _MINUS_EPSILON, _EPSILON);

	_a1eqmINF_mask = (_mm256_cmp_ps(_a1, _MINUS_INFINITY, _CMP_EQ_OQ));
	_a1eqpINF_mask = (_mm256_cmp_ps(_a1, _INFINITY, _CMP_EQ_OQ));
	_a2eqmINF_mask = (_mm256_cmp_ps(_a2, _MINUS_INFINITY, _CMP_EQ_OQ));
	_a2eqpINF_mask = (_mm256_cmp_ps(_a2, _INFINITY, _CMP_EQ_OQ));

	_l1vertical = _mm256_or_ps(_a1eqmINF_mask, _a1eqpINF_mask);
	_l2vertical = _mm256_or_ps(_a2eqmINF_mask, _a2eqpINF_mask);

	__m256 _l1angle, _l2angle;
	_l1angle = _mm256_nor_ps(_l1horizontal, _l1vertical);
	_l2angle = _mm256_nor_ps(_l2horizontal, _l2vertical);



	CollisionInfoAVXRegisters collision_info;

	uint64_t all_ones_int = UINT64_MAX;
	float all_ones = *(float*)(uint64_t*)&all_ones_int;
	__m256 _all_ones = _mm256_set1_ps(all_ones);

	_inter_x = _mm256_setzero_ps();
	_inter_y = _mm256_setzero_ps();

	__m256 _hh, _hv, _ha, _vh, _vv, _va, _ah, _av, _aa;
	_hh = _mm256_and_ps(_l1horizontal, _l2horizontal);
	_hv = _mm256_and_ps(_l1horizontal, _l2vertical);
	_ha = _mm256_and_ps(_l1horizontal, _l2angle);
	_vh = _mm256_and_ps(_l1vertical, _l2horizontal);
	_vv = _mm256_and_ps(_l1vertical, _l2vertical);
	_va = _mm256_and_ps(_l1vertical, _l2angle);
	_ah = _mm256_and_ps(_l1angle, _l2horizontal);
	_av = _mm256_and_ps(_l1angle, _l2vertical);
	_aa = _mm256_and_ps(_l1angle, _l2angle);

	collision_info._intersect = _all_ones;
	collision_info._coincide = _mm256_setzero_ps();

	// hh
	{
		__m256 _intersect_and_coincide_mask = _mm256_eq_zero_ps(_mm256_sub_ps(_l1p1y, _l2p1y), _MINUS_EPSILON, _EPSILON);
		__m256 _intersect_and_coincide_i = _mm256_and_ps(_hh, _intersect_and_coincide_mask);
		collision_info._intersect = _mm256_and_ps(collision_info._intersect, _mm256_not_ps(_hh));
		collision_info._intersect = _mm256_or_ps(collision_info._intersect, _intersect_and_coincide_i);
		collision_info._coincide = _mm256_or_ps(collision_info._coincide, _intersect_and_coincide_i);
	}
	// hv
	_inter_x = _mm256_or_ps(_inter_x, _mm256_and_ps(_hv, _l2p1x));
	_inter_y = _mm256_or_ps(_inter_y, _mm256_and_ps(_hv, _l1p1y));
	// ha
	_inter_x = _mm256_or_ps(_inter_x, _mm256_and_ps(_ha, _mm256_div_ps(_mm256_sub_ps(_l1p1y, _b2), _a2)));
	_inter_y = _mm256_or_ps(_inter_y, _mm256_and_ps(_ha, _l1p1y));
	// vh
	_inter_x = _mm256_or_ps(_inter_x, _mm256_and_ps(_vh, _l1p1x));
	_inter_y = _mm256_or_ps(_inter_y, _mm256_and_ps(_vh, _l2p1y));
	// vv
	{
		__m256 _intersect_and_coincide_mask = _mm256_eq_zero_ps(_mm256_sub_ps(_l1p1x, _l2p1x), _MINUS_EPSILON, _EPSILON);
		__m256 _intersect_and_coincide_i = _mm256_and_ps(_vv, _intersect_and_coincide_mask);
		collision_info._intersect = _mm256_and_ps(collision_info._intersect, _mm256_not_ps(_vv));
		collision_info._intersect = _mm256_or_ps(collision_info._intersect, _intersect_and_coincide_i);
		collision_info._coincide = _mm256_or_ps(collision_info._coincide, _intersect_and_coincide_i);
	}
	// va
	_inter_x = _mm256_or_ps(_inter_x, _mm256_and_ps(_va, _l1p1x));
	_inter_y = _mm256_or_ps(_inter_y, _mm256_and_ps(_va, _mm256_add_ps(_mm256_mul_ps(_l1p1x, _a2), _b2)));
	// ah
	_inter_x = _mm256_or_ps(_inter_x, _mm256_and_ps(_ah, _mm256_div_ps(_mm256_sub_ps(_l2p1y, _b1), _a1)));
	_inter_y = _mm256_or_ps(_inter_y, _mm256_and_ps(_ah, _l2p1y));
	// av
	_inter_x = _mm256_or_ps(_inter_x, _mm256_and_ps(_av, _l2p1x));
	_inter_y = _mm256_or_ps(_inter_y, _mm256_and_ps(_av, _mm256_add_ps(_mm256_mul_ps(_l2p1x, _a1), _b1)));
	// aa
	{
		__m256 _a1_eq_a2 = _mm256_eq_zero_ps(_mm256_sub_ps(_a1, _a2), _MINUS_EPSILON, _EPSILON);
		__m256 _b1_eq_b2 = _mm256_eq_zero_ps(_mm256_sub_ps(_b1, _b2), _MINUS_EPSILON, _EPSILON);
		__m256 _intersect_and_coincide_mask = _mm256_and_ps(_a1_eq_a2, _b1_eq_b2);
		__m256 _intersect_and_coincide_i = _mm256_and_ps(_aa, _intersect_and_coincide_mask);
		collision_info._intersect = _mm256_and_ps(collision_info._intersect, _mm256_not_ps(_aa));
		collision_info._intersect = _mm256_or_ps(collision_info._intersect, _intersect_and_coincide_i);
		collision_info._coincide = _mm256_or_ps(collision_info._coincide, _intersect_and_coincide_i);
	}
	_inter_x = _mm256_or_ps(_inter_x, _mm256_and_ps(_aa, _mm256_div_ps(_mm256_sub_ps(_b2, _b1), _mm256_sub_ps(_a1, _a2))));
	_inter_y = _mm256_or_ps(_inter_y, _mm256_and_ps(_aa, _mm256_add_ps(_mm256_mul_ps(_inter_x, _a1), _b1)));


	//float ray1_limit_dot_product = 0.0;
	//
	//__m256 _l2length;
	//__m256 _l2dxsqr, _l2dysqr;
	//__m256 _l2extension;
	//__m256 _l1_limit_dot_product, _l2_limit_dot_product;
	//__m256 _dx_l1p1_to_inter, _dx_l1p2_to_inter, _dx_l2p1_to_inter, _dx_l2p2_to_inter;
	//__m256 _dy_l1p1_to_inter, _dy_l1p2_to_inter, _dy_l2p1_to_inter, _dy_l2p2_to_inter;
	//__m256 _l1_dot_product, _l2_dot_product;
	//
	//_l1_limit_dot_product = _mm256_set1_ps(ray1_limit_dot_product);
	//
	//_l2dxsqr = _mm256_mul_ps(_dx2, _dx2);
	//_l2dysqr = _mm256_mul_ps(_dy2, _dy2);
	//_l2length = _mm256_add_ps(_l2dxsqr, _l2dysqr);
	//_l2length = _mm256_sqrt_ps(_l2length);
	//
	//_l2extension = _mm256_set1_ps(SURFACES_EXTENSION);
	//_l2_limit_dot_product = _mm256_add_ps(_l2extension, _l2length);
	//_l2_limit_dot_product = _mm256_mul_ps(_l2_limit_dot_product, _l2extension);
	//
	//_dx_l1p1_to_inter = _mm256_sub_ps(_inter_x, _l1p1x);
	//_dx_l1p2_to_inter = _mm256_sub_ps(_inter_x, _l1p2x);
	//_dx_l2p1_to_inter = _mm256_sub_ps(_inter_x, _l2p1x);
	//_dx_l2p2_to_inter = _mm256_sub_ps(_inter_x, _l2p2x);
	//_dy_l1p1_to_inter = _mm256_sub_ps(_inter_y, _l1p1y);
	//_dy_l1p2_to_inter = _mm256_sub_ps(_inter_y, _l1p2y);
	//_dy_l2p1_to_inter = _mm256_sub_ps(_inter_y, _l2p1y);
	//_dy_l2p2_to_inter = _mm256_sub_ps(_inter_y, _l2p2y);
	//
	//__m256 _product_x_l1_to_inter, _product_y_l1_to_inter;
	//__m256 _product_x_l2_to_inter, _product_y_l2_to_inter;
	//
	//_product_x_l1_to_inter = _mm256_mul_ps(_dx_l1p1_to_inter, _dx_l1p2_to_inter);
	//_product_y_l1_to_inter = _mm256_mul_ps(_dy_l1p1_to_inter, _dy_l1p2_to_inter);
	//_product_x_l2_to_inter = _mm256_mul_ps(_dx_l2p1_to_inter, _dx_l2p2_to_inter);
	//_product_y_l2_to_inter = _mm256_mul_ps(_dy_l2p1_to_inter, _dy_l2p2_to_inter);
	//
	//_l1_dot_product = _mm256_add_ps(_product_x_l1_to_inter, _product_y_l1_to_inter);
	//_l2_dot_product = _mm256_add_ps(_product_x_l2_to_inter, _product_y_l2_to_inter);
	//
	//__m256 _point_lies_on_l1, _point_lies_on_l2;
	//
	//_point_lies_on_l1 = _mm256_cmp_ps(_l1_dot_product, _l1_limit_dot_product, _CMP_LT_OQ);
	//_point_lies_on_l2 = _mm256_cmp_ps(_l2_dot_product, _l2_limit_dot_product, _CMP_LT_OQ);
	//
	//__m256 _inter_exists_and_lies_on_lines;
	//_inter_exists_and_lies_on_lines = _mm256_and_ps(collision_info._intersect, _point_lies_on_l1);
	//_inter_exists_and_lies_on_lines = _mm256_and_ps(_inter_exists_and_lies_on_lines, _point_lies_on_l2);
	//collision_info._intersect = _mm256_or_ps(collision_info._coincide, _inter_exists_and_lies_on_lines);

	return collision_info;

	//CollisionInfo collision_info = LineVsLine(surface1, surface2, intersectionPoint);
	//
	//if (collision_info.coincide)
	//{
	//	return collision_info;
	//}
	//
	//float surface1_length = (surface1.p2 - surface1.p1).mag();
	//float surface2_length = (surface2.p2 - surface2.p1).mag();
	//float surface1_limit_dot_product = surface1.extension * (surface1_length + surface1.extension);
	//float surface2_limit_dot_product = surface2.extension * (surface2_length + surface2.extension);
	//bool point_lies_on_surface1 = (intersectionPoint - surface1.p1).dot(intersectionPoint - surface1.p2) < surface1_limit_dot_product;
	//bool point_lies_on_surface2 = (intersectionPoint - surface2.p1).dot(intersectionPoint - surface2.p2) < surface2_limit_dot_product;
	//
	//if (collision_info.intersect && point_lies_on_surface1 && point_lies_on_surface2)
	//{
	//	return collision_info;
	//}
	//
	//return CollisionInfo(false, false);
}