#pragma once
#include <vector>
#include <set>
#include <tuple>
#include "../mesh_generator/polygon.h"

std::tuple<double, double, int, int> generate_rect_size(const double& width, const double& height)
{
	double width_divider, height_divider;
	int width_num, height_num;
	if (width < height)
	{
		width_divider = width / rectangular_vertex_num;
		height_divider = height / static_cast<int>(width_divider);
		height_divider = height / std::ceil(height_divider);
		width_num = static_cast<int>(width / width_divider)+ 1;
		height_num = static_cast<int>(height / height_divider);
	}
	else
	{
		height_divider = height / rectangular_vertex_num;
		width_divider = width / static_cast<int>(height_divider);
		width_divider = width / std::ceil(width_divider);
		width_num = static_cast<int>(width / width_divider) + 1;
		height_num = static_cast<int>(height / height_divider);
	}
	return std::tuple<double, double, int, int>(width_divider, height_divider, width_num, height_num);
}

std::vector<vertex> generate_rect_vertex(const double& x_loc, const double& y_loc, const double& width, const double& height, std::tuple<double, double, int, int>& vertex_inform)
{
	std::vector<vertex> vt_set;
	double width_divider = std::get<0>(vertex_inform);
	double height_divider = std::get<1>(vertex_inform);
	int width_num = std::get<2>(vertex_inform);
	int height_num = std::get<3>(vertex_inform);
	for (int i = 0; i < width_num; i += 1)
	{
		vt_set.push_back(vertex(x_loc - (width / 2.0) + i * width_divider, y_loc + (height / 2.0)));
		vt_set.push_back(vertex(x_loc - (width / 2.0) + i * width_divider, y_loc - (height / 2.0)));
	}
	for (int i = 1; i < height_num; i += 1)
	{
		vt_set.push_back(vertex(x_loc - (width / 2.0), y_loc - (height / 2.0) + i * height_divider));
		vt_set.push_back(vertex(x_loc + (width / 2.0), y_loc - (height / 2.0) + i * height_divider));
	}
	return vt_set;
}

std::vector<vertex> generate_circ_vertex(const double& x_loc, const double& y_loc, const double& rad)
{
	std::vector<vertex> vt_set;
	double degree_divider = 360 / circle_vertex_num;  //360 / 30 = 12;
	for (int i = 0; i < circle_vertex_num; i += 1)
	{
		vt_set.push_back(vertex(x_loc + rad * std::cos(degree_divider * i * 2 * PI / 360), y_loc + rad * std::sin((degree_divider * i * 2 * PI / 360))));
	}
	return vt_set;
}

std::vector<vertex> generate_domain_vertex(const double& width, const double& height, std::tuple<double, double, int, int>& vertex_inform)
{
	std::vector<vertex> vt_set;
	double width_divider = std::get<0>(vertex_inform);
	double height_divider = std::get<1>(vertex_inform);
	int width_num = std::get<2>(vertex_inform);
	int height_num = std::get<3>(vertex_inform);
	for (int i = 0; i < width_num; i += 1)
	{
		vt_set.push_back(vertex(i * width_divider, height));
		vt_set.push_back(vertex(i * width_divider, 0));
	}
	for (int i = 1; i < height_num; i += 1)
	{
		vt_set.push_back(vertex(0,  i * height_divider));
		vt_set.push_back(vertex(width,  i * height_divider));
	}
	return vt_set;
}


void serial_delaunay(std::vector<vertex>& vt_set_in, std::vector<triangle>& tri_set_in, std::vector<triangle>& skinny_tri_set_in)
{
	std::cout << "Delaunay Triangulation Started..." << std::endl;
	vt_set_in.push_back(vertex(-SUPER_TRI, -SUPER_TRI));
	vt_set_in.push_back(vertex(SUPER_TRI, -SUPER_TRI));
	vt_set_in.push_back(vertex(0, SUPER_TRI));

	tri_set_in.push_back(triangle(vertex(-SUPER_TRI, -SUPER_TRI), vertex(SUPER_TRI, -SUPER_TRI), vertex(0, SUPER_TRI)));

	for (int i = 0; i < vt_set_in.size(); i += 1)
	{
		std::set<line> polygon;
		for (int j = 0; j < tri_set_in.size(); j += 1)
		{
			triangle& tri = tri_set_in[j];
			if (tri.in_circum(vt_set_in[i]))
			{
				if (polygon.count(tri.get_line1()) == 1) { polygon.erase(tri.get_line1()); }
				else { polygon.insert(tri.get_line1()); }
				if (polygon.count(tri.get_line2()) == 1) { polygon.erase(tri.get_line2()); }
				else { polygon.insert(tri.get_line2()); }
				if (polygon.count(tri.get_line3()) == 1) { polygon.erase(tri.get_line3()); }
				else { polygon.insert(tri.get_line3()); }

				std::swap(tri_set_in[j], tri_set_in[tri_set_in.size() - 1]);
				tri_set_in.pop_back();
				j -= 1;
			}
		}

		for (auto& li : polygon)
		{
			tri_set_in.push_back(triangle{ li, vt_set_in[i] });
		}
		polygon.clear();
	}

	// remove super triangle's vertexes from the vt_set_in
	vt_set_in.pop_back();
	vt_set_in.pop_back();
	vt_set_in.pop_back();

	for (int i = 0; i < tri_set_in.size(); i += 1)
	{
		if ((tri_set_in[i].vt_1.length() > SUPER_TRI-EPSILON) || (tri_set_in[i].vt_2.length() > SUPER_TRI - EPSILON) || (tri_set_in[i].vt_3.length() > SUPER_TRI - EPSILON))
		{
			std::swap(tri_set_in[i], tri_set_in[tri_set_in.size() - 1]);
			tri_set_in.pop_back();
			i--;
		}
		else
		{
			continue;
		}
	}

	for (auto & tri : tri_set_in)
	{
		if (tri.is_skinny()) { skinny_tri_set_in.push_back(tri); }
	}
	std::cout << "Delaunay Triangulation Completed" << std::endl;
}


std::vector<int> async_compare_vertices_set(std::vector<vertex>& base_vt_set, std::vector<vertex>& compared_vt_set)
{;
	std::vector<std::future<int>> check_task;
	check_task.resize(compared_vt_set.size());
	for (int i = 0; i < check_task.size(); i += 1)
	{
		check_task[i] = std::async([&, i] {
			for (int j = 0; j < base_vt_set.size(); j += 1)
			{
				if (base_vt_set[j] == compared_vt_set[i]) { return j; break; }
			}
			});
	}
	std::vector<int> result;
	result.resize(compared_vt_set.size());
	for (int i = 0; i < check_task.size(); i += 1)
	{
		result[i] = check_task[i].get();
	}
	return result;
}

std::vector<std::vector<int>> async_compare_lines_set(std::vector<vertex>& base_vt_set, std::vector<line>& compared_line_set)
{
	std::vector<std::future<std::vector<int>>> check_task;
	check_task.resize(compared_line_set.size());
	for (int i = 0; i < check_task.size(); i += 1)
	{
		check_task[i] = std::async([&, i] {
			std::vector<int> dum;
			dum.resize(2);
			for (int j = 0; j < base_vt_set.size(); j += 1)
			{
				if (base_vt_set[j] == compared_line_set[i].vt_1) { dum[0] = j; }
				if (base_vt_set[j] == compared_line_set[i].vt_2) { dum[1] = j; }
			}
			return dum;
			});
	}
	std::vector<std::vector<int>> result;
	result.resize(compared_line_set.size());
	for (int i = 0; i < check_task.size(); i += 1)
	{
		result[i] = check_task[i].get();
	}
	return result;
}