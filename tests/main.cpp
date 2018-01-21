#include <thread>
#include <iostream>
#include <string>
#include <vector>
#include <time.h>
#include "mpi.h"

int main(int argc, char *argv[]) {
	int npes, rank;
	double a = 1, b;
	MPI_Request requests[2];
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &npes);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	if (rank == 0) {
		MPI_Irecv(&b, 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, &requests[0]);
		MPI_Wait(&requests[0], &status);
		std::cout << "Received " << b << std::endl;
	} else {
		MPI_Isend(&a, 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, &requests[1]);
		MPI_Wait(&requests[1], &status);
		std::cout << "Sent" << std::endl;
	}

	MPI_Finalize();

	return EXIT_SUCCESS;
}