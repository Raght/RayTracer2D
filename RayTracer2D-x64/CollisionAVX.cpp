#include "CollisionAVX.h"


inline __m256d _mm256_xnor_pd(const __m256d& _a, const __m256d& _b)
{
	return _mm256_cmp_pd(_a, _b, _CMP_EQ_OQ);
}

inline __m256d _mm256_nor_pd(const __m256d& _a, const __m256d& _b)
{
	return _mm256_xor_pd(_mm256_and_pd(_a, _b), _mm256_cmp_pd(_a, _b, _CMP_EQ_OQ));
}

inline __m256d _mm256_not_pd(const __m256d& _a)
{
	return _mm256_nor_pd(_a, _a);
}

inline __m256d _mm256_eq_zero_pd(const __m256d& _a, const __m256d& _minus_bound, const __m256d& _bound)
{
	__m256d _gt_m_bound = _mm256_cmp_pd(_minus_bound, _a, _CMP_LT_OQ);
	__m256d _lt_bound = _mm256_cmp_pd(_a, _bound, _CMP_LT_OQ);
	return _mm256_and_pd(_gt_m_bound, _lt_bound);
}



#if AVX_OPTIMIZED
CollisionInfoAVXRegisters _Ray1VsSurface4(const Ray& ray1, const std::vector<Surface>& surfaces2, int index_first_surface, __m256d& _inter_x, __m256d& _inter_y)
{
	/*
		{ a1 * x + b1 = y
		{ a2 * x + b2 = y
	*/

	//double dx1 = line1.p2.x - line1.p1.x;
	//double dx2 = line2.p2.x - line2.p1.x;
	//double dy1 = line1.p2.y - line1.p1.y;
	//double dy2 = line2.p2.y - line2.p1.y;
	//
	//double a1 = dy1 / dx1;
	//double a2 = dy2 / dx2;
	//double b1 = line1.p1.y - a1 * line1.p1.x;
	//double b2 = line2.p1.y - a2 * line2.p1.x;

	__m256d _minus_one, _MINUS_EPSILON, _EPSILON, _MINUS_INFINITY, _INFINITY;

	_minus_one = _mm256_set1_pd(-1.0);
	_MINUS_EPSILON = _mm256_set1_pd(-EPSILON);
	_EPSILON = _mm256_set1_pd(EPSILON);
	_MINUS_INFINITY = _mm256_set1_pd((double)-INFINITY);
	_INFINITY = _mm256_set1_pd((double)INFINITY);

	__m256d _l1p1x, _l1p1y, _l1p2x, _l1p2y, _l2p1x, _l2p1y, _l2p2x, _l2p2y;
	__m256d _dx1, _dx2, _dy1, _dy2;
	__m256d _a1, _a2, _b1, _b2;
	__m256d _a1eqpINF_mask, _a1eqmINF_mask, _a2eqpINF_mask, _a2eqmINF_mask;
	__m256d _l1horizontal, _l2horizontal, _l1vertical, _l2vertical;

	int j = index_first_surface;
	_l1p1x = _mm256_set1_pd(ray1.origin.x);
	_l1p1y = _mm256_set1_pd(ray1.origin.y);
	_l1p2x = _mm256_set1_pd(ray1.EndPoint().x);
	_l1p2y = _mm256_set1_pd(ray1.EndPoint().y);
	_l2p1x = _mm256_set_pd(surfaces2[j].p1.x, surfaces2[j + 1].p1.x, surfaces2[j + 2].p1.x, surfaces2[j + 3].p1.x);
	_l2p1y = _mm256_set_pd(surfaces2[j].p1.y, surfaces2[j + 1].p1.y, surfaces2[j + 2].p1.y, surfaces2[j + 3].p1.y);
	_l2p2x = _mm256_set_pd(surfaces2[j].p2.x, surfaces2[j + 1].p2.x, surfaces2[j + 2].p2.x, surfaces2[j + 3].p2.x);
	_l2p2y = _mm256_set_pd(surfaces2[j].p2.y, surfaces2[j + 1].p2.y, surfaces2[j + 2].p2.y, surfaces2[j + 3].p2.y);

	_dx1 = _mm256_sub_pd(_l1p2x, _l1p1x);
	_dx2 = _mm256_sub_pd(_l2p2x, _l2p1x);
	_dy1 = _mm256_sub_pd(_l1p2y, _l1p1y);
	_dy2 = _mm256_sub_pd(_l2p2y, _l2p1y);

	_a1 = _mm256_div_pd(_dy1, _dx1);
	_a2 = _mm256_div_pd(_dy2, _dx2);

	//_b1 = _mm256_fnmadd_pd(_a1, _l1p1x, _l1p1y);
	//_b2 = _mm256_fnmadd_pd(_a2, _l2p1x, _l2p1y);

	_b1 = _mm256_mul_pd(_a1, _l1p1x);
	_b1 = _mm256_mul_pd(_b1, _minus_one);
	_b1 = _mm256_add_pd(_b1, _l1p1y);
	_b2 = _mm256_mul_pd(_a2, _l2p1x);
	_b2 = _mm256_mul_pd(_b2, _minus_one);
	_b2 = _mm256_add_pd(_b2, _l2p1y);

	_l1horizontal = _mm256_eq_zero_pd(_a1, _MINUS_EPSILON, _EPSILON);
	_l2horizontal = _mm256_eq_zero_pd(_a2, _MINUS_EPSILON, _EPSILON);

	_a1eqmINF_mask = (_mm256_cmp_pd(_a1, _MINUS_INFINITY, _CMP_EQ_OQ));
	_a1eqpINF_mask = (_mm256_cmp_pd(_a1, _INFINITY, _CMP_EQ_OQ));
	_a2eqmINF_mask = (_mm256_cmp_pd(_a2, _MINUS_INFINITY, _CMP_EQ_OQ));
	_a2eqpINF_mask = (_mm256_cmp_pd(_a2, _INFINITY, _CMP_EQ_OQ));

	_l1vertical = _mm256_or_pd(_a1eqmINF_mask, _a1eqpINF_mask);
	_l2vertical = _mm256_or_pd(_a2eqmINF_mask, _a2eqpINF_mask);

	__m256d _l1angle, _l2angle;
	_l1angle = _mm256_nor_pd(_l1horizontal, _l1vertical);
	_l2angle = _mm256_nor_pd(_l2horizontal, _l2vertical);



	CollisionInfoAVXRegisters collision_info;

	uint64_t all_ones_int = UINT64_MAX;
	double all_ones = *(double*)(uint64_t*)&all_ones_int;
	__m256d _all_ones = _mm256_set1_pd(all_ones);

	_inter_x = _mm256_setzero_pd();
	_inter_y = _mm256_setzero_pd();

	__m256d _hh, _hv, _ha, _vh, _vv, _va, _ah, _av, _aa;
	_hh = _mm256_and_pd(_l1horizontal, _l2horizontal);
	_hv = _mm256_and_pd(_l1horizontal, _l2vertical);
	_ha = _mm256_and_pd(_l1horizontal, _l2angle);
	_vh = _mm256_and_pd(_l1vertical, _l2horizontal);
	_vv = _mm256_and_pd(_l1vertical, _l2vertical);
	_va = _mm256_and_pd(_l1vertical, _l2angle);
	_ah = _mm256_and_pd(_l1angle, _l2horizontal);
	_av = _mm256_and_pd(_l1angle, _l2vertical);
	_aa = _mm256_and_pd(_l1angle, _l2angle);

	collision_info._intersect = _all_ones;
	collision_info._coincide = _mm256_setzero_pd();

	// hh
	{
		__m256d _intersect_and_coincide_mask = _mm256_eq_zero_pd(_mm256_sub_pd(_l1p1y, _l2p1y), _MINUS_EPSILON, _EPSILON);
		__m256d _intersect_and_coincide_i = _mm256_and_pd(_hh, _intersect_and_coincide_mask);
		collision_info._intersect = _mm256_and_pd(collision_info._intersect, _mm256_not_pd(_hh));
		collision_info._intersect = _mm256_or_pd(collision_info._intersect, _intersect_and_coincide_i);
		collision_info._coincide = _mm256_or_pd(collision_info._coincide, _intersect_and_coincide_i);
	}
	// hv
	_inter_x = _mm256_or_pd(_inter_x, _mm256_and_pd(_hv, _l2p1x));
	_inter_y = _mm256_or_pd(_inter_y, _mm256_and_pd(_hv, _l1p1y));
	// ha
	_inter_x = _mm256_or_pd(_inter_x, _mm256_and_pd(_ha, _mm256_div_pd(_mm256_sub_pd(_l1p1y, _b2), _a2)));
	_inter_y = _mm256_or_pd(_inter_y, _mm256_and_pd(_ha, _l1p1y));
	// vh
	_inter_x = _mm256_or_pd(_inter_x, _mm256_and_pd(_vh, _l1p1x));
	_inter_y = _mm256_or_pd(_inter_y, _mm256_and_pd(_vh, _l2p1y));
	// vv
	{
		
		__m256d _intersect_and_coincide_mask = _mm256_eq_zero_pd(_mm256_sub_pd(_l1p1x, _l2p1x), _MINUS_EPSILON, _EPSILON);
		__m256d _intersect_and_coincide_i = _mm256_and_pd(_vv, _intersect_and_coincide_mask);
		collision_info._intersect = _mm256_and_pd(collision_info._intersect, _mm256_not_pd(_vv));
		collision_info._intersect = _mm256_or_pd(collision_info._intersect, _intersect_and_coincide_i);
		collision_info._coincide = _mm256_or_pd(collision_info._coincide, _intersect_and_coincide_i);
	}
	// va
	_inter_x = _mm256_or_pd(_inter_x, _mm256_and_pd(_va, _l1p1x));
	_inter_y = _mm256_or_pd(_inter_y, _mm256_and_pd(_va, _mm256_add_pd(_mm256_mul_pd(_l1p1x, _a2), _b2)));
	// ah
	_inter_x = _mm256_or_pd(_inter_x, _mm256_and_pd(_ah, _mm256_div_pd(_mm256_sub_pd(_l2p1y, _b1), _a1)));
	_inter_y = _mm256_or_pd(_inter_y, _mm256_and_pd(_ah, _l2p1y));
	// av
	_inter_x = _mm256_or_pd(_inter_x, _mm256_and_pd(_av, _l2p1x));
	_inter_y = _mm256_or_pd(_inter_y, _mm256_and_pd(_av, _mm256_add_pd(_mm256_mul_pd(_l2p1x, _a1), _b1)));
	// aa
	{
		__m256d _a1_eq_a2 = _mm256_eq_zero_pd(_mm256_sub_pd(_a1, _a2), _MINUS_EPSILON, _EPSILON);
		__m256d _b1_eq_b2 = _mm256_eq_zero_pd(_mm256_sub_pd(_b1, _b2), _MINUS_EPSILON, _EPSILON);
		__m256d _intersect_and_coincide_mask = _mm256_and_pd(_a1_eq_a2, _b1_eq_b2);
		__m256d _intersect_and_coincide_i = _mm256_and_pd(_aa, _intersect_and_coincide_mask);
		collision_info._intersect = _mm256_and_pd(collision_info._intersect, _mm256_not_pd(_aa));
		collision_info._intersect = _mm256_or_pd(collision_info._intersect, _intersect_and_coincide_i);
		collision_info._coincide = _mm256_or_pd(collision_info._coincide, _intersect_and_coincide_i);
	}
	_inter_x = _mm256_or_pd(_inter_x, _mm256_and_pd(_aa, _mm256_div_pd(_mm256_sub_pd(_b2, _b1), _mm256_sub_pd(_a1, _a2))));
	_inter_y = _mm256_or_pd(_inter_y, _mm256_and_pd(_aa, _mm256_add_pd(_mm256_mul_pd(_inter_x, _a1), _b1)));
	

	double ray1_limit_dot_product = 0.0;

	__m256d _l2length;
	__m256d _l2dxsqr, _l2dysqr;
	__m256d _l2extension;
	__m256d _l1_limit_dot_product, _l2_limit_dot_product;
	__m256d _dx_l1p1_to_inter, _dx_l1p2_to_inter, _dx_l2p1_to_inter, _dx_l2p2_to_inter;
	__m256d _dy_l1p1_to_inter, _dy_l1p2_to_inter, _dy_l2p1_to_inter, _dy_l2p2_to_inter;
	__m256d _l1_dot_product, _l2_dot_product;

	_l1_limit_dot_product = _mm256_set1_pd(ray1_limit_dot_product);

	_l2dxsqr = _mm256_mul_pd(_dx2, _dx2);
	_l2dysqr = _mm256_mul_pd(_dy2, _dy2);
	_l2length = _mm256_add_pd(_l2dxsqr, _l2dysqr);
	_l2length = _mm256_sqrt_pd(_l2length);

	_l2extension = _mm256_set_pd(surfaces2[j].extension, surfaces2[j + 1].extension, surfaces2[j + 2].extension, surfaces2[j + 3].extension);
	_l2_limit_dot_product = _mm256_add_pd(_l2extension, _l2length);
	_l2_limit_dot_product = _mm256_mul_pd(_l2_limit_dot_product, _l2extension);

	_dx_l1p1_to_inter = _mm256_sub_pd(_inter_x, _l1p1x);
	_dx_l1p2_to_inter = _mm256_sub_pd(_inter_x, _l1p2x);
	_dx_l2p1_to_inter = _mm256_sub_pd(_inter_x, _l2p1x);
	_dx_l2p2_to_inter = _mm256_sub_pd(_inter_x, _l2p2x);
	_dy_l1p1_to_inter = _mm256_sub_pd(_inter_y, _l1p1y);
	_dy_l1p2_to_inter = _mm256_sub_pd(_inter_y, _l1p2y);
	_dy_l2p1_to_inter = _mm256_sub_pd(_inter_y, _l2p1y);
	_dy_l2p2_to_inter = _mm256_sub_pd(_inter_y, _l2p2y);

	__m256d _product_x_l1_to_inter, _product_y_l1_to_inter;
	__m256d _product_x_l2_to_inter, _product_y_l2_to_inter;

	_product_x_l1_to_inter = _mm256_mul_pd(_dx_l1p1_to_inter, _dx_l1p2_to_inter);
	_product_y_l1_to_inter = _mm256_mul_pd(_dy_l1p1_to_inter, _dy_l1p2_to_inter);
	_product_x_l2_to_inter = _mm256_mul_pd(_dx_l2p1_to_inter, _dx_l2p2_to_inter);
	_product_y_l2_to_inter = _mm256_mul_pd(_dy_l2p1_to_inter, _dy_l2p2_to_inter);

	_l1_dot_product = _mm256_add_pd(_product_x_l1_to_inter, _product_y_l1_to_inter);
	_l2_dot_product = _mm256_add_pd(_product_x_l2_to_inter, _product_y_l2_to_inter);

	__m256d _point_lies_on_l1, _point_lies_on_l2;

	_point_lies_on_l1 = _mm256_cmp_pd(_l1_dot_product, _l1_limit_dot_product, _CMP_LT_OQ);
	_point_lies_on_l2 = _mm256_cmp_pd(_l2_dot_product, _l2_limit_dot_product, _CMP_LT_OQ);

	__m256d _inter_exists_and_lies_on_lines;
	_inter_exists_and_lies_on_lines = _mm256_and_pd(collision_info._intersect, _point_lies_on_l1);
	_inter_exists_and_lies_on_lines = _mm256_and_pd(_inter_exists_and_lies_on_lines, _point_lies_on_l2);
	collision_info._intersect = _mm256_or_pd(collision_info._coincide, _inter_exists_and_lies_on_lines);

	return collision_info;

	//CollisionInfo collision_info = LineVsLine(surface1, surface2, intersectionPoint);
	//
	//if (collision_info.coincide)
	//{
	//	return collision_info;
	//}
	//
	//double surface1_length = (surface1.p2 - surface1.p1).mag();
	//double surface2_length = (surface2.p2 - surface2.p1).mag();
	//double surface1_limit_dot_product = surface1.extension * (surface1_length + surface1.extension);
	//double surface2_limit_dot_product = surface2.extension * (surface2_length + surface2.extension);
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
#else
CollisionInfoAVXRegisters _Ray1VsSurface4(const Ray& ray1, const std::vector<Surface>& surfaces2, int index_first_surface, __m256d& _inter_x, __m256d& _inter_y)
{
	/*
		{ a1 * x + b1 = y
		{ a2 * x + b2 = y
	*/

	//double dx1 = line1.p2.x - line1.p1.x;
	//double dx2 = line2.p2.x - line2.p1.x;
	//double dy1 = line1.p2.y - line1.p1.y;
	//double dy2 = line2.p2.y - line2.p1.y;
	//
	//double a1 = dy1 / dx1;
	//double a2 = dy2 / dx2;
	//double b1 = line1.p1.y - a1 * line1.p1.x;
	//double b2 = line2.p1.y - a2 * line2.p1.x;

	__m256d _minus_one, _MINUS_EPSILON, _EPSILON, _MINUS_INFINITY, _INFINITY;

	_minus_one = _mm256_set1_pd(-1.0);
	_MINUS_EPSILON = _mm256_set1_pd(-EPSILON);
	_EPSILON = _mm256_set1_pd(EPSILON);
	_MINUS_INFINITY = _mm256_set1_pd((double)-INFINITY);
	_INFINITY = _mm256_set1_pd((double)INFINITY);

	__m256d _l1p1x, _l1p1y, _l1p2x, _l1p2y, _l2p1x, _l2p1y, _l2p2x, _l2p2y;
	__m256d _dx1, _dx2, _dy1, _dy2;
	__m256d _a1, _a2, _b1, _b2;
	__m256d _a1gtmEPS_mask, _a1ltEPS_mask, _a2gtmEPS_mask, _a2ltEPS_mask;
	__m256d _a1eqpINF_mask, _a1eqmINF_mask, _a2eqpINF_mask, _a2eqmINF_mask;
	__m256d _a1eqINF_mask, _a2eqINF_mask;
	__m256i _l1horizontal, _l2horizontal, _l1vertical, _l2vertical;

	int j = index_first_surface;
	_l1p1x = _mm256_set1_pd(ray1.origin.x);
	_l1p1y = _mm256_set1_pd(ray1.origin.y);
	_l1p2x = _mm256_set1_pd(ray1.EndPoint().x);
	_l1p2y = _mm256_set1_pd(ray1.EndPoint().y);
	_l2p1x = _mm256_set_pd(surfaces2[j].p1.x, surfaces2[j + 1].p1.x, surfaces2[j + 2].p1.x, surfaces2[j + 3].p1.x);
	_l2p1y = _mm256_set_pd(surfaces2[j].p1.y, surfaces2[j + 1].p1.y, surfaces2[j + 2].p1.y, surfaces2[j + 3].p1.y);
	_l2p2x = _mm256_set_pd(surfaces2[j].p2.x, surfaces2[j + 1].p2.x, surfaces2[j + 2].p2.x, surfaces2[j + 3].p2.x);
	_l2p2y = _mm256_set_pd(surfaces2[j].p2.y, surfaces2[j + 1].p2.y, surfaces2[j + 2].p2.y, surfaces2[j + 3].p2.y);

	_dx1 = _mm256_sub_pd(_l1p2x, _l1p1x);
	_dx2 = _mm256_sub_pd(_l2p2x, _l2p1x);
	_dy1 = _mm256_sub_pd(_l1p2y, _l1p1y);
	_dy2 = _mm256_sub_pd(_l2p2y, _l2p1y);

	_a1 = _mm256_div_pd(_dy1, _dx1);
	_a2 = _mm256_div_pd(_dy2, _dx2);

	//_b1 = _mm256_fnmadd_pd(_a1, _l1p1x, _l1p1y);
	//_b2 = _mm256_fnmadd_pd(_a2, _l2p1x, _l2p1y);

	_b1 = _mm256_mul_pd(_a1, _l1p1x);
	_b1 = _mm256_mul_pd(_b1, _minus_one);
	_b1 = _mm256_add_pd(_b1, _l1p1y);
	_b2 = _mm256_mul_pd(_a2, _l2p1x);
	_b2 = _mm256_mul_pd(_b2, _minus_one);
	_b2 = _mm256_add_pd(_b2, _l2p1y);


	_a1gtmEPS_mask = (_mm256_cmp_pd(_MINUS_EPSILON, _a1, _CMP_LT_OQ));
	_a1ltEPS_mask = (_mm256_cmp_pd(_a1, _EPSILON, _CMP_LT_OQ));
	_a2gtmEPS_mask = (_mm256_cmp_pd(_MINUS_EPSILON, _a2, _CMP_LT_OQ));
	_a2ltEPS_mask = (_mm256_cmp_pd(_a2, _EPSILON, _CMP_LT_OQ));
	_l1horizontal = _mm256_castpd_si256(_mm256_and_pd(_a1gtmEPS_mask, _a1ltEPS_mask));
	_l2horizontal = _mm256_castpd_si256(_mm256_and_pd(_a2gtmEPS_mask, _a2ltEPS_mask));

	_a1eqmINF_mask = (_mm256_cmp_pd(_a1, _MINUS_INFINITY, _CMP_EQ_OQ));
	_a1eqpINF_mask = (_mm256_cmp_pd(_a1, _INFINITY, _CMP_EQ_OQ));
	_a2eqmINF_mask = (_mm256_cmp_pd(_a2, _MINUS_INFINITY, _CMP_EQ_OQ));
	_a2eqpINF_mask = (_mm256_cmp_pd(_a2, _INFINITY, _CMP_EQ_OQ));

	_a1eqINF_mask = _mm256_or_pd(_a1eqmINF_mask, _a1eqpINF_mask);
	_a2eqINF_mask = _mm256_or_pd(_a2eqmINF_mask, _a2eqpINF_mask);

	_l1vertical = _mm256_castpd_si256(_a1eqINF_mask);
	_l2vertical = _mm256_castpd_si256(_a2eqINF_mask);

	CollisionInfoAVXRegisters collision_info;
	__m256d _first_mask, _second_mask, _third_mask, _fourth_mask;

	uint64_t all_ones_int = UINT64_MAX;
	double all_ones = *(double*)(uint64_t*)&all_ones_int;

	_first_mask = _mm256_set_pd(all_ones, 0, 0, 0);
	_second_mask = _mm256_set_pd(0, all_ones, 0, 0);
	_third_mask = _mm256_set_pd(0, 0, all_ones, 0);
	_fourth_mask = _mm256_set_pd(0, 0, 0, all_ones);

	_inter_x = _mm256_setzero_pd();
	_inter_y = _mm256_setzero_pd();

	//double x0_1 = 0, x0_2 = 0, x0_3 = 0, x0_4 = 0;
	//double y0_1 = 0, y0_2 = 0, y0_3 = 0, y0_4 = 0;
	//bool intersect1, intersect2, intersect3, intersect4;
	//bool coincide1, coincide2, coincide3, coincide4;


	for (int i = 0; i < 4; i++)
	{
		bool line1_horizontal = bool(_l1horizontal.m256i_i64[3 - i]);
		bool line2_horizontal = bool(_l2horizontal.m256i_i64[3 - i]);
		bool line1_vertical = bool(_l1vertical.m256i_i64[3 - i]);
		bool line2_vertical = bool(_l2vertical.m256i_i64[3 - i]);
		const Line& line2 = surfaces2[index_first_surface + i];
		double a1 = _a1.m256d_f64[3 - i];
		double a2 = _a2.m256d_f64[3 - i];
		double b1 = _b1.m256d_f64[3 - i];
		double b2 = _b2.m256d_f64[3 - i];
		double x0 = 0, y0 = 0;
		__m256d& _index_mask = _first_mask;
		if (i == 1)
		{
			_index_mask = _second_mask;
		}
		else if (i == 2)
		{
			_index_mask = _third_mask;
		}
		else if (i == 3)
		{
			_index_mask = _fourth_mask;
		}


		bool intersect, coincide;

		if (line1_horizontal)
		{
			if (line2_horizontal)
			{
				bool intersect_and_coincide = Equal(ray1.origin.y, line2.p1.y);
				intersect = intersect_and_coincide;
				coincide = intersect_and_coincide;
			}
			else if (line2_vertical)
			{
				x0 = line2.p1.x;
				y0 = ray1.origin.y;
				intersect = true;
				coincide = false;
			}
			else
			{
				/*
					{ y = c
					{ a2 * x + b2 = y

					x0 = (c - b2) / a2
					y0 = c
				*/

				x0 = (ray1.origin.y - b2) / a2;
				y0 = ray1.origin.y;

				intersect = true;
				coincide = false;
			}
		}
		else if (line1_vertical)
		{
			if (line2_horizontal)
			{
				x0 = ray1.origin.x;
				y0 = line2.p1.y;
				intersect = true;
				coincide = false;
			}
			else if (line2_vertical)
			{
				bool intersect_and_coincide = Equal(ray1.origin.x, line2.p1.x);
				intersect = intersect_and_coincide;
				coincide = intersect_and_coincide;
			}
			else
			{
				/*
					{ x = c
					{ a2 * x + b2 = y

					x0 = c
					y0 = a2 * c + b2
				*/

				x0 = ray1.origin.x;
				y0 = a2 * ray1.origin.x + b2;

				intersect = true;
				coincide = false;
			}
		}
		else
		{
			if (line2_horizontal)
			{
				/*
					{ a1 * x + b1 = y
					{ y = c

					x0 = (c - b1) / a1
					y0 = c
				*/

				x0 = (line2.p1.y - b1) / a1;
				y0 = line2.p1.y;

				intersect = true;
				coincide = false;
			}
			else if (line2_vertical)
			{
				/*
					{ a1 * x + b1 = y
					{ x = c

					x0 = c
					y0 = a2 * c + b2
				*/

				x0 = line2.p1.x;
				y0 = a1 * line2.p1.x + b1;

				intersect = true;
				coincide = false;
			}
			else
			{
				/*
					{ a1 * x + b1 = y
					{ a2 * x + b2 = y

					x0 = (b2 - b1) / (a1 - a2)
					y0 = a1 * x0 + b1
				*/

				if (Equal(a1, a2))
				{
					bool intersect_and_coincide = Equal(b1, b2);
					intersect = intersect_and_coincide;
					coincide = intersect_and_coincide;
				}
				else
				{
					x0 = (b2 - b1) / (a1 - a2);
					y0 = a1 * x0 + b1;

					intersect = true;
					coincide = false;
				}
			}
		}

		double intersect_mask_double = (intersect) ? all_ones : 0.0;
		double coincide_mask_double = (coincide) ? all_ones : 0.0;
		__m256d _intersect_mask = _mm256_set1_pd(intersect_mask_double);
		__m256d _intersect_i = _mm256_and_pd(_intersect_mask, _index_mask);
		__m256d _coincide_mask = _mm256_set1_pd(coincide_mask_double);
		__m256d _coincide_i = _mm256_and_pd(_coincide_mask, _index_mask);
		collision_info._intersect = _mm256_or_pd(collision_info._intersect, _intersect_i);
		collision_info._coincide = _mm256_or_pd(collision_info._coincide, _coincide_i);

		__m256d _inter_x_mask = _mm256_and_pd(_mm256_set1_pd(x0), _index_mask);
		_inter_x = _mm256_or_pd(_inter_x, _inter_x_mask);
		__m256d _inter_y_mask = _mm256_and_pd(_mm256_set1_pd(y0), _index_mask);
		_inter_y = _mm256_or_pd(_inter_y, _inter_y_mask);
	}

	double ray1_limit_dot_product = 0.0;

	__m256d _l2length;
	__m256d _l2dxsqr, _l2dysqr;
	__m256d _l2extension;
	__m256d _l1_limit_dot_product, _l2_limit_dot_product;
	__m256d _dx_l1p1_to_inter, _dx_l1p2_to_inter, _dx_l2p1_to_inter, _dx_l2p2_to_inter;
	__m256d _dy_l1p1_to_inter, _dy_l1p2_to_inter, _dy_l2p1_to_inter, _dy_l2p2_to_inter;
	__m256d _l1_dot_product, _l2_dot_product;

	_l1_limit_dot_product = _mm256_set1_pd(ray1_limit_dot_product);

	_l2dxsqr = _mm256_mul_pd(_dx2, _dx2);
	_l2dysqr = _mm256_mul_pd(_dy2, _dy2);
	_l2length = _mm256_add_pd(_l2dxsqr, _l2dysqr);
	_l2length = _mm256_sqrt_pd(_l2length);

	_l2extension = _mm256_set_pd(surfaces2[0].extension, surfaces2[1].extension, surfaces2[2].extension, surfaces2[3].extension);
	_l2_limit_dot_product = _mm256_add_pd(_l2extension, _l2length);
	_l2_limit_dot_product = _mm256_mul_pd(_l2_limit_dot_product, _l2extension);

	_dx_l1p1_to_inter = _mm256_sub_pd(_inter_x, _l1p1x);
	_dx_l1p2_to_inter = _mm256_sub_pd(_inter_x, _l1p2x);
	_dx_l2p1_to_inter = _mm256_sub_pd(_inter_x, _l2p1x);
	_dx_l2p2_to_inter = _mm256_sub_pd(_inter_x, _l2p2x);
	_dy_l1p1_to_inter = _mm256_sub_pd(_inter_y, _l1p1y);
	_dy_l1p2_to_inter = _mm256_sub_pd(_inter_y, _l1p2y);
	_dy_l2p1_to_inter = _mm256_sub_pd(_inter_y, _l2p1y);
	_dy_l2p2_to_inter = _mm256_sub_pd(_inter_y, _l2p2y);

	__m256d _product_x_l1_to_inter, _product_y_l1_to_inter;
	__m256d _product_x_l2_to_inter, _product_y_l2_to_inter;

	_product_x_l1_to_inter = _mm256_mul_pd(_dx_l1p1_to_inter, _dx_l1p2_to_inter);
	_product_y_l1_to_inter = _mm256_mul_pd(_dy_l1p1_to_inter, _dy_l1p2_to_inter);
	_product_x_l2_to_inter = _mm256_mul_pd(_dx_l2p1_to_inter, _dx_l2p2_to_inter);
	_product_y_l2_to_inter = _mm256_mul_pd(_dy_l2p1_to_inter, _dy_l2p2_to_inter);

	_l1_dot_product = _mm256_add_pd(_product_x_l1_to_inter, _product_y_l1_to_inter);
	_l2_dot_product = _mm256_add_pd(_product_x_l2_to_inter, _product_y_l2_to_inter);

	__m256d _point_lies_on_l1, _point_lies_on_l2;

	_point_lies_on_l1 = _mm256_cmp_pd(_l1_dot_product, _l1_limit_dot_product, _CMP_LT_OQ);
	_point_lies_on_l2 = _mm256_cmp_pd(_l2_dot_product, _l2_limit_dot_product, _CMP_LT_OQ);

	__m256d _inter_exists_and_lies_on_lines;
	_inter_exists_and_lies_on_lines = _mm256_and_pd(collision_info._intersect, _point_lies_on_l1);
	_inter_exists_and_lies_on_lines = _mm256_and_pd(_inter_exists_and_lies_on_lines, _point_lies_on_l2);
	collision_info._intersect = _mm256_or_pd(collision_info._coincide, _inter_exists_and_lies_on_lines);

	return collision_info;

	//CollisionInfo collision_info = LineVsLine(surface1, surface2, intersectionPoint);
	//
	//if (collision_info.coincide)
	//{
	//	return collision_info;
	//}
	//
	//double surface1_length = (surface1.p2 - surface1.p1).mag();
	//double surface2_length = (surface2.p2 - surface2.p1).mag();
	//double surface1_limit_dot_product = surface1.extension * (surface1_length + surface1.extension);
	//double surface2_limit_dot_product = surface2.extension * (surface2_length + surface2.extension);
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
#endif
