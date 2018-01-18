#include "mpimanager.h"


MPImanager::MPImanager(size_t size) {
	this->size = size;
}

void MPImanager::initialize(int *argc, char ** argv[]) {
	MPI_Init(argc, argv);
	MPI_Comm_size(MPI_COMM_WORLD, &number_processes);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
}

void MPImanager::finalize() {
	MPI_Finalize();
}

int MPImanager::get_rank() {
	return rank;
}

bool MPImanager::is_root() {
	return rank == 0;
}

size_t MPImanager::lower_bound() {
	return rank * size / number_processes;
}

size_t MPImanager::upper_bound() {
	return (rank + 1) * size / number_processes - 1;
}

void MPImanager::collect_results(size_t solutions_size) {

	for (int i = 1; i < number_processes; i++) {
		size_t lower = i * size / number_processes, upper = (i + 1) * size / number_processes - 1;
		size_t size = solutions_size * (upper - lower) * NUMBER_TIME_STEPS;
		double segment[solutions_size][(size_t)NUMBER_TIME_STEPS][upper - lower];

		std::cout << rank << " receiving from " << i << std::endl;
		MPI_Recv(segment, size, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		for (size_t i = 0; i < solutions_size; i++) {
			for (size_t j = 0; j < (size_t)NUMBER_TIME_STEPS; j++) {
				for (size_t k = 0; k < upper - lower; k++) {
					std::cout << segment[i][j][k] << " ";
				}
				std::cout << std::endl;
			}
			std::cout << std::endl;
			std::cout << std::endl;
		}
	}
}

void MPImanager::send_results() {
	std::cout << rank << " sending" << std::endl;
	MPI_Send(&sub_matrices, sizeof(sub_matrices), MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
}

void MPImanager::add_sub_matrix(vector<vector<double>> sub_matrix) {
	sub_matrices.push_back(sub_matrix);
}