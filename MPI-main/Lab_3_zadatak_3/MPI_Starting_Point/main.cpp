#include <stdio.h>
#include <mpi.h>
#include <malloc.h>
#define n 3

int main(int argc, char* argv[]) {
    int rank, size;
    MPI_Datatype kolona, novi;

    int ndims = 2;
    int dim_size[2] = { n, n };
    int periods[2] = { 0 , 0 };
    MPI_Comm cart, gornja, donja;
    int cords[2];
    int sum, rez;
    int br1 = 0;
    int br2 = 0;
    int* nizGornja = new int[(n * n - n) / 2 + n];
    int* nizDonji = new int[(n * n - n) / 2];
    MPI_Group cartG, gornjaG, donjaG;
    int rankCart1;
    int rankCart2;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Cart_create(MPI_COMM_WORLD, ndims, dim_size, periods, 0, &cart);

    MPI_Comm_group(cart, &cartG);

    MPI_Cart_coords(cart, rank, 2, cords);


    if (cords[0] <= cords[1]) {
        nizGornja[br1++] = rank;
        printf("%d\n", rank);
    }
    else {
        nizDonji[br2++] = rank;
        printf("%d\n", rank);
    }

    for (int i = 0; i < br1; i++) {
        printf("Gornja: %d\n", nizGornja[i]);
    }

    MPI_Group_incl(cartG, br1, nizGornja, &gornjaG);
    MPI_Group_incl(cartG, br2, nizDonji, &donjaG);

    MPI_Comm_create(cart, gornjaG, &gornja);
    MPI_Group_rank(gornjaG, &rankCart1);
    MPI_Comm_create(cart, donjaG, &donja);
    MPI_Group_rank(donjaG, &rankCart2);

    printf("Novi : %d\n", rankCart1);


    //if (cords[0] <= cords[1]) {
    //    printf("Novi : %d\n", rankCart);
    //    MPI_Reduce(&rankCart, &sum, 1, MPI_INT, MPI_SUM, 0, gornja);
    //}
    //else {
    //    printf("Novi : %d\n", rankCart);
    //    MPI_Reduce(&rankCart, &sum, 1, MPI_INT, MPI_SUM, 0, donja);
    //}

    //if (rankCart == 0) {
    //    //MPI_Send(&sum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    //    printf("Suma %d\n", sum);
    //}



    MPI_Finalize();

    return 0;
}