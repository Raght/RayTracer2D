#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <execution>
#include "wtypes.h"
#include "Math.h"
#include "Collision.h"
#include "Physics.h"
#include "Ray.h"
#include "Line.h"
#include "Surface.h"
#include "Range.h"
#include "Constants.h"
#include "CollisionAVX.h"


using namespace std;


namespace std
{
	template<>
	struct iterator_traits<RangeIterator>
	{
		using iterator_category = std::random_access_iterator_tag;
		using value_type = int;
		using pointer = int*;
		using reference = int&;
		using difference_type = int;
	};
}


uint64_t float_as_uint64_t(float a)
{
	return *(uint64_t*)&a;
}

float uint64_t_as_float(uint64_t a)
{
	return *(float*)&a;
}

uint32_t float_as_uint32_t(float a)
{
	return *(uint32_t*)&a;
}

float uint32_t_as_float(uint32_t a)
{
	return *(float*)&a;
}



struct Shape
{
	vector<olc::vf2d> points;

	void Draw(olc::PixelGameEngine* pge, olc::Pixel color)
	{

	}
};

olc::vf2d null_point = olc::vf2d(~0, ~0);
Surface null_surface = Surface(olc::vf2d(~0, ~0), olc::vf2d(~0, ~0));


int rays_simulated = 8;
int min_rays_simulated = 2;
int max_rays_simulated = 256;


class Engine2D : public olc::PixelGameEngine
{
public:
	Engine2D()
	{
		sAppName = "2DRayTracer";
	}

private:
	olc::vf2d GetMousePosition()
	{
		return olc::vf2d((float)GetMouseX(), (float)GetMouseY());
	}

	olc::vf2d GetWorldMousePosition()
	{
		return ToWorldSpace(GetMousePosition());
	}

	olc::vi2d ToScreenSpace(const olc::vf2d& world_position)
	{
		olc::vf2d screen_position = { world_position.x * view_scale, (float)ScreenHeight() - world_position.y * view_scale };
		return { (int)round(screen_position.x), (int)round(screen_position.y) };
	}

	olc::vf2d ToWorldSpace(const olc::vi2d& screen_position)
	{
		olc::vf2d world_position = { (float)screen_position.x, float(ScreenHeight() - screen_position.y) };
		world_position /= view_scale;
		return world_position;
	}

	void DrawSurface(const Surface& surface)
	{
		if (surface.is_reflective)
		{
			DrawLine(ToScreenSpace(surface.p1), ToScreenSpace(surface.p2), reflective_surface_color);
		}
		else if (surface.is_refractive)
		{
			DrawLine(ToScreenSpace(surface.p1), ToScreenSpace(surface.p2), refractive_surface_color);
		}
	}

	void DrawNormal(const olc::vf2d& point, const Surface& surface, const Ray& ray)
	{
		DrawLine(ToScreenSpace(point),
			ToScreenSpace(point + UI_scale / view_scale * surface_normal_length * surface.Normal(ray.direction)),
			surface_normal_color);

	}

	void DrawRay(const Ray& ray)
	{
		DrawLine(ToScreenSpace(ray.origin), ToScreenSpace(ray.EndPoint()), ray.Color());
	}

	void DrawRay(const Ray& ray, const olc::vf2d& endPoint)
	{
		DrawLine(ToScreenSpace(ray.origin), ToScreenSpace(endPoint), ray.Color());
	}

	void DrawStringUpLeftCorner(const olc::vi2d& position, const string& text, const olc::Pixel& color)
	{
		DrawString(position, text, color, UI_scale);
	}

	void DrawStringUpRightCorner(const olc::vi2d& position, const string& text, const olc::Pixel& color)
	{
		olc::vi2d pos = { int32_t(position.x - 8 * UI_scale * text.size()), int32_t(position.y) };
		DrawString(pos, text, color, UI_scale);
	}

	void DrawStringBottomRightCorner(const olc::vi2d& position, const string& text, const olc::Pixel& color)
	{
		olc::vi2d pos = { int32_t(position.x - 8 * UI_scale * text.size()), int32_t(position.y - 8 * UI_scale) };
		DrawString(pos, text, color, UI_scale);
	}

	void DrawStringBottomLeftCorner(const olc::vi2d& position, const string& text, const olc::Pixel& color)
	{
		olc::vi2d pos = { position.x, int32_t(position.y - 8 * UI_scale) };
		DrawString(pos, text, color, UI_scale);
	}

	void ExitConstructing()
	{
		is_constructing = false;
		first_point_constructed = false;
	}

	void ExitCutting()
	{
		is_cutting = false;
		cutting_surface_first_point_set = false;
		is_cutting_during_construction = false;
	}

	void AddSurface(const Surface& surface)
	{
		surfaces.push_back(surface);
		segments.push_back(Segment(surface.p1, surface.p2));
	}

	inline void RemoveSurface(int i)
	{
		surfaces.erase(surfaces.begin() + i);
		segments.erase(segments.begin() + i);
	}

	inline void RemoveSurface(std::vector<Surface>::iterator it)
	{
		RemoveSurface(it - surfaces.begin());
	}

	inline void ClearSurfaces()
	{
		surfaces.clear();
		segments.clear();
	}


	bool debug_mode = false;
	bool debug_show_ray_intersections = true;
	bool debug_UI_write_ray_intersections_positions = false;
	int debug_show_ray_intersections_depth = 16;
	bool debug_UI_write_surface_points_positions = false;
	bool debug_UI_write_ray_intersections = false;
	olc::vi2d debug_UI_intersections_screen_position;

	vector<olc::vf2d> debug_rays_intersections;

	bool surfaces_stress_test1 = false;
	bool surfaces_stress_test2 = true;

	
	Ray light_ray;
	vector<Surface> surfaces;
	vector<Segment> segments;
	bool hit_corner = false;
	olc::vf2d corner_position;
	int index_ray_simulated;


	SurfaceType surface_type = SurfaceType::REFLECTIVE;
	Surface surface_in_construction;
	bool first_point_constructed = false;
	bool is_constructing = false;
	float refractive_index = 1.5f;
	float nearest_point_snap_radius = 8;
	olc::vf2d nearest_point;
	olc::vf2d point_to_construct;
	//unordered_map<olc::vf2d, Surface&>


	Surface cutting_surface;
	bool is_cutting = false;
	bool is_cutting_during_construction = false;
	bool cutting_surface_first_point_set = false;
	vector<int> surfaces_to_remove;


	int rays_per_second = 50;
	float ms_per_ray_increase = 1.0f / rays_per_second;
	float timer = 0.0f;

	float refractive_index_step = 0.1f;


	int UI_scale;
	int UI_character_size;
	float view_scale = 1;

	bool full_brightness = false;
	bool draw_normals = true;

	float point_radius = 2;
	olc::Pixel ray_origin_color = olc::RED;
	float inner_circle_origin_radius = 2;
	float outer_circle_origin_radius = 4;
	olc::Pixel ray_color = olc::Pixel(255, 89, 192);
	olc::Pixel reflective_surface_color = olc::WHITE;
	olc::Pixel refractive_surface_color = olc::CYAN;
	olc::Pixel nearest_point_snap_point_color = olc::MAGENTA;
	olc::Pixel nearest_point_snap_circle_color = olc::MAGENTA;
	olc::Pixel cutting_surface_color = olc::RED;
	olc::Pixel surface_to_be_removed_color = olc::DARK_RED;
	olc::Pixel UI_text_color = olc::WHITE;
	olc::Pixel UI_error_text_color = olc::RED;
	olc::Pixel UI_switch_state_on_color = olc::GREEN;
	olc::Pixel UI_switch_state_off_color = olc::RED;
	olc::Pixel surface_normal_color = olc::YELLOW;
	float surface_normal_length = 25.0f;
	

public:
	bool OnUserCreate() override
	{
		light_ray = Ray({ float(ScreenWidth() / 2 / view_scale), float(ScreenHeight() / 2 / view_scale) },
			{ 1, 0 },
			sqrt(float(ScreenWidth() * ScreenWidth() + ScreenHeight() * ScreenHeight())) + 1,
			ray_color,
			1);

		UI_scale = max(int((float)ScreenWidth() / 640), 1);
		UI_character_size = 8 * UI_scale;

		if (debug_mode)
		{
			min_rays_simulated = 1;
		}

		if (surfaces_stress_test1 || surfaces_stress_test2)
		{
			int max_surfaces = 16000;
			int surfaces_counter = 0;
			int offset_x = 200;
			int offset_y = 100;

			int rect_offset_x = 5;
			int rect_offset_y = 20;
			int height = 30;
			olc::vf2d point = { float(offset_x + rect_offset_x), (float)height };
			olc::vf2d size = { float(ScreenWidth() - 2 * offset_x - 2 * rect_offset_x), float(offset_y - height - rect_offset_y) };

			if (surfaces_stress_test1)
			{
				for (int y1 = ScreenHeight() - offset_y; y1 > offset_y && surfaces_counter < max_surfaces; y1--)
				{
					for (int y2 = ScreenHeight() - offset_y; y2 > offset_y && surfaces_counter < max_surfaces; y2--)
					{
						AddSurface(Surface({ (float)offset_x, (float)y1 }, { float(ScreenWidth() - offset_x), (float)y2 }, SurfaceType::REFLECTIVE));
						surfaces_counter++;
					}
				}
			}
			else if (surfaces_stress_test2)
			{
				for (int y = ScreenHeight() - offset_y; y > offset_y && surfaces_counter < max_surfaces; y--)
				{
					for (int x = offset_x; x < ScreenWidth() - offset_x && surfaces_counter < max_surfaces; x++)
					{
						AddSurface(Surface({ (float)x, (float)y }, { float(x + 1), (float)y }, SurfaceType::REFLECTIVE));
						surfaces_counter++;
					}
				}
			}

			light_ray.origin = point + size.vector_y() / 2 + olc::vf2d(2.0f, 0.0f);
			
			max_rays_simulated = 256;
			rays_simulated = max_rays_simulated;
			
			AddSurface(Surface(point, point + size.vector_y(), SurfaceType::REFLECTIVE));
			AddSurface(Surface(point + size.vector_y(), point + size, SurfaceType::REFLECTIVE));
			AddSurface(Surface(point + size, point + size.vector_x(), SurfaceType::REFLECTIVE));
			AddSurface(Surface(point + size.vector_x(), point, SurfaceType::REFLECTIVE));
		}

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::BLACK);

		// Controlling the first ray
		light_ray.direction = olc::vf2d(GetWorldMousePosition() - light_ray.origin).norm();


		if (GetMouse(2).bPressed || GetKey(olc::ESCAPE).bPressed)
		{
			if (!is_cutting_during_construction)
			{
				if (first_point_constructed)
					first_point_constructed = false;
				else
					ExitConstructing();
			}

			if (cutting_surface_first_point_set)
				cutting_surface_first_point_set = false;
			else
				ExitCutting();
		}
		if (GetKey(olc::S).bPressed && !is_cutting)
		{
			//if (is_constructing)
			//{
			//	ExitConstructing();
			//	if (is_cutting_during_construction)
			//		ExitCutting();
			//}
			is_constructing = !is_constructing;
			first_point_constructed = false;
		}
		if (GetKey(olc::R).bPressed)
		{
			if (is_cutting)
			{
				ExitCutting();
			}
			else
			{
				is_cutting = true;
				if (is_constructing)
					is_cutting_during_construction = true;
			}
		}

		cutting_surface.p2 = GetWorldMousePosition();

		surfaces_to_remove.clear();
		if (is_cutting)
		{
			for (int i = 0; i < surfaces.size(); i++)
			{
				if (SurfaceVsSurface(cutting_surface, surfaces[i]).intersect)
				{
					surfaces_to_remove.push_back(i);
				}
			}
		}

		surface_in_construction = Surface(surface_in_construction.p1, GetWorldMousePosition(), surface_type, refractive_index);
		
		bool nearest_point_found = false;
		if (is_constructing && !is_cutting_during_construction)
		{
			vector<olc::vf2d> nearest_points;
			for (Surface& surface : surfaces)
			{
				if ((surface.p1 - GetWorldMousePosition()).mag2() < nearest_point_snap_radius * nearest_point_snap_radius)
					nearest_points.push_back(surface.p1);
				else if ((surface.p2 - GetWorldMousePosition()).mag2() < nearest_point_snap_radius * nearest_point_snap_radius)
					nearest_points.push_back(surface.p2);
			}

			if (nearest_points.size() > 0)
			{
				sort(nearest_points.begin(), nearest_points.end(), 
					[world_mouse_position = GetWorldMousePosition()](olc::vf2d& p1, olc::vf2d& p2)
					{
						return (p1 - world_mouse_position).mag2() < (p2 - world_mouse_position).mag2();
					});

				if (!first_point_constructed)
				{
					nearest_point = nearest_points[0];
					nearest_point_found = true;
					surface_in_construction.p1 = nearest_point;
				}
				else
				{
					for (int i = 0; i < nearest_points.size(); i++)
					{
						if (nearest_points[i] != surface_in_construction.p1)
						{
							nearest_point = nearest_points[i];
							nearest_point_found = true;

							for (Surface& surface : surfaces)
							{
								if (surface == Surface(nearest_points[i], surface_in_construction.p1))
								{
									nearest_point_found = false;
									break;
								}
							}

							if (nearest_point_found)
								break;
						}
					}
				}	
			}
		}

		if (GetMouse(0).bPressed && !is_cutting)
		{
			is_constructing = true;
		}

		if (is_constructing && !is_cutting_during_construction)
		{
			if (nearest_point_found)
				point_to_construct = nearest_point;
			else
				point_to_construct = GetWorldMousePosition();

			if (first_point_constructed)
				surface_in_construction.p2 = point_to_construct;

			surface_in_construction = Surface(surface_in_construction.p1, surface_in_construction.p2, surface_type, refractive_index);

			if (GetMouse(0).bPressed)
			{
				if (!first_point_constructed)
				{
					surface_in_construction.p1 = point_to_construct;
					first_point_constructed = true;
				}
				else
				{
					if (nearest_point_found)
						first_point_constructed = false;
					
					AddSurface(surface_in_construction);
					surface_in_construction.p1 = surface_in_construction.p2;
				}
			}

			if (GetMouse(1).bPressed)
			{
				if (surface_type == SurfaceType::REFLECTIVE)
					surface_type = SurfaceType::REFRACTIVE;
				else if (surface_type == SurfaceType::REFRACTIVE)
					surface_type = SurfaceType::REFLECTIVE;
			}
		}

		if (is_cutting && GetMouse(0).bPressed)
		{
			if (cutting_surface_first_point_set)
			{
				cutting_surface.p2 = GetWorldMousePosition();

				int surfaces_removed = 0;
				for (int i = 0; i < surfaces_to_remove.size(); i++)
				{
					surfaces.erase(surfaces.begin() + surfaces_to_remove[i] - surfaces_removed);

					surfaces_removed++;
				}

				surfaces_to_remove.clear();
			}
			else
			{
				cutting_surface = Surface(GetWorldMousePosition(), GetWorldMousePosition());
			}

			cutting_surface_first_point_set = !cutting_surface_first_point_set;
			
		}
		

		if (is_constructing && !is_cutting_during_construction)
		{
			if (surface_type == SurfaceType::REFRACTIVE)
			{
				if (GetKey(olc::DOWN).bPressed || GetKey(olc::LEFT).bPressed)
					refractive_index -= refractive_index_step;
				if (GetKey(olc::UP).bPressed || GetKey(olc::RIGHT).bPressed)
					refractive_index += refractive_index_step;

				refractive_index = Cap(refractive_index, 1.0f, 999.0f);
			}
		}
		else if (!is_cutting_during_construction)
		{
			timer += fElapsedTime;
			if (timer >= ms_per_ray_increase)
			{
				if (GetKey(olc::DOWN).bHeld)
					rays_simulated -= 1;
				if (GetKey(olc::UP).bHeld)
					rays_simulated += 1;

				timer = 0.0f;
			}
			if (GetKey(olc::LEFT).bPressed)
				rays_simulated -= 1;
			if (GetKey(olc::RIGHT).bPressed)
				rays_simulated += 1;

			rays_simulated = Cap(rays_simulated, min_rays_simulated, max_rays_simulated);


			if (GetMouse(1).bPressed)
				light_ray.origin = GetWorldMousePosition();
		}

		if (GetKey(olc::C).bPressed)
		{
			ClearSurfaces();
			is_constructing = false;
			first_point_constructed = false;
			is_cutting = false;
			is_cutting_during_construction = false;
			cutting_surface_first_point_set = false;
		}
			

		if (GetKey(olc::B).bPressed)
			full_brightness = !full_brightness;

		if (GetKey(olc::N).bPressed)
			draw_normals = !draw_normals;

		if (GetKey(olc::CTRL).bHeld && GetKey(olc::Z).bPressed && surfaces.size() > 0)
			RemoveSurface(surfaces.end() - 1);

		Ray first_ray = light_ray;
		Ray second_ray = first_ray;
		Surface nearest_surface = null_surface;
		hit_corner = false;

		if (!is_constructing && !is_cutting)
		{
			for (index_ray_simulated = 0; index_ray_simulated < rays_simulated; index_ray_simulated++)
			{
				vector<Segment> segments_collided;
				vector<int> segments_indexes;
				segments_collided.reserve(segments.size());
				segments_indexes.reserve(segments.size());
				
				for (int i = 0; i < segments.size(); i++)
				{
					if (RayLineVsSegment(first_ray, segments[i]))
					{
						segments_collided.push_back(segments[i]);
						segments_indexes.push_back(i);
					}
				}


				int segments_collided_left_to_check = segments_collided.size() % NUMBERS_PER_AVX_REGISTER;

#if MT_AVX
				std::vector<olc::vf2d> intersections_per_segment;
				intersections_per_segment.resize(segments_collided.size(), null_point);

				Range intersections_indexes_range(0, intersections_per_segment.size() - segments_collided_left_to_check, NUMBERS_PER_AVX_REGISTER);

				std::for_each(std::execution::par, intersections_indexes_range.begin(), intersections_indexes_range.end(),
					[&](int i) {
						__m256 _inter_x, _inter_y;
						CollisionInfoAVXRegisters collision_infos = _RayVsSegmentsAVX(first_ray, segments_collided, i, _inter_x, _inter_y);

						for (int j = 0; j < NUMBERS_PER_AVX_REGISTER; j++)
						{
							bool intersect = bool(collision_infos._intersect.m256_f32[NUMBERS_PER_AVX_REGISTER - 1 - j]);
							bool coincide = bool(collision_infos._coincide.m256_f32[NUMBERS_PER_AVX_REGISTER - 1 - j]);
							if (intersect && !coincide && nearest_surface != segments_collided[i + j])
							{
								intersections_per_segment[i + j] = olc::vf2d(
									_inter_x.m256_f32[NUMBERS_PER_AVX_REGISTER - 1 - j],
									_inter_y.m256_f32[NUMBERS_PER_AVX_REGISTER - 1 - j]
								);
							}
						}
					});

				vector<olc::vf2d> intersections;
				vector<int> indexes;
				intersections.reserve(segments_collided.size());
				indexes.reserve(segments_collided.size());

				for (int i = 0; i < intersections_per_segment.size(); i++)
				{
					olc::vf2d& intersection = intersections_per_segment[i];
					if (intersection != null_point)
					{
						intersections.push_back(intersection);
						indexes.push_back(segments_indexes[i]);
					}
				}
#else
				vector<olc::vf2d> intersections;
				vector<int> indexes;
				intersections.reserve(segments_collided.size());
				indexes.reserve(segments_collided.size());

				for (int i = 0; i < segments_collided.size() - segments_collided_left_to_check; i += NUMBERS_PER_AVX_REGISTER)
				{
					__m256 _inter_x, _inter_y;
					CollisionInfoAVXRegisters collision_infos = _RayVsSegmentsAVX(first_ray, segments_collided, i, _inter_x, _inter_y);

					for (int j = 0; j < NUMBERS_PER_AVX_REGISTER; j++)
					{
						bool intersect = bool(collision_infos._intersect.m256_f32[NUMBERS_PER_AVX_REGISTER - 1 - j]);
						bool coincide = bool(collision_infos._coincide.m256_f32[NUMBERS_PER_AVX_REGISTER - 1 - j]);
						if (intersect && !coincide && nearest_surface != segments_collided[i + j])
						{
							intersections.push_back(olc::vf2d(
								_inter_x.m256_f32[NUMBERS_PER_AVX_REGISTER - 1 - j],
								_inter_y.m256_f32[NUMBERS_PER_AVX_REGISTER - 1 - j])
							);
							indexes.push_back(segments_indexes[i + j]);
						}
					}
				}
#endif
				for (int i = segments_collided.size() - segments_collided_left_to_check; i < segments_collided.size(); i++)
				{
					olc::vf2d intersection_point;
					CollisionInfo collision_info = RayVsSegment(first_ray, segments_collided[i], intersection_point);
					if (collision_info.intersect && !collision_info.coincide && nearest_surface != segments_collided[i])
					{
						intersections.push_back(intersection_point);
						indexes.push_back(segments_indexes[i]);
					}
				}


				if (intersections.size() == 0)
				{
					index_ray_simulated++;
					DrawRay(first_ray);
					break;
				}

				if (debug_mode && debug_show_ray_intersections && index_ray_simulated < debug_show_ray_intersections_depth)
				{
					for (olc::vf2d& intersection : intersections)
					{
						debug_rays_intersections.push_back(intersection);
					}
				}

				olc::vf2d closest_intersection = intersections[0];
				int closest_surface_index = indexes[0];
				int closest_intersection_index = 0;

				float distance_closest = (closest_intersection - first_ray.origin).mag2();
				for (int i = 0; i < intersections.size(); i++)
				{
					olc::vf2d& intersection = intersections[i];

					float distance_current = (intersection - first_ray.origin).mag2();

					if (distance_current < distance_closest)
					{
						closest_intersection = intersection;
						distance_closest = (closest_intersection - first_ray.origin).mag2();
						closest_intersection_index = i;
						closest_surface_index = indexes[i];
					}
				}

				
				Range intersections_range(0, intersections.size());
				hit_corner = std::any_of(std::execution::seq, intersections_range.begin(), intersections_range.end(), [&](int i) {
					float distance_current = (intersections[i] - first_ray.origin).mag2();
					return abs(distance_current - distance_closest) < EPSILON &&
						i != closest_intersection_index &&
						!surfaces[closest_surface_index].IsContinuationOfAnotherSurface(surfaces[indexes[i]]);
					});

				//for (int i = 0; i < intersections.size(); i++)
				//{
				//	float distance_current = (intersections[i] - first_ray.origin).mag2();
				//
				//	if (abs(distance_current - distance_closest) < EPSILON &&
				//		i != closest_intersection_index &&
				//		!surfaces[closest_surface_index].IsContinuationOfAnotherSurface(surfaces[indexes[i]]))
				//	{
				//		hit_corner = true;
				//
				//		break;
				//	}
				//}

				if (hit_corner)
				{
					corner_position = closest_intersection;

					index_ray_simulated++;

					DrawRay(first_ray, corner_position);

					break;
				}
					
				

				
				olc::vf2d intersection_point = closest_intersection;
				nearest_surface = surfaces[closest_surface_index];

				DrawRay(first_ray, intersection_point);


				ScatterInfo scatter_info = ScatterRay(first_ray, nearest_surface, intersection_point, second_ray);

				if (!full_brightness)
					second_ray.brightness -= 1.0 / rays_simulated;

				if (draw_normals && index_ray_simulated != rays_simulated - 1)
				{
					DrawNormal(intersection_point, nearest_surface, first_ray);

					if (nearest_surface.is_refractive && scatter_info.refracted)
						DrawNormal(intersection_point, nearest_surface, first_ray.OppositeRay());
				}


				first_ray = second_ray;


				if (debug_mode && debug_UI_write_ray_intersections)
				{
					if (index_ray_simulated == 0)
						debug_UI_intersections_screen_position = olc::vi2d(0, 8 * UI_scale);

					DrawStringUpLeftCorner(debug_UI_intersections_screen_position, "i = " + to_string(index_ray_simulated), UI_text_color);
					debug_UI_intersections_screen_position.y += UI_character_size;
					for (int j = 0; j < intersections.size(); j++)
					{
						string x = to_string(intersections[j].x);
						string y = to_string(intersections[j].y);
						DrawStringUpLeftCorner(debug_UI_intersections_screen_position, '#' + to_string(j) + ' ' + x + ' ' + y, UI_text_color);

						debug_UI_intersections_screen_position.y += UI_character_size;
					}
				}
			}
		}

		for (Surface& surface : surfaces)
		{
			DrawSurface(surface);
			if (debug_mode && debug_UI_write_surface_points_positions)
			{
				DrawStringUpLeftCorner(ToScreenSpace(surface.p1), "(" + to_string(surface.p1.x) + "; " + to_string(surface.p1.y) + ")", UI_text_color);
				DrawStringUpLeftCorner(ToScreenSpace(surface.p2), "(" + to_string(surface.p2.x) + "; " + to_string(surface.p2.y) + ")", UI_text_color);
			}
		}

		if (first_point_constructed && !is_cutting_during_construction)
			DrawSurface(surface_in_construction);

		if (debug_mode && debug_show_ray_intersections)
		{
			for (olc::vf2d& intersection : debug_rays_intersections)
			{
				FillCircle(ToScreenSpace(intersection), 3, olc::CYAN);

				if (debug_UI_write_ray_intersections_positions)
				{
					string x = to_string(intersection.x);
					string y = to_string(intersection.y);
					DrawStringBottomLeftCorner(ToScreenSpace(intersection), x + ' ' + y, olc::CYAN);
				}
			}

			debug_rays_intersections.clear();
		}

		if (nearest_point_found)
		{
			FillCircle(ToScreenSpace(point_to_construct), point_radius, nearest_point_snap_point_color);
			DrawCircle(ToScreenSpace(point_to_construct), nearest_point_snap_radius, nearest_point_snap_circle_color);
		}

		if (is_cutting && cutting_surface_first_point_set)
		{
			for (int i : surfaces_to_remove)
			{
				DrawLine(ToScreenSpace(surfaces[i].p1), ToScreenSpace(surfaces[i].p2), surface_to_be_removed_color);
			}
			DrawLine(ToScreenSpace(cutting_surface.p1), ToScreenSpace(cutting_surface.p2), cutting_surface_color);
		}

		FillCircle(ToScreenSpace(light_ray.origin), inner_circle_origin_radius * UI_scale, ray_origin_color);
		DrawCircle(ToScreenSpace(light_ray.origin), outer_circle_origin_radius * UI_scale, ray_origin_color);


		// Draw UI

		// Draw surfaces count
		DrawStringUpLeftCorner(olc::vi2d{ 0, 0 },
			"SURFACES COUNT: " + to_string(surfaces.size()) + " | " + 
			"MAX RAYS : " + to_string(rays_simulated) + "/" + to_string(index_ray_simulated),
			UI_text_color);

		if (is_cutting)
			DrawStringUpRightCorner(olc::vi2d{ ScreenWidth(), 2 * UI_character_size }, "(R)EMOVING SURFACES", UI_switch_state_on_color);
		else
			DrawStringUpRightCorner(olc::vi2d{ ScreenWidth(), 2 * UI_character_size }, "(R)EMOVE SURFACES", UI_switch_state_off_color);
		
		// Full brightness mode
		if (full_brightness)
			DrawStringBottomLeftCorner(olc::vi2d{ 0, ScreenHeight() }, "FULL (B)RIGHTNESS: ON", UI_switch_state_on_color);
		else
			DrawStringBottomLeftCorner(olc::vi2d{ 0, ScreenHeight() }, "FULL (B)RIGHTNESS: OFF", UI_switch_state_off_color);

		if (is_constructing)
		{
			if (surface_type == SurfaceType::REFLECTIVE)
			{
				DrawStringUpRightCorner(olc::vi2d{ ScreenWidth(), 0 }, "REFLECTIVE (S)URFACE", reflective_surface_color);
			}
			else if (surface_type == SurfaceType::REFRACTIVE)
			{
				DrawStringUpRightCorner(olc::vi2d{ ScreenWidth(), 0 }, "REFRACTIVE (S)URFACE", refractive_surface_color);
				DrawStringUpRightCorner(olc::vi2d{ ScreenWidth(), UI_character_size }, to_string(refractive_index), refractive_surface_color);
			}
		}
		else
		{
			DrawStringUpRightCorner(olc::vi2d{ ScreenWidth(), 0 }, "(S)URFACE", reflective_surface_color);
		}
		
		if (draw_normals)
			DrawStringBottomRightCorner(olc::vi2d{ ScreenWidth(), ScreenHeight() }, "DRAW (N)ORMALS", UI_switch_state_on_color);
		else
			DrawStringBottomRightCorner(olc::vi2d{ ScreenWidth(), ScreenHeight() }, "HIDE (N)ORMALS", UI_switch_state_off_color);

		if (hit_corner)
			DrawStringBottomLeftCorner(ToScreenSpace(corner_position), "HIT CORNER", UI_error_text_color);

		
		return true;
	}
};


void PrintBitRepresentation(float num)
{
	uint64_t num_as_int = *(uint64_t*)(float*)&num;
	for (int64_t i = 63; i >= 0; i--)
	{
		int bit = ((num_as_int & (1ull << (uint64_t)i)) >> (uint64_t)i);
		cout << bit;
	}
	cout << '\n';
}

float ConvertBitIntegerTofloat(uint64_t num)
{
	return *(float*)(uint64_t*)&num;
}

void GetDesktopResolution(int& horizontal, int& vertical)
{
	RECT desktop;
	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	horizontal = desktop.right;
	vertical = desktop.bottom;
}


int main()
{
	olc::GraphicsMode graphics_mode;

	vector<olc::GraphicsMode> graphics_modes;

	int screen_width, screen_height;
	GetDesktopResolution(screen_width, screen_height);

	graphics_modes.push_back({ screen_width, screen_height, 1, 1, true });
	graphics_modes.push_back({ screen_width / 2, screen_height / 2, 2, 2, true });
	graphics_modes.push_back({ 1600, 900, 1, 1 });
	graphics_modes.push_back({ 854,  480, 2, 2 });
	graphics_modes.push_back({ 532,  300, 3, 3 });
	graphics_modes.push_back({ 1280, 720, 1, 1 });
	graphics_modes.push_back({ 640,  360, 2, 2 });

	cout << "|----------------------------------|\n";
	cout << "| Mode | Resolution   | Pixel size |\n";
	cout << "|----------  FULL SCREEN  ---------|\n";
	cout << "| 1    | FULLSCREEN   | 1x1        |\n";
	cout << "| 2    | FULLSCREEN/2 | 2x2        |\n";
	cout << "|------------  FULL HD  -----------|\n";
	cout << "| 3    | 1600x900     | 1x1        |\n";
	cout << "| 4    | 854x480      | 2x2        |\n";
	cout << "| 5    | 532x300      | 3x3        |\n";
	cout << "|--------------  HD  --------------|\n";
	cout << "| 6    | 1280x720     | 1x1        |\n";
	cout << "| 7    | 640x360      | 2x2        |\n";
	cout << "| 8    | Custom       | Custom     |\n";
	cout << "|----------------------------------|\n";
	cout << '\n';

	while (true)
	{
		int mode = 4;
		cout << "Choose mode: ";
		cin >> mode;

		if (mode < 0 || mode > graphics_modes.size() + 1)
		{
			cout << "The mode you've chosen is incorrect or doesn't exist\n\n";
			continue;
		}
		else if (mode == graphics_modes.size() + 1)
		{
			cout << "Resolution width: ";
			cin >> graphics_mode.resolution_width;
			cout << "Resolution height: ";
			cin >> graphics_mode.resolution_height;
			int pixel_size;
			cout << "Pixel size: ";
			cin >> pixel_size;
			graphics_mode.pixel_width = pixel_size;
			graphics_mode.pixel_height = pixel_size;

			if (graphics_mode.resolution_width * graphics_mode.pixel_width > screen_width ||
				graphics_mode.resolution_height * graphics_mode.pixel_height > screen_height)
			{
				cout << "Could not construct a window with such resolution\n\n";
				continue;
			}
		}
		else
		{
			graphics_mode = graphics_modes[mode - 1];
		}

		break;
	}

	if (graphics_mode.resolution_width * graphics_mode.pixel_width > screen_width ||
		graphics_mode.resolution_height * graphics_mode.pixel_height > screen_height)
	{
		cout << "Could not construct a window with such resolution\n\n";
		cin.get();
		return 1;
	}

	Engine2D Engine;
	if (Engine.Construct(graphics_mode))
	{
		Engine.Start();
	}
	else
	{
		return 1;
	}

	return 0;
}