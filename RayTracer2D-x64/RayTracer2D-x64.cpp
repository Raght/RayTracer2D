#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include "Math.h"
#include "Collision.h"
#include "Physics.h"
#include "Ray.h"
#include "Line.h"
#include "Surface.h"
#include "Constants.h"

using namespace std;


struct PointAndSurface
{
	olc::vd2d point;
	Surface surface;
};

struct Shape
{
	vector<olc::vd2d> points;

	void Draw(olc::PixelGameEngine* pge, olc::Pixel color)
	{

	}
};


int rays_simulated = 8;
const int max_rays_simulated = 256;


class Engine2D : public olc::PixelGameEngine
{
public:
	Engine2D()
	{
		sAppName = "2DRayTracer";
	}

private:
	olc::vd2d GetMousePosition()
	{
		return olc::vd2d((double)GetMouseX(), (double)GetMouseY());
	}

	olc::vd2d GetWorldMousePosition()
	{
		return ToWorldSpace(GetMousePosition());
	}

	olc::vi2d ToScreenSpace(const olc::vd2d& world_position)
	{
		olc::vd2d screen_position = { world_position.x * view_scale, (double)ScreenHeight() - world_position.y * view_scale };
		return { (int)round(screen_position.x), (int)round(screen_position.y) };
	}

	olc::vd2d ToWorldSpace(const olc::vi2d& screen_position)
	{
		olc::vd2d world_position = { (double)screen_position.x, double(ScreenHeight() - screen_position.y) };
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

	void DrawNormal(const olc::vd2d& point, const Surface& surface, const Ray& ray)
	{
		DrawLine(ToScreenSpace(point),
			ToScreenSpace(point + UI_scale / view_scale * surface_normal_length * surface.Normal(ray.direction)),
			surface_normal_color);

	}

	void DrawRay(const Ray& ray)
	{
		DrawLine(ToScreenSpace(ray.origin), ToScreenSpace(ray.EndPoint()), ray.Color());
	}

	void DrawRay(const Ray& ray, const olc::vd2d& endPoint)
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


	bool debug_mode = false;
	
	
	Ray light_ray;
	vector<Surface> surfaces;
	bool hit_corner = false;
	olc::vd2d corner_position;


	SurfaceType surface_type = SurfaceType::REFLECTIVE;
	Surface surface_in_construction;
	bool first_point_constructed = false;
	bool is_constructing =false;
	double refractive_index = 1.5;
	double nearest_point_snap_radius = 8;
	olc::vd2d nearest_point;
	olc::vd2d point_to_construct;
	//unordered_map<olc::vd2d, Surface&>


	Surface cutting_surface;
	bool is_cutting = false;
	bool is_cutting_during_construction = false;
	bool cutting_surface_first_point_set = false;
	vector<int> surfaces_to_remove;


	int rays_per_second = 50;
	double ms_per_ray_increase = 1.0 / rays_per_second;
	double timer = 0.0;

	double refractive_index_step = 0.1;


	int UI_scale;
	double view_scale = 1;

	bool full_brightness = false;
	bool draw_normals = true;

	double point_radius = 2;
	olc::Pixel ray_origin_color = olc::RED;
	double inner_circle_origin_radius = 2;
	double outer_circle_origin_radius = 4;
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
	double surface_normal_length = 25.0;
	

public:
	bool OnUserCreate() override
	{
		light_ray = Ray({ double(ScreenWidth() / 2 / view_scale), double(ScreenHeight() / 2 / view_scale) },
			{ 1, 0 },
			sqrt(double(ScreenWidth() * ScreenWidth() + ScreenHeight() * ScreenHeight())) + 1,
			ray_color,
			1);

		UI_scale = max(int((double)ScreenWidth() / 640), 1);
		
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::BLACK);

		// Controlling the first ray
		light_ray.direction = olc::vd2d(GetWorldMousePosition() - light_ray.origin).norm();


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
			vector<olc::vd2d> nearest_points;
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
					[world_mouse_position = GetWorldMousePosition()](olc::vd2d& p1, olc::vd2d& p2)
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
							{
								break;
							}
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
					
					surface_in_construction.extension = 2 * EPSILON;
					surfaces.push_back(surface_in_construction);
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

				refractive_index = Cap(refractive_index, 1.0, 999.0);
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

			rays_simulated = Cap(rays_simulated, 2, max_rays_simulated);


			if (GetMouse(1).bPressed)
				light_ray.origin = GetWorldMousePosition();
		}

		if (GetKey(olc::C).bPressed)
		{
			surfaces.clear();
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
			surfaces.erase(surfaces.end() - 1);

		Ray first_ray = light_ray;
		Ray second_ray = first_ray;
		Surface nearest_surface = Surface(olc::vd2d(~0, ~0), olc::vd2d(~0, ~0));
		hit_corner = false;
		if (!is_constructing && !is_cutting)
		{
			for (int i = 0; i < rays_simulated; i++)
			{
				vector<PointAndSurface> intersections_and_surfaces;

				for (Surface& surface : surfaces)
				{
					olc::vd2d intersection_point;
					CollisionInfo collision_info = RayVsSurface(first_ray, surface, intersection_point);
					if ((collision_info.intersect || collision_info.coincide) && nearest_surface != surface)
					{
						intersections_and_surfaces.push_back({ intersection_point, surface });
					}
				}

				if (intersections_and_surfaces.size() == 0)
				{
					DrawRay(first_ray);
					break;
				}

				sort(intersections_and_surfaces.begin(),
					intersections_and_surfaces.end(),
					[ray_origin = first_ray.origin](const PointAndSurface& s1, const PointAndSurface& s2)
					{
						return (s1.point - ray_origin).mag2() < (s2.point - ray_origin).mag2();
					});

				if (debug_mode && i == 0)
				{
					for (int j = 0; j < intersections_and_surfaces.size(); j++)
					{
						DrawStringUpLeftCorner({ 0, (j + 1) * 8 * UI_scale }, to_string(intersections_and_surfaces[j].point.x) + ' ' + to_string(intersections_and_surfaces[j].point.y), UI_text_color);
					}
				}

				olc::vd2d intersection_point = intersections_and_surfaces[0].point;
				nearest_surface = intersections_and_surfaces[0].surface;

				DrawRay(first_ray, intersection_point);


				vector<PointAndSurface> close_intersections_and_surfaces;
				PointAndSurface closest = intersections_and_surfaces[0];
				for (PointAndSurface& intersection_and_surface : intersections_and_surfaces)
				{
					if ((intersection_and_surface.point - closest.point).mag2() < EPSILON)
					{
						close_intersections_and_surfaces.push_back(intersection_and_surface);
					}
				}

				if (close_intersections_and_surfaces.size() > 1)
				{
					hit_corner = true;
					corner_position = closest.point;
					break;
				}
				

				ScatterInfo scatter_info = ScatterRay(first_ray, nearest_surface, intersection_point, second_ray);

				if (!full_brightness)
					second_ray.brightness -= 1.0 / rays_simulated;

				if (draw_normals && i != rays_simulated - 1)
				{
					DrawNormal(intersection_point, nearest_surface, first_ray);

					if (nearest_surface.is_refractive && scatter_info.refracted)
						DrawNormal(intersection_point, nearest_surface, first_ray.OppositeRay());
				}


				first_ray = second_ray;
			}
		}

		for (Surface& surface : surfaces)
		{
			DrawSurface(surface);
			if (debug_mode)
			{
				DrawStringUpLeftCorner(ToScreenSpace(surface.p1), "(" + to_string(surface.p1.x) + "; " + to_string(surface.p1.y) + ")", UI_text_color);
				DrawStringUpLeftCorner(ToScreenSpace(surface.p2), "(" + to_string(surface.p2.x) + "; " + to_string(surface.p2.y) + ")", UI_text_color);
			}
		}

		if (first_point_constructed && !is_cutting_during_construction)
			DrawSurface(surface_in_construction);

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

		// Draw lines count
		DrawStringUpLeftCorner(olc::vi2d{ 0, 0 },
			"SURFACES COUNT: " + to_string(surfaces.size()) + " | MAX RAYS: " + to_string(rays_simulated),
			UI_text_color);

		if (is_cutting)
			DrawStringUpLeftCorner(olc::vi2d{ 0, 8 * UI_scale }, "(R)EMOVING SURFACES", UI_switch_state_on_color);
		else
			DrawStringUpLeftCorner(olc::vi2d{ 0, 8 * UI_scale }, "(R)EMOVE SURFACES", UI_switch_state_off_color);
		
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
				DrawStringUpRightCorner(olc::vi2d{ ScreenWidth(), 8 * UI_scale }, to_string(refractive_index), refractive_surface_color);
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


int main()
{
	olc::GraphicsMode graphics_mode;

	vector<olc::GraphicsMode> graphics_modes;
	graphics_modes.push_back({ 1600, 900, 1, 1 });
	graphics_modes.push_back({ 1280, 720, 1, 1 });
	graphics_modes.push_back({ 854,  480, 2, 2 });
	graphics_modes.push_back({ 640,  360, 2, 2 });

	cout << "Mode  Resolution  Pixel size \n";
	cout << "1     1600x900    1x1        \n";
	cout << "2     1280x720    1x1        \n";
	cout << "3     854x480     2x2        \n";
	cout << "4     640x360     2x2        \n";
	cout << "5     Custom      Custom     \n";

	while (true)
	{
		int mode = 3;
		cout << "Choose mode: ";
		cin >> mode;

		if (mode < 0 || mode > graphics_modes.size() + 1)
		{
			cout << "The mode you've chosen is incorrect or doesn't exist" << '\n';
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
		}
		else
		{
			graphics_mode = graphics_modes[mode - 1];
		}

		break;
	}

	Engine2D Engine;
	if (Engine.Construct(graphics_mode))
	{
		Engine.Start();
	}
	else return 1;

	return 0;
}