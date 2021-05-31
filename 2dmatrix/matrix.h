#pragma once
#include <iostream>
#include <cassert>
#include "../2dmatrix/function_parallel.h"

class matrix
{
private :
	int row_num;
	int col_num;
	double** m_mat = nullptr;

public:
	matrix() : row_num(0), col_num(0){}

	matrix(const int& row_in, const int& col_in, const double& init = 0) : row_num(row_in), col_num(col_in)
	{
		m_mat = new double* [row_in];
		for (int i = 0; i < row_in; i += 1) { m_mat[i] = new double[col_in]; }
		async_data_assign(m_mat, init);
	}

	matrix(double**& mat_in) : row_num(static_cast<int>(_msize(mat_in)) / sizeof(double*)), col_num(static_cast<int>(_msize(mat_in[0])) / sizeof(double))
	{
		copy_allocation(m_mat, mat_in);
		async_data_assign(m_mat, mat_in);
	}

	matrix(double** && mat_in) : row_num(static_cast<int>(_msize(mat_in)) / sizeof(double*)), col_num(static_cast<int>(_msize(mat_in[0])) / sizeof(double))
	{
		move_semantic(m_mat, mat_in);
	}

	matrix(matrix& mat_in) : row_num(mat_in.row_num), col_num(mat_in.col_num)
	{
		copy_allocation(m_mat, mat_in.m_mat);
		async_data_assign(m_mat, mat_in.m_mat);
	}

	matrix(matrix&& mat_in) noexcept : row_num(mat_in.row_num), col_num(mat_in.col_num)
	{
		move_semantic(m_mat, mat_in.m_mat);
	}

	matrix(const std::initializer_list<std::initializer_list<double>>& list) : row_num(static_cast<int>(list.size())), col_num(0)
	{
		int temp_iter = 0;
		for (auto& input_row : list)
		{
			if (temp_iter == 0) { col_num = static_cast<int>(input_row.size()); }
			assert(("Number of columns of Input have to be same", col_num == input_row.size()));
			temp_iter += 1;
		}
		for (auto& input_row : list) { if (input_row.size() > col_num) { col_num = static_cast<int>(input_row.size()); } }
		m_mat = new double* [row_num];
		auto iterator = list.begin();         // iterate through outer list 
		for (int i = 0; i < row_num; i += 1, iterator += 1)
		{
			m_mat[i] = new double[col_num];
			std::copy(iterator->begin(), iterator->end(), m_mat[i]); // copy current inner list
		}
	}

	void set(const int& row_in, const int& col_in, const double& init = 0)
	{
		row_num = row_in; col_num = col_in;
		delete_memory(m_mat);
		m_mat = new double* [row_in];
		for (int i = 0; i < row_in; i += 1) { m_mat[i] = new double[col_in]; }
		async_data_assign(m_mat, init);
	}

	matrix& operator = (matrix& mat_in)
	{
		if (m_mat == mat_in.m_mat) { return *this; }
		row_num = mat_in.row_num;
		col_num = mat_in.col_num;
		delete_memory(m_mat);
		copy_allocation(m_mat, mat_in.m_mat);
		async_data_assign(m_mat, mat_in.m_mat);
		return *this;
	}

	matrix& operator = (matrix&& mat_in) noexcept
	{
		if (m_mat == mat_in.m_mat) { return *this; }
		row_num = mat_in.row_num;
		col_num = mat_in.col_num;
		delete_memory(m_mat);
		move_semantic(m_mat, mat_in.m_mat);
		return *this;
	}

	matrix operator *(matrix& matrix_in)
	{
		assert(("inproper matrix shape, N X M * M X N has to be set", this->col_num == matrix_in.row_num));
		double** ptr_result = async_mat_mul(m_mat, matrix_in.m_mat, this->row_num, matrix_in.col_num, this->col_num);
		matrix result{ ptr_result };
		delete_memory(ptr_result);
		return result;
	}

	matrix operator *(const double& double_in)
	{
		double** ptr_result = async_mat_times(m_mat, double_in, this->row_num, this->col_num);
		matrix result{ ptr_result };
		delete_memory(ptr_result);
		return result;
	}

	matrix operator +(matrix& matrix_in)
	{
		assert(("To get matrix sum, row_num of matrix have to be same", this->row_num == matrix_in.row_num));
		assert(("To get matrix sum, col_num of matrix have to be same", this->col_num == matrix_in.col_num));
		double** ptr_result = async_mat_sum(m_mat, matrix_in.m_mat, this->row_num, this->col_num);
		matrix result{ ptr_result };
		delete_memory(ptr_result);
		return result;
	}

	void print()
	{
		for (int i = 0; i < row_num; i += 1)
		{
			for (int j = 0; j < col_num; j += 1)
			{
				std::cout << m_mat[i][j] << " ";
			}
			std::cout << std::endl;
		}
	}

	double*& operator[](const int& idx){ return this->m_mat[idx]; }

	double**& get_pointer() { return this->m_mat; }

	void mat_swap_row(const int& idx1, const int& idx2) { swap_row(idx1, idx2, this->m_mat); }

	void mat_swap_col(const int& idx1, const int& idx2) { swap_col(idx1, idx2, this->m_mat); }

	matrix transpose()
	{ 
		int transed_row_num = static_cast<int>(_msize(m_mat[0])) / sizeof(double); 
		int transed_col_num = static_cast<int>(_msize(m_mat)) / sizeof(double*);
		double** result_ptr = async_transpose(transed_row_num, transed_col_num, m_mat);
		matrix result{ result_ptr };
		delete_memory(result_ptr);
		return result;
	}

	matrix inv()
	{
		// get inverse matrix with permutation matrix and gauss elimination
		assert(("To get an inverse matrix, row number and column number have to be same", this->row_num == this->col_num));

		matrix dum{ this->m_mat };
		matrix inverse{ identity(row_num) };
		gje_inv_pivot(dum.m_mat, inverse.m_mat);
		async_forward_elimination(dum.m_mat, inverse.m_mat);
		async_backward_elimination(dum.m_mat, inverse.m_mat);
		gje_diagonal_to_one(dum.m_mat, inverse.m_mat);
		return inverse;
	}

	matrix solve(matrix& b_in)
	{
		matrix result = b_in;
		result = this->inv() * result;
		return result;
	}

	//matrix serial_mat_mul(matrix& mat_in)
	//{
	//	return matrix{ serial_dot_product(this->m_mat, mat_in.m_mat) };
	//}

	~matrix()
	{
		if (m_mat != nullptr)
		{
			for (int i = 0; i < row_num; i += 1)
			{
				delete[] m_mat[i];
			}
			delete[] m_mat;
			m_mat = nullptr;
		}
	}
};