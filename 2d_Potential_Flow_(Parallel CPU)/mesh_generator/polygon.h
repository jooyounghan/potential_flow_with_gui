#pragma once
#include <iostream>
#include "../constants.h"

class vertex
{
public:
	double x_loc, y_loc;

public:
	vertex() { x_loc = 0; y_loc = 0; }
	vertex(const double& x_in, const double& y_in) : x_loc(x_in), y_loc(y_in) { }

	friend std::ostream& operator << (std::ostream& out, const vertex& vt) 
	{
		out << vt.x_loc << " " << vt.y_loc;
		return out;
	}

	double det(const vertex& vt) const
	{
		double result = this->x_loc * vt.y_loc - this->y_loc * vt.x_loc;
		return result;
	}

	double length() const { return std::sqrt(std::pow(this->x_loc, 2) + std::pow(this->y_loc, 2)); }
	double length_to(const vertex& vt_in) const { return std::sqrt(std::pow(this->x_loc - vt_in.x_loc, 2) + std::pow(this->y_loc - vt_in.y_loc, 2)); }

	double get_x_dist(const vertex& vt) const { return (this->x_loc - vt.x_loc); }
	double get_y_dist(const vertex& vt) const { return (this->y_loc - vt.y_loc); }

	vertex operator - (const vertex& vt_in) const { return vertex(this->x_loc - vt_in.x_loc, this->y_loc - vt_in.y_loc); }

	bool operator > (const vertex& vt_in) const
	{
		if (this->x_loc > vt_in.x_loc) { return true; }
		else if (this->x_loc == vt_in.x_loc)
		{
			if (this->y_loc > vt_in.y_loc) { return true; }
			else { return false; }
		}
		else { return false; }
	}

	bool operator < (const vertex& vt_in) const
	{
		if (this->x_loc < vt_in.x_loc) { return true; }
		else if (this->x_loc == vt_in.x_loc)
		{
			if (this->y_loc < vt_in.y_loc) { return true; }
			else { return false; }
		}
		else { return false; }
	}

	bool operator == (const vertex& vt_in) const
	{
		if (this->x_loc == vt_in.x_loc && this->y_loc == vt_in.y_loc) { return true; }
		else { return false; }
	}
};

class line
{
public:
	vertex vt_1, vt_2;

public:
	line(const vertex& v1_in, const vertex& v2_in)
	{
		if (v1_in > v2_in) { vt_1 = v1_in; vt_2 = v2_in; }
		else { vt_1 = v2_in; vt_2 = v1_in; }
	}
	
	bool operator > (const line& line_in) const
	{
		if (this->vt_1 > line_in.vt_1) { return true; }
		else if (this->vt_1 == line_in.vt_1)
		{
			if (this->vt_2 > line_in.vt_2) { return true; }
			else { return false; }
		}
		else { return false; }
	}

	bool operator < (const line& line_in) const
	{
		if (this->vt_1 < line_in.vt_1) { return true; }
		else if (this->vt_1 == line_in.vt_1)
		{
			if (this->vt_2 < line_in.vt_2) { return true; }
			else { return false; }
		}
		else { return false; }
	}

	bool operator == (const line& line_in) const
	{
		if (this->vt_1 == line_in.vt_1 && this->vt_2 == line_in.vt_2) { return true; }
		else { return false; }
	}

	const double length() const { return vt_1.length_to(vt_2); }
};

class triangle
{
public:

	vertex vt_1, vt_2, vt_3;

public:
	triangle(const vertex& v1_in, const vertex& v2_in, const vertex& v3_in)
		: vt_1(v1_in), vt_2(v2_in), vt_3(v3_in)
	{}

	triangle(const line& li, const vertex& v_in)
		: vt_1(li.vt_1), vt_2(li.vt_2), vt_3(v_in)
	{}

	bool find_vertex(std::set<vertex>& vt_set_in) const
	{
		for (auto &vt : vt_set_in)
		{
			if (vt_1 == vt || vt_2 == vt || vt_3 == vt) { return true; }
			else { continue; }
		}
		return false;
	}

	bool in_circum(const vertex& vt_in) const
	{
		vertex vector1 = vt_2 - vt_1;
		vertex vector2 = vt_3 - vt_1;
		double deter = vector1.det(vector2);
		if (deter > 0) { deter -= EPSILON; }
		else { deter += EPSILON; }
		double
			adx = vt_1.get_x_dist(vt_in), ady = vt_1.get_y_dist(vt_in),
			bdx = vt_2.get_x_dist(vt_in), bdy = vt_2.get_y_dist(vt_in),
			cdx = vt_3.get_x_dist(vt_in), cdy = vt_3.get_y_dist(vt_in),
			bdxcdy = bdx * cdy, cdxbdy = cdx * bdy,
			cdxady = cdx * ady, adxcdy = adx * cdy,
			adxbdy = adx * bdy, bdxady = bdx * ady,
			apower = adx * adx + ady * ady,
			bpower = bdx * bdx + bdy * bdy,
			cpower = cdx * cdx + cdy * cdy;

		double value = apower * (bdxcdy - cdxbdy) + bpower * (cdxady - adxcdy) + cpower * (adxbdy - bdxady);
		if (deter > 0) { return value > -EPSILON; }
		else { return value < EPSILON; }
	}

	const vertex get_circum_center() const
	{
		//https://blog.naver.com/010427kim/221015973397
		double m_x1 = vt_1.x_loc;
		double m_y1 = vt_1.y_loc;
		double m_x2 = vt_2.x_loc;
		double m_y2 = vt_2.y_loc;
		double m_x3 = vt_3.x_loc;
		double m_y3 = vt_3.y_loc;
		double center_x;
		center_x = (std::pow(m_x1, 2) * (m_y2 - m_y3) + std::pow(m_x2, 2) * (m_y3 - m_y1) + std::pow(m_x3, 2) * (m_y1 - m_y2) - (m_y1 - m_y2) * (m_y2 - m_y3) * (m_y3 - m_y1))
			/ (2.0 * (m_x1 * (m_y2 - m_y3) + m_x2 * (m_y3 - m_y1) + m_x3 * (m_y1 - m_y2)));
		double center_y;
		center_y = (std::pow(m_y1, 2) * (m_x2 - m_x3) + std::pow(m_y2, 2) * (m_x3 - m_x1) + std::pow(m_y3, 2) * (m_x1 - m_x2) - (m_x1 - m_x2) * (m_x2 - m_x3) * (m_x3 - m_x1))
			/ (2.0 * (m_y1 * (m_x2 - m_x3) + m_y2 * (m_x3 - m_x1) + m_y3 * (m_x1 - m_x2)));
		return vertex(center_x, center_y);
	}


	bool is_skinny(const double& threshold = 1)
	{
		//https://www.cs.cmu.edu/~quake-papers/delaunay-refinement.pdf
		//With this bound(threshold), every new edge created is at least as long as some other edge already in the mesh.

		//Chew¡¯s first algorithm splits any triangle whose circumradius is greater than minimum length of the triangle, and hence creates a
		//uniform mesh.Chew¡¯s second Delaunay refinement algorithm relaxes this stricture, splitting only triangles
		//whose circumradius - to - shortest edge ratios are greater than one, and hence produces graded meshes in
		//practice, although Chew supplies no theoretical guarantee of good grading.
		double radius = get_circum_center().length_to(vt_1);
		double length1 = vt_1.length_to(vt_2);
		double length2 = vt_2.length_to(vt_3);
		double length3 = vt_3.length_to(vt_1);
		double mininum;
		if (length1 < length2 && length1 < length3) { mininum = length1; }
		else if (length2 < length1 && length2 < length3) { mininum = length2; }
		else { mininum = length3; }
		double ctseratio = radius / mininum;
		if (ctseratio > threshold) { return true; }
		else { return false; }
	}

	bool is_ccw()
	{
		vertex vector1 = vertex(vt_2.x_loc - vt_1.x_loc, vt_2.y_loc - vt_1.y_loc);
		vertex vector2 = vertex(vt_3.x_loc - vt_1.x_loc, vt_3.y_loc - vt_1.y_loc);
		return (vector1.det(vector2) > -EPSILON);
	}

	const double get_area() const
	{
		double x1y2 = vt_1.x_loc * vt_2.y_loc;
		double x2y3 = vt_2.x_loc * vt_3.y_loc;
		double x3y1 = vt_3.x_loc * vt_1.y_loc;
		double x2y1 = vt_2.x_loc * vt_1.y_loc;
		double x3y2 = vt_3.x_loc * vt_2.y_loc;
		double x1y3 = vt_1.x_loc * vt_3.y_loc;
		double area = 0.5 * abs(x1y2 + x2y3 + x3y1 - x2y1 - x3y2 - x1y3);
		return area;
	}


	line get_line1() const { return line(vt_1, vt_2); }
	line get_line2() const { return line(vt_2, vt_3); }
	line get_line3() const { return line(vt_3, vt_1); }
};