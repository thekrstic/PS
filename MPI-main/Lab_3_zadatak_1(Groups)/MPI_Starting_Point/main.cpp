#include "iostream"
#include "mpi.h"

#define COLUMN_SIZE 4
#define V 1
int main(int argc, char** argv)
{
	int my_rank, p;

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	bool should_be_in_group = ((my_rank % 5) == 0);

	int included_ranks_count = (p - 1) / 5 + 1;
	int* included_ranks = new int[included_ranks_count];

	for (int i = 0; i < included_ranks_count; i++)
	{
		included_ranks[i] = i * 5;
	}

	MPI_Group old_group, new_group;
	MPI_Comm our_comm;

	MPI_Comm_group(MPI_COMM_WORLD, &old_group);
	MPI_Group_incl(old_group, included_ranks_count, included_ranks, &new_group);
	MPI_Comm_create(MPI_COMM_WORLD, new_group, &our_comm);

	if (should_be_in_group)
	{
		int q;
		MPI_Comm_size(our_comm, &q);
		std::cout << "I\'am in a group, my rank is: " << my_rank << '\n' << std::flush;

		const auto matrix = new int[q * COLUMN_SIZE];

		if (my_rank == 0)
		{
			for (int i = 0; i < q; i++)
			{
				for (int j = 0; j < COLUMN_SIZE; j++)
				{
					matrix[i * COLUMN_SIZE + j] = i;
				}
			}
		}

		int column[COLUMN_SIZE];

		MPI_Scatter(matrix, COLUMN_SIZE, MPI_INT, column, COLUMN_SIZE, MPI_INT, 0, our_comm);

		
		int sum = 0;
		for (int i = 0; i < COLUMN_SIZE; i++)
		{
			sum += column[i];
		}

		if (sum >= V)
		{
			std::cout << "I satisfy condition, my rank is: " << my_rank << ", my sum is: " << sum << '\n';
		}
	}
	MPI_Finalize();
	return 0;
}