#pragma once
#include "../mesh_generator/geometry_function.h"

void chew(std::vector<vertex>& vt_set_in, std::vector<triangle>& tri_set_in, std::vector<triangle>& skinny_tri_set_in)
{
	std::cout << "Delaunay Refinement(Method of Chew) Started..." << std::endl;

	while (skinny_tri_set_in.size() > 0)
	{
		vertex vt_in = skinny_tri_set_in[0].get_circum_center();

		vt_set_in.push_back(vt_in);

		std::set<line> polygon;
		for (int i = 0; i < tri_set_in.size(); i += 1)
		{
			triangle& tri = tri_set_in[i];
			if (tri.in_circum(vt_in))
			{
				if (polygon.count(tri.get_line1()) == 1) { polygon.erase(tri.get_line1()); }
				else { polygon.insert(tri.get_line1()); }
				if (polygon.count(tri.get_line2()) == 1) { polygon.erase(tri.get_line2()); }
				else { polygon.insert(tri.get_line2()); }
				if (polygon.count(tri.get_line3()) == 1) { polygon.erase(tri.get_line3()); }
				else { polygon.insert(tri.get_line3()); }

				std::swap(tri_set_in[i], tri_set_in[tri_set_in.size() - 1]);
				tri_set_in.pop_back();
				i -= 1;
			}
		}

		std::set<vertex> polygon_vt;
		for (auto& li : polygon)
		{
			polygon_vt.insert(li.vt_1);
			polygon_vt.insert(li.vt_2);
			triangle new_tri = triangle{ li, vt_in };
			tri_set_in.push_back(new_tri);
		}

		for (int i = 0; i < skinny_tri_set_in.size(); i += 1)
		{
			if (polygon_vt.count(skinny_tri_set_in[i].vt_1) == 1 && polygon_vt.count(skinny_tri_set_in[i].vt_2) == 1 && polygon_vt.count(skinny_tri_set_in[i].vt_3) == 1)
			{
				std::swap(skinny_tri_set_in[i], skinny_tri_set_in[skinny_tri_set_in.size() - 1]);
				skinny_tri_set_in.pop_back();
				i -= 1;
			}
		}
		for (auto& li : polygon)
		{
			triangle new_tri = triangle{ li, vt_in };
			if (new_tri.is_skinny()) { skinny_tri_set_in.push_back(new_tri); }
		}


		polygon.clear();
	}
	std::cout << "Delaunay Refinement(Method of Chew) Completed" << std::endl;
}