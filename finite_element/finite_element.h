#pragma once
#include "../2dmatrix/matrix.h"
#include "../mesh_generator/geometry.h"
#include <string>
#include <fstream>

std::vector<std::string> split(const std::string& s_in)
{
	std::vector<std::string> result;
	std::string temp;
	for (auto& ch : s_in)
	{
		if (ch == ' ') { result.push_back(temp); temp.clear(); }
		else { temp.append(sizeof(char), ch); }
	}
	result.push_back(temp); temp.clear();
	return result;
}

class finite_element
{
private:
	int width, height;
	std::vector<vertex>* vt_set_ptr;
	std::vector<triangle>* tri_set_ptr;
	std::vector<obstacle_shape*>* obstacle_set_ptr;
	matrix fem_surface;
	matrix fem_boundary;
	matrix fem_result;

public:
	finite_element(const int& width_in, const int& height_in, std::vector<vertex>* & vt_set_ptr_in, std::vector<triangle>* & tri_set_ptr_in, std::vector<obstacle_shape*>*& obstacle_set_ptr_in)
		: vt_set_ptr(vt_set_ptr_in), tri_set_ptr(tri_set_ptr_in), obstacle_set_ptr(obstacle_set_ptr_in), fem_surface(matrix()), fem_boundary(matrix()), fem_result(matrix()), width(width_in), height(height_in)
	{
		fem_surface.set(static_cast<int>(vt_set_ptr->size()), static_cast<int>(vt_set_ptr->size()));
		fem_boundary.set(static_cast<int>(vt_set_ptr->size()), 1);

		for (auto& tri : *tri_set_ptr)
		{
			matrix g_part = matrix{ {1.0, tri.vt_1.x_loc, tri.vt_1.y_loc}, {1.0, 0.0, 0.0}, {1.0, 0.0, 0.0} };
			std::vector<vertex> queue;
			if (tri.is_ccw())
			{
				queue.push_back(tri.vt_1);
				queue.push_back(tri.vt_2);
				queue.push_back(tri.vt_3);
				//get1 -> get2 -> get3
				g_part[1][1] = tri.vt_2.x_loc; g_part[1][2] = tri.vt_2.y_loc;
				g_part[2][1] = tri.vt_3.x_loc; g_part[2][2] = tri.vt_3.y_loc;
			}
			else
			{
				queue.push_back(tri.vt_1);
				queue.push_back(tri.vt_3);
				queue.push_back(tri.vt_2);
				//get1 -> get3 -> get2
				g_part[1][1] = tri.vt_3.x_loc; g_part[1][2] = tri.vt_3.y_loc;
				g_part[2][1] = tri.vt_2.x_loc; g_part[2][2] = tri.vt_2.y_loc;
			}
			g_part = g_part.inv();
			//fem_boundary assembly part
			matrix b{ {g_part[1][0]}, {g_part[1][1]}, {g_part[1][2]} };
			matrix c{ {g_part[2][0]}, {g_part[2][1]}, {g_part[2][2]} };
			matrix bt = b.transpose();
			matrix ct = c.transpose();
			matrix bbt = (b * bt);
			matrix cct = (c * ct);
			g_part = (bbt + cct) * tri.get_area();
			
			std::vector<int> queue_idx = async_compare_vertices_set(*vt_set_ptr, queue);

			for (int i = 0; i < queue_idx.size(); i += 1)
			{
				for (int j = 0; j < queue_idx.size(); j += 1)
				{
					fem_surface[queue_idx[i]][queue_idx[j]] += g_part[i][j];
				}
			}
		}
		std::vector<line> inlet_line_set;
		for (auto& tri : *tri_set_ptr)
		{
			if ((tri.vt_1.x_loc < EPSILON) && (tri.vt_1.x_loc > -EPSILON) && (tri.vt_2.x_loc < EPSILON) && (tri.vt_2.x_loc > -EPSILON))
			{
				inlet_line_set.push_back(line(tri.vt_1, tri.vt_2));
			}
			if ((tri.vt_2.x_loc < EPSILON) && (tri.vt_2.x_loc > -EPSILON) && (tri.vt_3.x_loc < EPSILON) && (tri.vt_3.x_loc > -EPSILON))
			{
				inlet_line_set.push_back(line(tri.vt_2, tri.vt_3));
			}
			if ((tri.vt_3.x_loc < EPSILON) && (tri.vt_3.x_loc > -EPSILON) && (tri.vt_1.x_loc < EPSILON) && (tri.vt_1.x_loc > -EPSILON))
			{
				inlet_line_set.push_back(line(tri.vt_3, tri.vt_1));
			}
		}

		double inlet_velocity;
		std::ifstream inlet;
		char inlet_velocity_char[32];
		inlet.open("./data/initial_value_set/inlet_v.txt");
		while (!inlet.eof())
		{
			inlet.getline(inlet_velocity_char, 32);
			std::vector<std::string> inlet_velocity_string = split(std::string{ inlet_velocity_char });
			if (inlet_velocity_string.size() == 1)
			{
				inlet_velocity = std::stod(inlet_velocity_string[0]);
			}
		}
		inlet.close();

		std::vector<std::vector<int>> inlet_idx = async_compare_lines_set(*vt_set_ptr, inlet_line_set);
		for (int i = 0; i < inlet_idx.size(); i += 1)
		{
			matrix l_part = matrix{ {2.0, 1.0}, {1.0, 2.0} } * (inlet_line_set[i].length() / 6.0);
			matrix boundary_condition = matrix{ {-inlet_velocity}, {-inlet_velocity} };
			l_part = l_part * boundary_condition;
			fem_boundary[inlet_idx[i][0]][0] = l_part[0][0];
			fem_boundary[inlet_idx[i][1]][0] = l_part[1][0];
		}
		inlet_idx.clear();


		std::vector<line> outlet_line_set;
		for (auto& tri : *tri_set_ptr)
		{
			if ((tri.vt_1.x_loc < this->width + EPSILON) && (tri.vt_1.x_loc > this->width - EPSILON) && (tri.vt_2.x_loc < this->width + EPSILON) && (tri.vt_2.x_loc > this->width - EPSILON))
			{
				outlet_line_set.push_back(line(tri.vt_1, tri.vt_2));
			}
			if ((tri.vt_2.x_loc < this->width + EPSILON) && (tri.vt_2.x_loc > this->width - EPSILON) && (tri.vt_3.x_loc < this->width + EPSILON) && (tri.vt_3.x_loc > this->width - EPSILON))
			{
				outlet_line_set.push_back(line(tri.vt_2, tri.vt_3));
			}
			if ((tri.vt_3.x_loc < this->width + EPSILON) && (tri.vt_3.x_loc > this->width - EPSILON) && (tri.vt_1.x_loc < this->width + EPSILON) && (tri.vt_1.x_loc > this->width - EPSILON))
			{
				outlet_line_set.push_back(line(tri.vt_3, tri.vt_1));
			}
		}

		std::vector<std::vector<int>> outlet_idx = async_compare_lines_set(*vt_set_ptr, outlet_line_set);
		for (int i = 0; i < outlet_idx.size(); i += 1)
		{
			//set outlet by dirichlet  boundary condition(uniform potential fucntion)
			for (int j = 0; j < (vt_set_ptr->size()); j += 1)
			{
				fem_surface[outlet_idx[i][0]][j] = 0;
				fem_surface[outlet_idx[i][1]][j] = 0;
				if (j == outlet_idx[i][0]) { fem_surface[j][outlet_idx[i][0]] = 1.0; }
				if (j == outlet_idx[i][1]) { fem_surface[j][outlet_idx[i][1]] = 1.0; }
			}
			fem_boundary[outlet_idx[i][0]][0] = 0;
			fem_boundary[outlet_idx[i][1]][0] = 0;
		}
	}

	void solve()
	{
		std::cout << "Solving problem with fem..." << std::endl;
		fem_result = fem_surface.solve(fem_boundary);
		std::cout << "problem solved!" << std::endl;
	}


	void export_data()
	{
		std::cout << "Exporting surface..." << std::endl;
		std::ofstream output1("./data/result/surface.txt");
		for (int i = 0; i < vt_set_ptr->size(); i += 1)
		{
			for (int j = 0; j < vt_set_ptr->size(); j += 1)
			{
				output1 << fem_surface[i][j] << " ";
			}
			output1 << std::endl;
		}
		output1.close();

		std::cout << "Exporting boundary..." << std::endl;
		std::ofstream output2("./data/result/boundary.txt");
		for (int i = 0; i < vt_set_ptr->size(); i += 1)
		{
			output2 << fem_boundary[i][0] << std::endl;
		}
		output2.close();

		std::cout << "Exporting result..." << std::endl;
		std::ofstream output3("./data/result/result.txt");
		for (int i = 0; i < vt_set_ptr->size(); i += 1)
		{
			output3 << (*vt_set_ptr)[i].x_loc << " " << (*vt_set_ptr)[i].y_loc << " " << fem_result[i][0] << std::endl;
		}
		output3.close();
		std::cout << "export ended" << std::endl;
	}
};