#ifndef MPI_MANAGER_H //include guard
#define MPI_MANAGER_H

#include "mpi.h"

class MPImanager {
private:
	int rank;
	int number_processes;
public:
	MPImanager();
	void initialize(int *argc, char ** argv[]);
	void finalize();
};

#endif