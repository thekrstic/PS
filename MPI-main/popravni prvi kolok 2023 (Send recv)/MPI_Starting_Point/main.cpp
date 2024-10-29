#include <cassert>

#include "iostream"
#include "mpi.h"

int main(int argc, char** argv)
{
	int my_rank, p;

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &p);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	int dims[2], periods[2] = { 0, 0 };

	if (my_rank == 0)
	{
		std::cin >> dims[0] >> dims[1];
		assert((dims[0] * dims[1]) == p);
	}

	MPI_Bcast(dims, 2, MPI_INT, 0, MPI_COMM_WORLD);

	MPI_Comm cart_comm;
	MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &cart_comm);

	int coords[2];
	MPI_Cart_coords(cart_comm, my_rank, 2, coords);

	MPI_Comm row_comm;
	MPI_Comm_split(cart_comm, coords[1], 99, &row_comm);

	int parity = coords[0] % 2;
	MPI_Comm column_comm;
	MPI_Comm_split(row_comm,parity, 99, &column_comm);

	int column_comm_rank;
	MPI_Comm_rank(column_comm, &column_comm_rank);

	int dest, source;
	MPI_Cart_shift(column_comm, 0, 1, &source, &dest);

	// Gubi se topologija nakon splita


	int sum;

	if (column_comm_rank == 0)
	{
		int k = 10;
		sum = k;
	}
	else
	{
		MPI_Status status;
		MPI_Recv(&sum, 1, MPI_INT, source, MPI_ANY_TAG, column_comm, &status);
	}

	sum += my_rank;

	if (dest != MPI_PROC_NULL)
	{
		MPI_Send(&sum, 1, MPI_INT, dest, 0, column_comm);
	}



	std::cout << sum << '\n';

	MPI_Finalize();
	return 0;
}