#include "mpimanager.h"


MPImanager::MPImanager() {}

void MPImanager::initialize(int *argc, char ** argv[]) {
	MPI_Init(argc, argv);
	MPI_Comm_size(MPI_COMM_WORLD, &number_processes);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
}

void MPImanager::finalize() {
	MPI_Finalize();
}