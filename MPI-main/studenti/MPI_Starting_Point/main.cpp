// salju se podaci o studentima. Podaci se nalaze u master
// procesu i salju se svim drugim procesima u komunikatoru DLAG.
// dlag ima sve elemente sa glavne dijagonale matrice procesa = pravi se cartesian
// broj indeksa ime prezime prosecna ocena
#include "mpi.h"
#include <iostream>
#include <string>
#define n 3

using namespace std;

struct
{
    int brindeksa;
	string ime;
    string prezime;
    float prosecnaocena;
} student;

int main(int argc, char* argv[])
{
    int global_rank, my_rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &global_rank);

    int dims[2] = { n, n }, periods[2] = { 0, 0 }, reorder = 0;

    MPI_Comm cart;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, &cart);

    int coords[2];
    MPI_Cart_coords(cart, global_rank, 2, coords);

    int color = coords[0] == coords[1] ? 1 : 0;

    MPI_Comm DIAG;
    MPI_Comm_split(MPI_COMM_WORLD, color, 1, &DIAG);

    MPI_Comm_rank(DIAG, &my_rank);

    size_t imeLen, prezimeLen;
    if (global_rank == 0)
    {
        std::cout << "Unesi indeks: ";
        std::cin >> student.brindeksa;
        std::cout << "Unesi ime: ";
        std::cin >> student.ime;
        std::cout << "Unesi prezime: ";
        std::cin >> student.prezime;
        std::cout << "Unesi prosecnu ocenu: ";
        std::cin >> student.prosecnaocena;

        imeLen = student.ime.length() + 1;
        prezimeLen = student.prezime.length() + 1;
    }

    MPI_Bcast(&imeLen, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&prezimeLen, 1, MPI_INT, 0, MPI_COMM_WORLD);

    char* buffer = new char[sizeof(int) + sizeof(char) * imeLen + sizeof(char) * prezimeLen + sizeof(float)];
    
    unsigned p0 = 0;
    unsigned p1 = p0 + sizeof(int);
    unsigned p2 = p1 + sizeof(char) * imeLen;
    unsigned p3 = p2 + sizeof(char) * prezimeLen;

    int* brindeksa = reinterpret_cast<int*>(&buffer[p0]);
    char* ime = reinterpret_cast<char*>(&buffer[p1]);
    char* prezime = reinterpret_cast<char*>(&buffer[p2]);
    float* prosecnaocena = reinterpret_cast<float*>(&buffer[p3]);

    if (my_rank == 0)
    {
	    *brindeksa = student.brindeksa;
	    student.ime.copy(ime, imeLen, 0);
	    ime[imeLen - 1] = '\0';
	    student.prezime.copy(prezime, prezimeLen, 0);
	    prezime[prezimeLen - 1] = '\0';
	    *prosecnaocena = student.prosecnaocena;
    }

    MPI_Aint displ[4];

    MPI_Get_address(brindeksa, &displ[0]);
    MPI_Get_address(ime, &displ[1]);
    MPI_Get_address(prezime, &displ[2]);
    MPI_Get_address(prosecnaocena, &displ[3]);

    displ[3] -= displ[0];
    displ[2] -= displ[0];
    displ[1] -= displ[0];
    displ[0] = 0;

    if (global_rank == 0)
    {
        printf("Pre slanja: %d %s %s %f \n", *brindeksa, ime, prezime, *prosecnaocena);
        printf("Pointeri: %p %p %p %p \n", brindeksa, ime, prezime, prosecnaocena);
        printf("Displ: %d %d %d %d \n", displ[0], displ[1], displ[2], displ[3]);
    }

    MPI_Datatype MPI_STRING_IME;
    MPI_Datatype MPI_STRING_PREZIME;

    MPI_Type_contiguous(imeLen, MPI_CHAR, &MPI_STRING_IME);
    MPI_Type_contiguous(prezimeLen, MPI_CHAR, &MPI_STRING_PREZIME);

    MPI_Type_commit(&MPI_STRING_IME);
    MPI_Type_commit(&MPI_STRING_PREZIME);

    int blLen[4] = { 1, 1, 1, 1 };
    MPI_Datatype oldTypes[4] = { MPI_INT, MPI_STRING_IME, MPI_STRING_PREZIME, MPI_FLOAT };

    MPI_Datatype student_type;
    MPI_Type_create_struct(4, blLen, displ, oldTypes, &student_type);

    MPI_Type_commit(&student_type);

    if (color == 1)
    {
        MPI_Bcast(buffer,  1, student_type, 0, DIAG);
        cout << global_rank << " " << *brindeksa << " " << ime << " " << prezime << " " << *prosecnaocena << endl;
    }
	
    MPI_Finalize();
    return 0;
}