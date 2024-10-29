#include "iostream"
#include "mpi.h"

int main(int argc, char** argv)
{
	MPI_Init(&argc, &argv);

	int my_rank, p, n;
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	n = (int)sqrt(p);

	int dims[2] = { n, n };
	int periods[2] = { 0, 0 };

	MPI_Comm cart_comm;

	MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &cart_comm);

	int coords[2];
	MPI_Cart_coords(cart_comm, my_rank, 2, coords);


	bool is_in_top_half = coords[0] <= coords[1];
	std::cout << "Ja sam proces broj: " << my_rank << ", koordinate su mi: " << coords[0] << " : " << coords[1] << " i " << (is_in_top_half ? "iznad sam" : "ispod sam") << '\n';// << std::fflush;
	//std::cout << "Ja sam proces br: " << my_rank << " i " << (is_in_top_half ? "iznad sam" : "ispod sam") << '\n' << std::fflush;

	MPI_Comm new_comm;
	MPI_Comm_split(cart_comm, is_in_top_half ? 1 : 0, 99, &new_comm);

	int new_rank;
	MPI_Comm_rank(new_comm, &new_rank);

	int sum;
	MPI_Reduce(&my_rank, &sum, 1, MPI_INT, MPI_SUM, 1, new_comm);

	if (new_rank == 1)
	{
		int tag = is_in_top_half ? 0 : 1;
		MPI_Send(&sum, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
	}
	int sum0, sum1;
	if (my_rank == 0)
	{
		MPI_Status status;
		MPI_Recv(&sum0, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(&sum1, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);

		std::cout << "Ja sam root. Primio sam za sum0: " << sum0 << ", za sum1: " << sum1 << '\n' << std::fflush;
	}


	MPI_Finalize();
	return 0;
}