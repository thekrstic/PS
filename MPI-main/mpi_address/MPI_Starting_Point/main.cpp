// heeej prox
#include <iostream>
#include "mpi.h"

int main(int argc, char** argv)
{
	int my_rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	struct { int a; double b; } values;

	int blocklens[2];
	blocklens[0] = 1;
	blocklens[1] = 1;

	MPI_Datatype old_types[2];
	old_types[0] = MPI_INT;
	old_types[1] = MPI_DOUBLE;

	MPI_Aint indices[2];

	MPI_Get_address(&values.a, &indices[0]);
	MPI_Get_address(&values.b, &indices[1]);

	indices[1] = indices[1] - indices[0];
	indices[0] = 0;

	MPI_Datatype mystruct;
	MPI_Type_create_struct(2, blocklens, indices, old_types, &mystruct);

	MPI_Type_commit(&mystruct);

	if (my_rank == 0)
	{
		std::cin >> values.a >> values.b;
	}

	MPI_Bcast(&values, 1, mystruct, 0, MPI_COMM_WORLD);

	printf("Process %d got %d and %lf\n", my_rank, values.a, values.b);

	MPI_Finalize();
	return 0;
}
