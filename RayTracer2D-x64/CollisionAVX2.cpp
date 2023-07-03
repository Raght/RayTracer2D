#include "CollisionAVX2.h"


std::vector<CollisionInfo> _Ray1VsSurface4(const Ray& ray1, const std::vector<Surface>& surfaces2, int index_first_surface, std::vector<olc::vd2d>& intersectionPoints)
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

	__m256d _minus_one, _MINUS_EPSILON, _EPSILON, _MINUS_INFINITY, _INFINITY, _MINUS_NAN, _NAN;

	_minus_one = _mm256_set1_pd(1.0);
	_MINUS_EPSILON = _mm256_set1_pd(-EPSILON);
	_EPSILON = _mm256_set1_pd(EPSILON);
	_MINUS_INFINITY = _mm256_set1_pd((double)-INFINITY);
	_INFINITY = _mm256_set1_pd((double)INFINITY);
	_MINUS_NAN = _mm256_set1_pd((double)-NAN);
	_NAN = _mm256_set1_pd((double)NAN);

	
	__m256d _l1p1x, _l1p1y, _l1p2x, _l1p2y, _l2p1x, _l2p1y, _l2p2x, _l2p2y;
	__m256d _dx1, _dx2, _dy1, _dy2;
	__m256d _a1, _a2, _b1, _b2;
	__m256d _a1gtmEPS_mask, _a1ltEPS_mask, _a2gtmEPS_mask, _a2ltEPS_mask;
	__m256d _a1eqpINF_mask, _a1eqmINF_mask, _a2eqpINF_mask, _a2eqmINF_mask;
	__m256d _a1eqINF_mask, _a2eqINF_mask;
	__m256d _a1eqpNAN_mask, _a1eqmNAN_mask, _a2eqpNAN_mask, _a2eqmNAN_mask;
	__m256d _a1eqNAN_mask, _a2eqNAN_mask;
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
	_a1eqmNAN_mask = (_mm256_cmp_pd(_a1, _MINUS_NAN, _CMP_EQ_OQ));
	_a1eqpNAN_mask = (_mm256_cmp_pd(_a1, _NAN, _CMP_EQ_OQ));
	_a2eqmNAN_mask = (_mm256_cmp_pd(_a2, _MINUS_NAN, _CMP_EQ_OQ));
	_a2eqpNAN_mask = (_mm256_cmp_pd(_a2, _NAN, _CMP_EQ_OQ));

	_a1eqINF_mask = _mm256_or_pd(_a1eqmINF_mask, _a1eqpINF_mask);
	_a2eqINF_mask = _mm256_or_pd(_a2eqmINF_mask, _a2eqpINF_mask);
	_a1eqNAN_mask = _mm256_or_pd(_a1eqmNAN_mask, _a1eqpNAN_mask);
	_a2eqNAN_mask = _mm256_or_pd(_a2eqmNAN_mask, _a2eqpNAN_mask);

	_l1vertical = _mm256_castpd_si256(_mm256_or_pd(_a1eqINF_mask, _a1eqNAN_mask));
	_l2vertical = _mm256_castpd_si256(_mm256_or_pd(_a2eqINF_mask, _a2eqNAN_mask));

	std::vector<double> a1_v = { _a1.m256d_f64[3], _a1.m256d_f64[2], _a1.m256d_f64[1], _a1.m256d_f64[0] };
	std::vector<double> a2_v = { _a2.m256d_f64[3], _a2.m256d_f64[2], _a2.m256d_f64[1], _a2.m256d_f64[0] };
	std::vector<double> b1_v = { _b1.m256d_f64[3], _b1.m256d_f64[2], _b1.m256d_f64[1], _b1.m256d_f64[0] };
	std::vector<double> b2_v = { _b2.m256d_f64[3], _b2.m256d_f64[2], _b2.m256d_f64[1], _b2.m256d_f64[0] };
	std::vector<bool> line1_horizontal_v = {
		bool(_l1horizontal.m256i_i64[3]), bool(_l1horizontal.m256i_i64[2]),
		bool(_l1horizontal.m256i_i64[1]), bool(_l1horizontal.m256i_i64[0])
	};
	std::vector<bool> line2_horizontal_v = {
		bool(_l2horizontal.m256i_i64[3]), bool(_l2horizontal.m256i_i64[2]),
		bool(_l2horizontal.m256i_i64[1]), bool(_l2horizontal.m256i_i64[0])
	};
	std::vector<bool> line1_vertical_v = {
		bool(_l1vertical.m256i_i64[3]), bool(_l1vertical.m256i_i64[2]),
		bool(_l1vertical.m256i_i64[1]), bool(_l1vertical.m256i_i64[0])
	};
	std::vector<bool> line2_vertical_v = {
		bool(_l2vertical.m256i_i64[3]), bool(_l2vertical.m256i_i64[2]),
		bool(_l2vertical.m256i_i64[1]), bool(_l2vertical.m256i_i64[0])
	};

	std::vector<CollisionInfo> collision_infos(4);

	for (int i = 0; i < 4; i++)
	{
		bool line1_horizontal = line1_horizontal_v[i];
		bool line2_horizontal = line2_horizontal_v[i];
		bool line1_vertical = line1_vertical_v[i];
		bool line2_vertical = line2_vertical_v[i];
		const Line& line2 = surfaces2[i];
		double a1 = a1_v[i];
		double a2 = a2_v[i];
		double b1 = b1_v[i];
		double b2 = b2_v[i];
		olc::vd2d& intersection_point = intersectionPoints[i];
		double& x0 = intersection_point.x, y0 = intersection_point.y;
		CollisionInfo& collision_info = collision_infos[i];

		if (line1_horizontal)
		{
			if (line2_horizontal)
			{
				bool intersect_and_coincide = Equal(ray1.origin.y, line2.p1.y);
				collision_info = CollisionInfo(intersect_and_coincide, intersect_and_coincide);
			}
			else if (line2_vertical)
			{
				x0 = line2.p1.x;
				y0 = ray1.origin.y;
				collision_info = CollisionInfo(true, false);
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

				collision_info = CollisionInfo(true, false);
			}
		}
		else if (line1_vertical)
		{
			if (line2_horizontal)
			{
				x0 = ray1.origin.x;
				y0 = line2.p1.y;
				collision_info = CollisionInfo(true, false);
			}
			else if (line2_vertical)
			{
				bool intersect_and_coincide = Equal(ray1.origin.x, line2.p1.x);
				collision_info = CollisionInfo(intersect_and_coincide, intersect_and_coincide);
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

				collision_info = CollisionInfo(true, false);
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

				collision_info = CollisionInfo(true, false);
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

				collision_info = CollisionInfo(true, false);
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
					collision_info = CollisionInfo(intersect_and_coincide, intersect_and_coincide);
				}
				else
				{
					x0 = (b2 - b1) / (a1 - a2);
					y0 = a1 * x0 + b1;

					collision_info = CollisionInfo(true, false);
				}
			}
		}
	}

	__m256d _interx, _intery;
	_interx = _mm256_set_pd(
		intersectionPoints[0].x,
		intersectionPoints[1].x,
		intersectionPoints[2].x,
		intersectionPoints[3].x
	);
	_intery = _mm256_set_pd(
		intersectionPoints[0].y,
		intersectionPoints[1].y,
		intersectionPoints[2].y,
		intersectionPoints[3].y
	);

	CollisionInfoAVXRegisters collision_infos_registers;
	collision_infos_registers.intersect = _mm256_set_epi64x(
		collision_infos[0].intersect,
		collision_infos[1].intersect,
		collision_infos[2].intersect,
		collision_infos[3].intersect
	);
	collision_infos_registers.coincide = _mm256_set_epi64x(
		collision_infos[0].coincide,
		collision_infos[1].coincide,
		collision_infos[2].coincide,
		collision_infos[3].coincide
	);

	double ray1_limit_dot_product = 0.0;
	
	__m256d _l2length;
	__m256d _l2dxsqr, _l2dysqr;
	__m256d _l2extension;
	__m256d _l1_limit_dot_product, _l2_limit_dot_product;
	__m256d _dx_l1p1_to_inter, _dx_l1p2_to_inter, _dx_l2p1_to_inter, _dx_l2p2_to_inter;
	__m256d _dy_l1p1_to_inter, _dy_l1p2_to_inter, _dy_l2p1_to_inter, _dy_l2p2_to_inter;
	__m256d _l1_dot_product, _l2_dot_product;
	
	_l1_limit_dot_product = _mm256_set1_pd(ray1_limit_dot_product);
	_l2length = _mm256_set1_pd(0.0);

	_l2dxsqr = _mm256_mul_pd(_dx2, _dx2);
	_l2dysqr = _mm256_mul_pd(_dy2, _dy2);
	_l2length = _mm256_add_pd(_l2dxsqr, _l2dysqr);
	_l2length = _mm256_sqrt_pd(_l2length);

	_l2extension = _mm256_set_pd(surfaces2[0].extension, surfaces2[1].extension, surfaces2[2].extension, surfaces2[3].extension);
	_l2_limit_dot_product = _mm256_add_pd(_l2extension, _l2length);
	_l2_limit_dot_product = _mm256_mul_pd(_l2_limit_dot_product, _l2extension);

	_dx_l1p1_to_inter = _mm256_sub_pd(_interx, _l1p1x);
	_dx_l1p2_to_inter = _mm256_sub_pd(_interx, _l1p2x);
	_dx_l2p1_to_inter = _mm256_sub_pd(_interx, _l2p1x);
	_dx_l2p2_to_inter = _mm256_sub_pd(_interx, _l2p2x);
	_dy_l1p1_to_inter = _mm256_sub_pd(_intery, _l1p1y);
	_dy_l1p2_to_inter = _mm256_sub_pd(_intery, _l1p2y);
	_dy_l2p1_to_inter = _mm256_sub_pd(_intery, _l2p1y);
	_dy_l2p2_to_inter = _mm256_sub_pd(_intery, _l2p2y);

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
	_inter_exists_and_lies_on_lines = _mm256_and_pd(_mm256_castsi256_pd(collision_infos_registers.intersect), _point_lies_on_l1);
	_inter_exists_and_lies_on_lines = _mm256_and_pd(_inter_exists_and_lies_on_lines, _point_lies_on_l2);
	collision_infos_registers.intersect = _mm256_castpd_si256(_mm256_or_pd(_mm256_castsi256_pd(collision_infos_registers.coincide), _mm256_castsi256_pd(collision_infos_registers.intersect)));

	collision_infos[0].intersect = collision_infos_registers.intersect.m256i_i64[3];
	collision_infos[1].intersect = collision_infos_registers.intersect.m256i_i64[2];
	collision_infos[2].intersect = collision_infos_registers.intersect.m256i_i64[1];
	collision_infos[3].intersect = collision_infos_registers.intersect.m256i_i64[0];

	collision_infos[0].coincide = collision_infos_registers.coincide.m256i_i64[3];
	collision_infos[1].coincide = collision_infos_registers.coincide.m256i_i64[2];
	collision_infos[2].coincide = collision_infos_registers.coincide.m256i_i64[1];
	collision_infos[3].coincide = collision_infos_registers.coincide.m256i_i64[0];

	return collision_infos;

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
	//return CollisionInfo(false, false);=
}

