#include "iostream"
#include "mpi.h"

#define COLUMN_SIZE 4
#define V 1
int main(int argc, char** argv)
{
	int my_rank, p, q;
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	q = static_cast<int>(sqrt(p));
	bool should_be_in_group = ((my_rank % 5) == 0);
	int color = should_be_in_group ? 1 : 0;

	MPI_Comm our_comm;

	MPI_Comm_split(MPI_COMM_WORLD, color, 99, &our_comm);

	if (should_be_in_group)
	{
		std::cout << "I\'am in group, my rank is: " << my_rank << '\n' << std::flush;

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