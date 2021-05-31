#pragma once
#include <future>
#include <vector>
#include "../2dmatrix/pointer_function.h"
#include <mutex>

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

void async_forward_elimination(double**& mat, double**& iden)
{
	int row_num = static_cast<int>(_msize(mat)) / sizeof(double*);
	std::vector<std::future<void>> future_vector;
	std::vector<int> task_allocation;
	future_vector.resize(concurrency_num);
	task_allocation.resize(concurrency_num);
	for (int base = 0; base < (row_num - 1); base += 1)
	{

		int all_task = row_num - 1 - base;
		int num_divided = all_task / concurrency_num; // round down by dividing with integer

		//task assignment
		for (int i = 0; i < concurrency_num; i += 1)
		{
			task_allocation[i] = num_divided;
		}
		for (int j = 0; j < concurrency_num; j += 1)
		{
			if ((all_task - num_divided * concurrency_num) == 0) { break; }
			task_allocation[j] += 1;
			all_task -= 1;
		}

		int start_task_idx = base + 1;
		unsigned int start_idx = 0;
		for (auto& task : task_allocation)
		{
			if (task == 0) { continue; }
			future_vector[start_idx] = std::async(gje_forward_elimination, std::ref(mat), std::ref(iden), base, start_task_idx, start_task_idx + task - 1);
			start_idx += 1;
			start_task_idx += task;
		}
		for (int i = 0; i < concurrency_num; i += 1)
		{
			if (task_allocation[i] == 0) { continue; }
			else { future_vector[i].wait(); }
		}
	}
}

void async_backward_elimination(double**& mat, double**& iden)
{
	int row_num = static_cast<int>(_msize(mat)) / sizeof(double*);
	std::vector<std::future<void>> future_vector;
	std::vector<int> task_allocation;
	future_vector.resize(concurrency_num);
	task_allocation.resize(concurrency_num);
	for (int base = (row_num - 1); base > 0; base -= 1)
	{

		int all_task = base;
		int num_divided = all_task / concurrency_num; // round down by dividing with integer

		//task allocation
		for (int i = 0; i < concurrency_num; i += 1)
		{
			task_allocation[i] = num_divided;
		}
		for (int j = 0; j < concurrency_num; j += 1)
		{
			if ((all_task - num_divided * concurrency_num) == 0) { break; }
			task_allocation[j] += 1;
			all_task -= 1;
		}

		int start_task_idx = base - 1;
		unsigned int start_idx = 0;
		for (auto& task : task_allocation)
		{
			if (task == 0) { continue; }
			future_vector[start_idx] = std::async(gje_backward_elimination, std::ref(mat), std::ref(iden), base, start_task_idx, start_task_idx - task + 1);
			start_idx += 1;
			start_task_idx -= task;

		}
		for (int i = 0; i < concurrency_num; i += 1)
		{
			if (task_allocation[i] == 0) { continue; }
			else { future_vector[i].wait(); }
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