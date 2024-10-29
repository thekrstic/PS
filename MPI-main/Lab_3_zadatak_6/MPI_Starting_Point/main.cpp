#include <assert.h>

#include "iostream"
#include "mpi.h"

int main(int argc, char** argv)
{
	MPI_Init(&argc, &argv);

	int my_rank, p;

	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	int dims[2], periods[2] = { 0, 0 };

	if (my_rank == 0)
	{
		std::cin >> dims[0] >> dims[1];
		assert((dims[0] * dims[1]) == p);
	}

	MPI_Bcast(dims, 2, MPI_INT, 0, MPI_COMM_WORLD);

	MPI_Comm cart_comm;
	MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &cart_comm);

	int dest, source;
	MPI_Cart_shift(cart_comm, 1, 1, &source, &dest);

	if (dest == MPI_PROC_NULL)
	{
		dest = 1;
	}
	else if (source == MPI_PROC_NULL)
	{
		source = 1;
	}

	int product = dest * source;

	int sum;
	MPI_Reduce(&product, &sum, 1, MPI_INT, MPI_SUM, 0, cart_comm);

	if (my_rank == 0)
	{
		std::cout << "Suma je: " << sum << '\n';
	}

	MPI_Finalize();
	return 0;
}