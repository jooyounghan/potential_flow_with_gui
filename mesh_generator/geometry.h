#pragma once
#include "../mesh_generator/chew_algorithm.h"
#include <bitset>
#include <future>
#include "../timer.h"

class obstacle_shape
{
public:
	virtual bool is_inner_vertex(const vertex& vt_in) const = 0;
	virtual bool is_inner_triangle(const vertex& vt_in) const = 0;
	virtual std::vector<vertex>& get_vector() = 0;
	virtual void print() const = 0;
};

class rectangle : public obstacle_shape
{
private:
	std::vector<vertex> vt_set;
	double x_loc, y_loc;
	int width, height;

public:
	rectangle(const double& x_in, const double& y_in, const int& width_in, const int& height_in) : x_loc(x_in), y_loc(y_in), width(width_in), height(height_in), obstacle_shape()
	{
		std::tuple<double, double, int, int> vertex_inform;
		vertex_inform = generate_rect_size(width, height);
		vt_set = generate_rect_vertex(x_loc, y_loc, width, height, vertex_inform);
	}

	virtual bool is_inner_vertex(const vertex& vt_in) const override
	{
		double x = vt_in.x_loc;
		double y = vt_in.y_loc;
		if ((x < x_loc + (width / 2.0) - EPSILON) && (x > x_loc - (width / 2.0) + EPSILON) && (y < y_loc + (height / 2.0) - EPSILON) && (y > y_loc - (height / 2.0) + EPSILON))
		{
			return true;
		}
		else { return false; }
	}

	virtual bool is_inner_triangle(const vertex& vt_in) const override
	{
		double x = vt_in.x_loc;
		double y = vt_in.y_loc;
		if ((x < x_loc + (width / 2.0) + EPSILON) && (x > x_loc - (width / 2.0) - EPSILON) && (y < y_loc + (height / 2.0) + EPSILON) && (y > y_loc - (height / 2.0) - EPSILON))
		{
			return true;
		}
		else { return false; }
	}

	virtual void print() const override
	{
		for (int i = 0; i < vt_set.size(); i += 1)
		{
			std::cout << (vt_set)[i] << std::endl;
		}
	}

	virtual std::vector<vertex>& get_vector() override { return vt_set; }
};

class circle : public obstacle_shape
{
private:
	std::vector<vertex> vt_set;
	int x_loc, y_loc, radius;

public:
	circle(const int& x_in, const int& y_in, const int& r_in) : x_loc(x_in), y_loc(y_in), radius(r_in), obstacle_shape()
	{
		vt_set = generate_circ_vertex(x_loc, y_loc, radius);
	}

	virtual bool is_inner_vertex(const vertex& vt_in) const override
	{
		double x = vt_in.x_loc;
		double y = vt_in.y_loc;
		if (std::pow(x - x_loc, 2) + std::pow(y - y_loc, 2) < std::pow(radius, 2) - EPSILON)
		{
			return true;
		}
		else { return false; }
	}

	virtual bool is_inner_triangle(const vertex& vt_in) const override
	{
		double x = vt_in.x_loc;
		double y = vt_in.y_loc;
		if (std::pow(x - x_loc, 2) + std::pow(y - y_loc, 2) < std::pow(radius, 2) + EPSILON)
		{
			return true;
		}
		else { return false; }
	}

	virtual void print() const override
	{
		for (int i = 0; i < vt_set.size(); i += 1)
		{
			std::cout << (vt_set)[i] << std::endl;
		}
	}

	virtual std::vector<vertex>& get_vector() override { return vt_set; }

};



class domain
{
public:
	int width, height;
	std::vector<vertex> vt_set;
	std::vector<triangle> tri_set;
	std::vector<triangle> skinny_set;
	std::vector<obstacle_shape*> obstacle_set;

	domain() : width(0), height(0){}	//default structure

	domain(const int& width_in, const int& height_in) : width(width_in), height(height_in)
	{
		std::tuple<double, double, int, int> vertex_inform;
		vertex_inform = generate_rect_size(width, height);
		vt_set = generate_domain_vertex(width, height, vertex_inform);
	}

	void setting(const int& width_in, const int& height_in)
	{
		width = width_in, height = height_in;
		std::tuple<double, double, int, int> vertex_inform;
		vertex_inform = generate_rect_size(width, height);
		vt_set = generate_domain_vertex(width, height, vertex_inform);
		tri_set.clear();
		obstacle_set.clear();
	}

	void insert(obstacle_shape* obs)
	{
		std::vector<vertex>& obs_vector = obs->get_vector();
		std::vector<std::future<void>> compare_task;
		compare_task.resize(obs_vector.size());
		for (int i = 0; i < obs_vector.size(); i += 1)
		{
			compare_task[i] = std::async([&, i] {
				for (int j = 0; j < vt_set.size(); j += 1)
				{
					if (vt_set[j] == obs_vector[i])
					{
						mtx.lock();
						std::swap(vt_set[j], vt_set[vt_set.size() - 1]);
						vt_set.pop_back();
						mtx.unlock();
						j -= 1;
					}
				}
				});
		}
		for (int i = 0; i < obs_vector.size(); i += 1)
		{
			compare_task[i].wait();
		}
		vt_set.insert(vt_set.end(), obs_vector.begin(), obs_vector.end());
		// insert obstacle itself to the domain to access to the function of the obstacle(is_inner)
		obstacle_set.push_back(obs);
	}
	
	void print()
	{
		for (int i = 0; i < vt_set.size(); i += 1)
		{
			std::cout << vt_set[i] << std::endl;
		}
	}

	bool is_inner_vertex(const vertex& vt_in)
	{
		double x = vt_in.x_loc;
		double y = vt_in.y_loc;
		if ((x < (width + EPSILON)) && (x > - EPSILON) && (y < (height) + EPSILON) && (y > - EPSILON))
		{
			return true;
		}
		else { return false; }
	}


	void remove_before()
	{
		std::cout << "Removing Dot which is not in the Domain..." << std::endl;
		for (int i = 0; i < vt_set.size(); i += 1)
		{
			if(is_inner_vertex(vt_set[i])){ continue; }
			else
			{
				std::swap(vt_set[i], vt_set[vt_set.size() - 1]);
				vt_set.pop_back();
				i -= 1;
			}
		}
		std::cout << "Removing Completed" << std::endl;
	}

	void remove_after() // TODO : NEEDS TO BE MODIFIED
	{
		std::cout << "Removing Dot And Triangle Which is in the Obstacle..." << std::endl;
		std::vector<std::future<void>> remove_task;
		remove_task.resize(2);
		remove_task[0] = std::async([&] {

			for (int i = 0; i < tri_set.size(); i += 1)
			{
				for (int j = 0; j < obstacle_set.size(); j += 1)
				{
					if (obstacle_set[j]->is_inner_triangle(tri_set[i].vt_1) && obstacle_set[j]->is_inner_triangle(tri_set[i].vt_2) && obstacle_set[j]->is_inner_triangle(tri_set[i].vt_3))
					{
						mtx.lock();
						std::swap(tri_set[i], tri_set[tri_set.size() - 1]);
						tri_set.pop_back();
						mtx.unlock();
						i -= 1;
						break;
					}
					else { continue; }
				}
			}
			});

		remove_task[1] = std::async([&] {
			for (int i = 0; i < vt_set.size(); i += 1)
			{
				for (int j = 0; j < obstacle_set.size(); j += 1)
				{
					if (obstacle_set[j]->is_inner_vertex(vt_set[i]))
					{
						mtx.lock();
						std::swap(vt_set[i], vt_set[vt_set.size() - 1]);
						vt_set.pop_back();
						mtx.unlock();
						i -= 1;
						break;
					}
					else { continue; }
				}
			}
			});
		for (int i = 0; i < remove_task.size(); i += 1)
		{
			remove_task[i].wait();
		}
		
		std::vector<std::future<void>> refinement_task;
		refinement_task.resize(obstacle_set.size());
		for (int i = 0; i < refinement_task.size(); i += 1)
		{
			refinement_task[i] = std::async([&, i] {
				//when using std::async[&], i becomes refinement_task.size() (because it is called at last, so vector subscript problem occurs.
				//To deal with this problem, we have to copy data to each thread's stack.
				std::vector<triangle> distorted_region;
				std::vector<triangle> distorted_triangle;
				// Check the Triagle which has the vertex in the obstacle and put in in the distorted_triangle
				mtx.lock();
				for (int j = 0; j < tri_set.size(); j += 1)
				{
					if (obstacle_set[i]->is_inner_vertex(tri_set[j].vt_1) || obstacle_set[i]->is_inner_vertex(tri_set[j].vt_2) || obstacle_set[i]->is_inner_vertex(tri_set[j].vt_3))
					{
						distorted_triangle.push_back(tri_set[j]);

						std::swap(tri_set[j], tri_set[tri_set.size() - 1]);
						tri_set.pop_back();
						j -= 1;
					}
				}
				mtx.unlock();

				while (distorted_triangle.size() > 0)
				{
					// distorted triangle is defined in the stack area, so data sharing not occur(we don't need to use mtx.lock() here.
					distorted_region.push_back(distorted_triangle[0]);
					std::swap(distorted_triangle[0], distorted_triangle[distorted_triangle.size() - 1]);
					distorted_triangle.pop_back();
					
					std::set<vertex> inner_vertex;
					if (obstacle_set[i]->is_inner_vertex(distorted_region[0].vt_1)) { inner_vertex.insert(distorted_region[0].vt_1);}
					if (obstacle_set[i]->is_inner_vertex(distorted_region[0].vt_2)) { inner_vertex.insert(distorted_region[0].vt_2); }
					if (obstacle_set[i]->is_inner_vertex(distorted_region[0].vt_3)) { inner_vertex.insert(distorted_region[0].vt_3); }


					for (int j = 0; j < distorted_triangle.size(); j += 1)
					{
						if (distorted_triangle[j].find_vertex(inner_vertex))
						{
							bool new_inner_vertex = false;
							if (obstacle_set[i]->is_inner_vertex(distorted_triangle[j].vt_1)) { new_inner_vertex = true; inner_vertex.insert(distorted_triangle[j].vt_1); }
							if (obstacle_set[i]->is_inner_vertex(distorted_triangle[j].vt_2)) { new_inner_vertex = true; inner_vertex.insert(distorted_triangle[j].vt_2); }
							if (obstacle_set[i]->is_inner_vertex(distorted_triangle[j].vt_3)) { new_inner_vertex = true; inner_vertex.insert(distorted_triangle[j].vt_3); }
							distorted_region.push_back(distorted_triangle[j]);
							std::swap(distorted_triangle[j], distorted_triangle[distorted_triangle.size() - 1]);
							distorted_triangle.pop_back();
							if (new_inner_vertex) { j = -1; }
							else { j -= 1; }
						}
					}

					std::set<vertex> local_delaunay_vertex;
					for (auto& tri : distorted_region)
					{
						local_delaunay_vertex.insert(tri.vt_1);
						local_delaunay_vertex.insert(tri.vt_2);
						local_delaunay_vertex.insert(tri.vt_3);
					}

					std::set<vertex>::iterator iter = local_delaunay_vertex.begin();
					while (iter != local_delaunay_vertex.end())
					{
						if (obstacle_set[i]->is_inner_vertex(*iter))
						{
							local_delaunay_vertex.erase(iter++);
						}
						else { ++iter; }
					}

					distorted_region.clear();

					std::vector<triangle> local_delaunay_triangle;
					local_delaunay_triangle.push_back(triangle(vertex(-SUPER_TRI, -SUPER_TRI), vertex(SUPER_TRI, -SUPER_TRI), vertex(0, SUPER_TRI)));

					for (auto & vt : local_delaunay_vertex)
					{
						std::set<line> local_delaunay_polygon;
						for (int j = 0; j < local_delaunay_triangle.size(); j += 1)
						{
							triangle& tri = local_delaunay_triangle[j];
							if (tri.in_circum(vt))
							{
								if (local_delaunay_polygon.count(tri.get_line1()) == 1) { local_delaunay_polygon.erase(tri.get_line1()); }
								else { local_delaunay_polygon.insert(tri.get_line1()); }
								if (local_delaunay_polygon.count(tri.get_line2()) == 1) { local_delaunay_polygon.erase(tri.get_line2()); }
								else { local_delaunay_polygon.insert(tri.get_line2()); }
								if (local_delaunay_polygon.count(tri.get_line3()) == 1) { local_delaunay_polygon.erase(tri.get_line3()); }
								else { local_delaunay_polygon.insert(tri.get_line3()); }

								std::swap(local_delaunay_triangle[j], local_delaunay_triangle[local_delaunay_triangle.size() - 1]);
								local_delaunay_triangle.pop_back();
								j -= 1;
							}
						}

						for (auto& li : local_delaunay_polygon)
						{
							local_delaunay_triangle.push_back(triangle{ li, vt });
						}
						local_delaunay_polygon.clear();
					}

					for (int j = 0; j < local_delaunay_triangle.size(); j += 1)
					{
						if ((local_delaunay_triangle[j].vt_1.length() > SUPER_TRI - EPSILON) || (local_delaunay_triangle[j].vt_2.length() > SUPER_TRI - EPSILON) || (local_delaunay_triangle[j].vt_3.length() > SUPER_TRI - EPSILON))
						{
							std::swap(local_delaunay_triangle[j], local_delaunay_triangle[local_delaunay_triangle.size() - 1]);
							local_delaunay_triangle.pop_back();
							j--;
						}
					}
					for (int j = 0; j < local_delaunay_triangle.size(); j += 1)
					{
						mtx.lock();
						tri_set.push_back(local_delaunay_triangle[j]);
						mtx.unlock();
						std::swap(local_delaunay_triangle[j], local_delaunay_triangle[local_delaunay_triangle.size() - 1]);
						local_delaunay_triangle.pop_back();
						j--;
					}
					local_delaunay_triangle.clear();
				}
			});
		}
		std::cout << "Removing Completed" << std::endl;
	}

	void generate()
	{
		remove_before();
		serial_delaunay(vt_set, tri_set, skinny_set);
		chew(vt_set, tri_set, skinny_set);
		remove_after();
	}
};