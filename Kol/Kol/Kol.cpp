#include <string>
#include <iostream>
#include <mpi.h>
using namespace std;

#define r 6
#define m 3
#define k 2

void main(int argc, char* argv[]) {

	int p, rank;
	int A[r][m], B[m][k];
	MPI_Datatype vector, rows;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	int stride = p * m;
	if (p == r) stride = m;
	MPI_Type_vector(r / p, m, stride, MPI_INT, &vector);
	MPI_Type_commit(&vector);
	MPI_Type_create_resized(vector, 0, m * sizeof(int), &rows);
	MPI_Type_commit(&rows);

	int* rowsA = (int*)malloc(r / p * m * sizeof(int));
	int* local_C = (int*)calloc(r * k, sizeof(int));;
	int* C = (int*)calloc(r * k, sizeof(int));

	if (rank == 0) {
		printf("A: \n");
		for (int i = 0; i < r; i++) {
			for (int j = 0; j < m; j++) {
				A[i][j] = i + j - 1;
				printf("%d ", A[i][j]);
			}
			printf("\n");
		}
		printf("\n\nB: \n");
		for (int i = 0; i < m; i++) {
			for (int j = 0; j < k; j++) {
				B[i][j] = i;
				printf("%d ", B[i][j]);
			}
			printf("\n");
		}

	}

	MPI_Scatter(A, 1, rows, rowsA, (r / p) * m, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(B, m * k, MPI_INT, 0, MPI_COMM_WORLD);

	int start = rank * 2;
	for (int i = 0; i < r / p; i++) {
		for (int j = 0; j < k; j++) {
			local_C[start + k * p * i + j] = 0;
			for (int q = 0; q < m; q++)
				local_C[start + k * p * i + j] += rowsA[q + i * m] * B[q][j];
		}
	}

	printf("\n\nlocA: %d\n", rank);
	for (int i = 0; i < r / p; i++) {
		for (int j = 0; j < m; j++) {
			printf("%d ", rowsA[i * r / p + j]);
		}
		printf("\n");
	}

	printf("\n\nlocC: %d\n", rank);
	for (int i = 0; i < r; i++) {
		for (int j = 0; j < k; j++) {
			printf("%d ", local_C[i * r + j]);
		}
		printf("\n");
	}

	MPI_Reduce(local_C, C, r * k, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		for (int i = 0; i < r; i++) {
			for (int j = 0; j < k; j++) {
				cout << C[i * k + j] << ' ';
			}
			cout << endl;
		}
	}

	MPI_Finalize();
}