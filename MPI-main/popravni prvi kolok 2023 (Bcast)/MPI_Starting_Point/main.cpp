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

	int k;
	if (parity == 0)
	{
		if (column_comm_rank == 0) k = 100 * coords[1];
		MPI_Bcast(&k, 1, MPI_INT, 0, column_comm);
	}
	else
	{
		if (column_comm_rank == 0) k = 1000 * coords[1];
		MPI_Bcast(&k, 1, MPI_INT, 0, column_comm);
	}

	int sum = k + my_rank;
	std::cout << sum << '\n';

	MPI_Finalize();
	return 0;
}