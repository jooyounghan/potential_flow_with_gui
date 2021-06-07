#pragma once
#include <future>
#include <vector>
#include "../2dmatrix/pointer_function.h"

void gje_inv_pivot(double**& origin, double**& identity, const int& idx, const int& row_num, const int& col_num)
{
	assert(("numbers of row and column has to be same", row_num == col_num));
	assert(("sizes between two matrixes have to be same", row_num == static_cast<int>(_msize(identity)) / sizeof(double*) && col_num == static_cast<int>(_msize(identity[0])) / sizeof(double)));
	if (std::abs(origin[idx][idx]) < EPSILON)
	{
		for (int i = idx; i < row_num; i += 1)
		{
			if (std::abs(origin[i][idx]) > std::abs(origin[idx][idx]))
			{ swap_row(idx, idx + 1, origin); swap_row(idx, idx + 1, identity); break; }
		}
	}
}

void gje_diagonal_to_one(double**& mat, double**& iden, const int& row_num, const int& col_num)
{
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

void gje_elimination(double**& mat, double**& iden, const int& base_idx, const int& calc_idx, const int& row_num, const int& col_num)
{
	double fact = mat[calc_idx][base_idx] / mat[base_idx][base_idx];
	for (int j = 0; j < col_num; j += 1)
	{
		mat[calc_idx][j] = mat[calc_idx][j] - fact * mat[base_idx][j];
		iden[calc_idx][j] = iden[calc_idx][j] - fact * iden[base_idx][j];
	}
}


void async_data_assign(double**& mat, double**& mat_other)
{
	int row_num = static_cast<int>(_msize(mat)) / sizeof(double*);
	int col_num = static_cast<int>(_msize(mat[0])) / sizeof(double);
	std::vector<std::future<void>> future_vector;
	if (row_num >= col_num)
	{
		future_vector.resize(row_num);
		for (int i = 0; i < future_vector.size(); i += 1)
		{
			future_vector[i] = std::async(replace_row_dir, std::ref(mat), std::ref(mat_other), i, std::ref(col_num));
		}
	}
	else
	{
		future_vector.resize(col_num);
		for (int i = 0; i < future_vector.size(); i += 1)
		{
				future_vector[i] = std::async(replace_col_dir, std::ref(mat), std::ref(mat_other), i, std::ref(row_num));
		}
	}
}

void async_data_assign(double**& mat, const double &init)
{
	int row_num = static_cast<int>(_msize(mat))/ sizeof(double*);
	int col_num = static_cast<int>(_msize(mat[0])) / sizeof(double);
	std::vector<std::future<void>> future_vector;
	if (row_num >= col_num)
	{
		future_vector.resize(row_num);
		for (int i = 0; i < row_num; i += 1)
		{
			future_vector[i] = std::async(replace_row_dir_const, std::ref(mat), std::ref(init), i, std::ref(col_num));
		}
	}
	else
	{
		future_vector.resize(col_num);
		for (int i = 0; i < col_num; i += 1)
		{
			future_vector[i] = std::async(replace_col_dir_const, std::ref(mat), std::ref(init),i, std::ref(row_num));
		}
	}
}

void async_forward_elimination(double**& mat, double**& iden, const int& row_num, const int& col_num)
{
	for (int i = 0; i < row_num; i += 1)
	{
		gje_inv_pivot(mat, iden, i, row_num, col_num);
		std::vector<std::future<void>> task_vector;
		task_vector.resize(row_num - (i + 1));
		for (int j = 0; j < task_vector.size(); j += 1)
		{
			int calc_idx = i + 1 + j;
			task_vector[j] = std::async(gje_elimination, std::ref(mat), std::ref(iden), i, calc_idx, std::ref(row_num), std::ref(col_num));
		}
		for (int j = 0; j < task_vector.size(); j += 1)
		{
			task_vector[j].wait();
		}
	}
}

void async_backward_elimination(double**& mat, double**& iden, const int& row_num, const int& col_num)
{
	for (int i = (row_num - 1); i >= 0; i -= 1)
	{
		std::vector<std::future<void>> task_vector;
		task_vector.resize(i);
		for (int j = 0; j < task_vector.size(); j += 1)
		{
			task_vector[j] = std::async(gje_elimination, std::ref(mat), std::ref(iden), i, j, std::ref(row_num), std::ref(col_num));
		}
		for (int j = 0; j < task_vector.size(); j += 1)
		{
			task_vector[j].wait();
		}
	}
}

//serial_gje_method for comparing

void serial_gje_forward_elimination(double**& mat, double**& iden)
{
	int row_num = _msize(mat) / sizeof(double*);
	int col_num = _msize(mat[0]) / sizeof(double);
	for (int i = 0; i < row_num - 1; i += 1)
	{
		gje_inv_pivot(mat, iden, i, row_num, col_num);
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




double** async_mat_mul(double**& mat_1, double**& mat_2, const int& r_row, const int& r_col, const int&k_col)
{
	double** result = generate(r_row, r_col);

	std::vector<std::vector<std::future<void>>> future_vector_2d;
	//future_vector_2d = (r_row, r_col);
	future_vector_2d.resize(r_row);
	for (int i = 0; i < r_row; i += 1)
	{
		future_vector_2d[i].resize(r_col);
	}

	/*
	allocate task with the grid
	std::vector<std::vector<std::future<void>>> future_vector_2d
	will be 2d matrix of std::futur(void)
	*/

	//parallel multiple
	for (int i = 0; i < r_row; i += 1)
	{
		for (int j = 0; j < r_col; j += 1)
		{
			future_vector_2d[i][j] = std::async([&, i, j] {
					double dum = 0;
					for (int k = 0; k < k_col; k += 1)
					{
						dum += mat_1[i][k] * mat_2[k][j];
					}
					result[i][j] = dum;
				});
		}
	}
	return result;
}

double** async_mat_times(double**& mat_in, const double& times, const int& r_row, const int& r_col)
{
	double** result = generate(r_row, r_col);

	std::vector<std::vector<std::future<void>>> future_vector_2d;
	//future_vector_2d = (r_row, r_col);
	future_vector_2d.resize(r_row);
	for (int i = 0; i < r_row; i += 1)
	{
		future_vector_2d[i].resize(r_col);
	}
	//parallel times
	for (int i = 0; i < r_row; i += 1)
	{
		for (int j = 0; j < r_col; j += 1)
		{
			future_vector_2d[i][j] = std::async([&, i, j] {
					result[i][j] = mat_in[i][j] * times;
				});
		}
	}
	return result;
}

double** async_mat_sum(double**& mat_1, double**& mat_2, const int& r_row, const int& r_col)
{
	double** result = generate(r_row, r_col);

	std::vector<std::vector<std::future<void>>> future_vector_2d;
	//future_vector_2d = (row_num_1, col_num_1) = (row_num_2, col_num_2)
	future_vector_2d.resize(r_row);
	for (int i = 0; i < r_row; i += 1)
	{
		future_vector_2d[i].resize(r_col);
	}

	//parallel sum
	for (int i = 0; i < r_row; i += 1)
	{
		for (int j = 0; j < r_col; j += 1)
		{
			future_vector_2d[i][j] = std::async([&, i, j] {
					result[i][j] = mat_1[i][j] + mat_2[i][j];
				});
		}
	}
	return result;
}

double** async_transpose(const int& transed_row, const int& transed_col, double**& mat)
{
	double** result = generate(transed_row, transed_col);
	std::vector<std::future<void>> transpose_task;
	transpose_task.resize(transed_col);
	for (int i = 0; i < transpose_task.size(); i += 1)
	{
		transpose_task[i] = std::async([&, i] {
			for (int j = 0; j < transed_row; j += 1)
			{
				result[j][i] = mat[i][j];
			}
			});
	}
	for (int i = 0; i < transpose_task.size(); i += 1)
	{
		transpose_task[i].wait();
	}
	return result;
}