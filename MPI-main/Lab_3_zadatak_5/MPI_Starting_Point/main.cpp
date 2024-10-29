#include "iostream"
#include "mpi.h"

int main(int argc, char** argv)
{
	MPI_Init(&argc, &argv);
	int my_rank, p;

	MPI_Comm_size(MPI_COMM_WORLD, &p);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	int n = sqrt(p);
	int dims[2] = { n, n };
	int periods[2] = { 0, 0 };

	MPI_Comm cart_comm;
	MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &cart_comm);

	int color = my_rank % n;

	MPI_Comm column_comm;
	MPI_Comm_split(cart_comm, color, 99, &column_comm);

	int new_rank;
	MPI_Comm_rank(column_comm, &new_rank);

	int max_rank;
	MPI_Reduce(&my_rank, &max_rank, 1, MPI_INT, MPI_MAX, 0, column_comm);
	if (new_rank == 0)
	{
		std::cout << "Ja sam proces: " << my_rank << ", i maksimalni rank ove kolone je: " << max_rank << '\n';
	}



	MPI_Finalize();
	return 0;
}