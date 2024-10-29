#include <cassert>
#include <string>

#include "iostream"
#include "mpi.h"

struct MinLocResult
{
	double min_value;
	int proc_rank;
};

int main(int argc, char** argv)
{
	int my_rank, p;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	int dims[3];

	if (my_rank == 0)
	{
		std::cin >> dims[0] >> dims[1] >> dims[2];
		assert((dims[1] % p) == 0);
		assert(dims[0] == p);
		assert(dims[2] == p);
	}

	MPI_Bcast(dims, 3, MPI_INT, 0, MPI_COMM_WORLD);

	int k = dims[0], m = dims[1], l = dims[2];

	int entities_per_proc = m / p;

	int* A_arr = new int[k * m];
	int* B_arr = new int[m * l];

	int* C_arr = new int[k * l];

	if (my_rank == 0)
	{
		std::cout << "\nMatrica A" << '\n';
		for (int i = 0; i < k; i++)
		{
			for (int j = 0; j < m; j++)
			{
				A_arr[i * m + j] = i * 100 + j;
				std::cout << A_arr[i * m + j] << " ";
			}
			std::cout << '\n';
		}
		std::cout << std::endl;

		std::cout << "\nMatrica B" << '\n';
		for (int i = 0; i < m; i++)
		{
			for (int j = 0; j < l; j++)
			{
				B_arr[i * l + j] = i * 100 + j;
				std::cout << B_arr[i * l + j] << " ";
			}
			std::cout << '\n';
		}
		std::cout << std::endl;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	MPI_Datatype column;
	MPI_Type_vector(k, 1, m, MPI_INT, &column);

	MPI_Datatype column_resized;
	MPI_Type_create_resized(column, 0, sizeof(int), &column_resized);

	MPI_Datatype columns_merged;
	MPI_Type_vector(entities_per_proc, 1, p, column_resized, &columns_merged);

	MPI_Datatype columns_merged_resized;
	MPI_Type_create_resized(columns_merged, 0, sizeof(int), &columns_merged_resized);

	MPI_Type_commit(&columns_merged_resized);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	MPI_Datatype rows_merged;
	MPI_Type_vector(entities_per_proc, l, p * l, MPI_INT, &rows_merged);

	MPI_Datatype rows_merged_resized;
	MPI_Type_create_resized(rows_merged, 0, l * sizeof(int), &rows_merged_resized);

	MPI_Type_commit(&rows_merged_resized);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////

	int* column_arr = new int[entities_per_proc * k];
	int* row_arr = new int[entities_per_proc * l];

	MPI_Scatter(A_arr, 1, columns_merged_resized, column_arr, entities_per_proc * k, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatter(B_arr, 1, rows_merged_resized, row_arr, entities_per_proc * l, MPI_INT, 0, MPI_COMM_WORLD);

	//std::string A = "", B = "";
	//for (int i = 0; i < entities_per_proc * k; i++)
	//{
	//	A += std::to_string(column_arr[i]) + " ";
	//}
	//for (int i = 0; i < entities_per_proc * l; i++)
	//{
	//	B += std::to_string(row_arr[i]) + " ";
	//}
	//std::cout << "Ja sam proces: " << my_rank << " kolona iz A je: " << A << "red iz B je: " << B  << std::endl;

	MPI_Barrier(MPI_COMM_WORLD);

	for (int i = 0; i < k; i++)
	{
		for (int j = 0; j < l; j++)
		{
			C_arr[i * k + j] = 0;
			for (int z = 0; z < entities_per_proc; z++)
			{
				int a = column_arr[i + k * z];
				int b = row_arr[z * l + j];

				C_arr[i * k + j] += a * b;
			}
		}
	}


	int sum = 0;
	for (int i = 0; i < k; i++)
	{
		for (int j = 0; j < l; j++)
		{
			sum += C_arr[i * k + j];
		}
	}

	MinLocResult local_min;
	local_min.min_value = sum;
	local_min.proc_rank = my_rank;

	MinLocResult global_min;
	MPI_Reduce(&local_min, &global_min, 1, MPI_DOUBLE_INT, MPI_MINLOC, 0, MPI_COMM_WORLD);
	MPI_Bcast(&global_min, 1, MPI_DOUBLE_INT, 0, MPI_COMM_WORLD);

	int* matrix = new int[k * l];
	MPI_Reduce(C_arr, matrix, k * l, MPI_INT, MPI_SUM, global_min.proc_rank, MPI_COMM_WORLD);

	if (my_rank == global_min.proc_rank)
	{
		std::cout << "Rezultat: \n";
		for (int i = 0; i < k; i++)
		{
			for (int j = 0; j < l; j++)
			{
				std::cout << matrix[i * l + j] << " ";
			}
			std::cout << '\n';
		}
		std::cout << std::endl;
	}

	MPI_Finalize();
	return 0;
}