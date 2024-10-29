#include "iostream"
#include "mpi.h"

int main(int argc, char** argv)
{
	int my_rank, p;
	
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	int n = sqrt(p);

	MPI_Comm cart_comm;
	int dims[2] = { n, n }, periods[2] = { 0, 0 };

	MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &cart_comm);

	int cart_rank;
	MPI_Comm_rank(cart_comm, &cart_rank);

	int coords[2];
	MPI_Cart_coords(cart_comm, cart_rank, 2, coords);
	
	bool is_on_diags = (coords[0] == coords[1]) || ((n - 1 - coords[0]) == coords[1]);

	MPI_Comm diag_comm;
	MPI_Comm_split(cart_comm, is_on_diags ? 1 : 0, 99, &diag_comm);

	int new_rank;
	MPI_Comm_rank(diag_comm, &new_rank);

	int magic_number;
	if (new_rank == 0)
	{
		magic_number = 69;
	}

	if (is_on_diags)
	{
		MPI_Bcast(&magic_number, 1, MPI_INT, 0, diag_comm);

		std::cout << "I'am process number: " << my_rank << ", my new rank is: " << new_rank << ", I received: " << magic_number << '\n';
	}


	MPI_Finalize();
	return 0;
}