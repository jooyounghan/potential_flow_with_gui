#pragma once
#include "../constants.h"

bool is_same(double**& mat1, double**& mat2, const int& start_row, const int &end_row)
{
	int row_num1 = static_cast<int>(_msize(mat1)) / sizeof(double*);
	int col_num1 = static_cast<int>(_msize(mat1[0])) / sizeof(double);
	int row_num2 = static_cast<int>(_msize(mat2)) / sizeof(double*);
	int col_num2 = static_cast<int>(_msize(mat2[0])) / sizeof(double);
	if (row_num1 != row_num2 || col_num1 != col_num2) { return false; }
	for (int i = start_row; i < end_row + 1; i += 1)
	{
		for (int j = 0; j < col_num1; j += 1)
		{
			if (std::abs(mat1[i][j] - mat2[i][j]) > EPSILON)
			{
				std::cout << i << " " << j << std::endl;

				return false;
			}
		}
	}
	return true;
}

void print(double**& origin)
{
	int row_num = static_cast<int>(_msize(origin)) / sizeof(double*);
	int col_num = static_cast<int>(_msize(origin[0])) / sizeof(double);
	for (int i = 0; i < row_num; i += 1)
	{
		for (int j = 0; j < col_num; j += 1)
		{
			std::cout << origin[i][j] << " ";
		}
		std::cout << std::endl;
	}
}
//
//double** generate_noraml_distribution(const int& row, const int& col)
//{
//	std::random_device rd;
//	std::mt19937 g(rd());
//	std::normal_distribution<> d{ 0, 1 };
//
//	double** mat = new double* [row];
//	for (int i = 0; i < row; i += 1)
//	{
//		mat[i] = new double[col];
//	}
//	for (int i = 0; i < row; i += 1)
//	{
//		for (int j = 0; j < col; j += 1)
//		{
//			mat[i][j] = d(g);
//		}
//	}
//	return mat;
//}

double** generate(const int& row, const int& col)
{
	double** mat = new double* [row];
	for (int i = 0; i < row; i += 1)
	{
		mat[i] = new double[col];
	}
	return mat;
}

void copy_allocation(double**& origin, double**& copy)
{
	int row_num = static_cast<int>(_msize(copy)) / sizeof(double*);
	int col_num = static_cast<int>(_msize(copy[0])) / sizeof(double);
	origin = new double* [row_num];
	for (int i = 0; i < row_num; i += 1) { origin[i] = new double[col_num]; }
}

void delete_memory(double**& origin)
{
	if (origin != nullptr)
	{
		for (int i = 0; i < _msize(origin) / sizeof(double*); i += 1)
		{
			delete[] origin[i];
		}
		delete[] origin;
		origin = nullptr;
	}
}

void copy_semantic(double**& origin, double**& copy)
{
	int row_num = static_cast<int>(_msize(copy)) / sizeof(double*);
	int col_num = static_cast<int>(_msize(copy[0])) / sizeof(double);
	delete_memory(origin);
	copy_allocation(origin, copy);
	for (int i = 0; i < row_num; i += 1)
	{
		for (int j = 0; j < col_num; j += 1)
		{
			origin[i][j] = copy[i][j];
		}
	}
}

void move_semantic(double**& origin, double**& move)
{
	origin = move;
	for (int i = 0; i < static_cast<int>(_msize(move)) / sizeof(double*); i += 1)
	{
		origin[i] = move[i];
	}
	move = nullptr;
}

double** identity(const int& dim)
{
	double** iden = new double* [dim];
	for (int i = 0; i < dim; i += 1)
	{
		iden[i] = new double[dim] {0};
		iden[i][i] = 1;
	}
	return iden;
}

void replace_row_dir(double**& mat, double**& mat_other, const int& row_num, const int& col_num)
{
	for (int i = 0; i < col_num; i += 1)
	{
		mat[row_num][i] = mat_other[row_num][i];
	}
}

void replace_col_dir(double**& mat, double**& mat_other, const int& col_num, const int& row_num)
{
	for (int i = 0; i < row_num; i += 1)
	{
		mat[i][col_num] = mat_other[i][col_num];
	}
}

void replace_row_dir_const(double**& mat, const double& init, const int& row_idx, const int& col_num)
{
	for (int i = 0; i < col_num; i += 1)
	{
		mat[row_idx][i] = init;
	}
}

void replace_col_dir_const(double**& mat, const double& init, const int& col_idx, const int& row_num)
{
	for (int i = 0; i < row_num; i += 1)
	{
		mat[i][col_idx] = init;
	}
}

void swap_row(const int& idx1, const int& idx2, double**& mat)
{
	int row_num_idx = static_cast<int>(_msize(mat)) / sizeof(double*);
	assert(("To swap row, input indexes have to be smaller than matrix's row size", row_num_idx > idx1 || row_num_idx > idx2));
	double* temp = nullptr;
	temp = mat[idx1];
	mat[idx1] = mat[idx2];
	mat[idx2] = temp;
	temp = nullptr;
}

void swap_col(const int& idx1, const int& idx2, double**& mat)
{
	int row_num = static_cast<int>(_msize(mat)) / sizeof(double*);
	int col_num_idx = static_cast<int>(_msize(mat[0])) / sizeof(double);
	assert(("To swap col, input indexes have to be smaller than matrix's row size", col_num_idx > idx1 || col_num_idx > idx2));
	for (int i = 0; i < row_num; i += 1)
	{
		double temp = 0;
		temp = mat[i][idx1];
		mat[i][idx1] = mat[i][idx2];
		mat[i][idx2] = temp;
	}
}


void gje_inv_pivot(double**& origin, double**& identity)
{
	// origin's row and column number is same with identity's row and column
	int row_num = static_cast<int>(_msize(origin)) / sizeof(double*);
	int col_num = static_cast<int>(_msize(origin[0])) / sizeof(double);
	assert(("numbers of row and column has to be same", row_num == col_num));
	if (row_num != static_cast<int>(_msize(identity)) / sizeof(double*) || col_num != static_cast<int>(_msize(identity[0])) / sizeof(double))
	{ assert(("sizes between two matrixes have to be same", row_num == col_num)); }
	for (int i = 0; i < row_num; i += 1)
	{
		int high_idx = i;
		for (int j = i; j < row_num; j += 1)
		{
			if (i == 0) { if (abs(origin[j][i]) > abs(origin[high_idx][i])) { high_idx = j; } }
			else { if ((abs(origin[j][i]) > abs(origin[high_idx][i])) && (abs(origin[high_idx - 1][i] - origin[high_idx][i]) < EPSILON)) { high_idx = j; } } //need when matrix larger than 3
		}
		if (0 < i && i < (row_num - 1)) { if (abs(origin[i - 1][i] - origin[i][i]) < EPSILON) { high_idx = i + 1; } }
		swap_row(i, high_idx, origin);
		swap_row(i, high_idx, identity);
	}
}

void gje_diagonal_to_one(double**& mat, double**& iden)
{
	int row_num = static_cast<int>(_msize(mat)) / sizeof(double*);
	int col_num = static_cast<int>(_msize(mat[0])) / sizeof(double);
	for (int i = 0; i < row_num; i += 1)
	{
		double divider = mat[i][i];
		mat[i][i] = mat[i][i] / divider;
		for (int j = 0; j < col_num; j += 1)
		{
			iden[i][j] = iden[i][j] / divider;
		}
	}
}

void gje_forward_elimination(double**& mat, double**& iden, const int& base_idx, const int& start_idx, const int& end_idx)
{
	assert(("end index has to be higher than start", start_idx <= end_idx));
	assert(("base index has to be lower than start and end index", base_idx < start_idx && base_idx < end_idx));
	int col_num = static_cast<int>(_msize(mat[0])) / sizeof(double);
	for (int i = start_idx; i <= end_idx; i += 1)
	{
		double fact = mat[i][base_idx] / mat[base_idx][base_idx];
		for (int j = 0; j < col_num; j += 1)
		{
			mat[i][j] = mat[i][j] - fact * mat[base_idx][j];
			iden[i][j] = iden[i][j] - fact * iden[base_idx][j];
		}
	}
}

void gje_backward_elimination(double**& mat, double**& iden, const int& base_idx, const int& start_idx, const int& end_idx)
{
	assert(("start index has to be higher than end index", start_idx >= end_idx));
	assert(("base index has to be higher than start and end index", base_idx > start_idx && base_idx > end_idx));
	int row_num = static_cast<int>(_msize(mat)) / sizeof(double*);
	int col_num = static_cast<int>(_msize(mat[0])) / sizeof(double);
	for (int i = start_idx; i >= end_idx; i -= 1)
	{
		double fact = mat[i][base_idx] / mat[base_idx][base_idx];
		for (int j = 0; j < col_num; j += 1)
		{
			mat[i][j] = mat[i][j] - fact * mat[base_idx][j];
			iden[i][j] = iden[i][j] - fact * iden[base_idx][j];
		}
	}
}


//test for serial gje computation

/*
void serial_gje_forward_elimination(double**& mat, double**& iden)
{
	int row_num = _msize(mat) / sizeof(double*);
	int col_num = _msize(mat[0]) / sizeof(double);
	for (int i = 0; i < row_num - 1; i += 1)
	{
		for (int j = i + 1; j < row_num; j += 1)
		{
			double fact = mat[j][i] / mat[i][i];
			for (int k = 0; k < col_num; k += 1)
			{
				mat[j][k] = mat[j][k] - fact * mat[i][k];
				iden[j][k] = iden[j][k] - fact * iden[i][k];
			}
		}
	}
}

void serial_gje_backward_elimination(double**& mat, double**& iden)
{
	int row_num = _msize(mat) / sizeof(double*);
	int col_num = _msize(mat[0]) / sizeof(double);
	for (int i = 0; i < row_num - 1; i += 1)
	{
		for (int j = i + 1; j < row_num; j += 1)
		{
			double fact = mat[row_num - 1 - j][row_num - 1 - i] / mat[row_num - 1 - i][row_num - 1 - i];
			for (int k = 0; k < col_num; k += 1)
			{
				mat[row_num - 1 - j][col_num - 1 - k] = mat[row_num - 1 - j][col_num - 1 - k] - fact * mat[row_num - 1 - i][col_num - 1 - k];
				iden[row_num - 1 - j][col_num - 1 - k] = iden[row_num - 1 - j][col_num - 1 - k] - fact * iden[row_num - 1 - i][col_num - 1 - k];
			}
		}
	}
}

void serial_gje(double**& mat, double**& iden)
{
	gje_inv_pivot(mat, iden);
	serial_gje_forward_elimination(mat, iden);
	serial_gje_backward_elimination(mat, iden);
	gje_diagonal_to_one(mat, iden);
}
*/

//test for serial dot prouct computation

/*
double** serial_dot_product(double**& mat_1, double**& mat_2)
{
	int row_num_1 = _msize(mat_1) / sizeof(double*);
	int col_num_1 = _msize(mat_1[0]) / sizeof(double);
	int row_num_2 = _msize(mat_2) / sizeof(double*);
	int col_num_2 = _msize(mat_2[0]) / sizeof(double);
	assert(("matrixes sizes have to be same"), row_num_1 == row_num_2 && col_num_1 == col_num_2);
	assert(("To dot product, N X M * M X N (col_num of first matrix and row_num of second matrix have to be same"), col_num_1 == row_num_2);
	double** result = generate(row_num_1, col_num_2);

	for (int i = 0; i < row_num_1; i += 1)
	{
		for (int j = 0; j < col_num_1; j += 1)
		{
			double dum = 0;
			for (int k = 0; k < col_num_1; k += 1)
			{
				dum += mat_1[i][k] * mat_2[k][j];
			}
			result[i][j] = dum;
		}
	}
	return result;
}
*/
