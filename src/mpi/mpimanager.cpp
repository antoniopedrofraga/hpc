#include "mpimanager.h"


MPImanager::MPImanager(size_t size) {
	this->size = size;
}

void MPImanager::initialize(int *argc, char ** argv[]) {
	MPI_Init(argc, argv);
	MPI_Comm_size(MPI_COMM_WORLD, &number_processes);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	size_t lower = lower_bound(), upper = upper_bound();
	sub_matrices = alloc3d(SOLUTIONS_NR, (NUMBER_TIME_STEPS - 1), (upper - lower + 1));
}

void MPImanager::finalize() {
	free(sub_matrices);
	MPI_Finalize();
}

int MPImanager::get_rank() {
	return rank;
}

bool MPImanager::is_root() {
	return rank == 0;
}

bool MPImanager::is_last() {
	return rank == number_processes - 1;
}

size_t MPImanager::lower_bound() {
	return rank * size / number_processes;
}

size_t MPImanager::upper_bound() {
	return (rank + 1) * size / number_processes - 1;
}

void MPImanager::collect_results(vector<Method*> &solutions) {

	for (int p = 0; p < number_processes; p++) {
		size_t lower = p * size / number_processes, upper = (p + 1) * size / number_processes - 1;
		size_t count = solutions.size() * (NUMBER_TIME_STEPS - 1) * (upper - lower + 1);
		double buffer[count];

		//std::cout << rank << " receiving from " << p << " " << count << " bytes" <<std::endl;
		if (p != 0) {
			MPI_Recv(buffer, count, MPI_DOUBLE, p, p, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}

		for (size_t i = 0; i < solutions.size(); i++) {
			for (size_t j = 0; j < (size_t)NUMBER_TIME_STEPS - 1; j++) {
				for (size_t k = 0; k <= upper - lower; k++) {
					solutions[i]->set_value(j + 1, k + lower + 1, p != 0 ?  buffer[i * ((size_t)NUMBER_TIME_STEPS - 1) * (upper - lower + 1) + j * (upper - lower + 1) + k] : sub_matrices[i][j][k]);
				}
			}
		}

	}
}

void MPImanager::send_results() {
	size_t lower = lower_bound(), upper = upper_bound();
	size_t count = SOLUTIONS_NR * ((size_t)NUMBER_TIME_STEPS - 1) * (upper - lower + 1);
	double buffer[count];
	//std::cout << rank << " sending: " << " " << count << " bytes" << std::endl;
	for (size_t i = 0; i < SOLUTIONS_NR; i++) {
		for (size_t j = 0; j < (size_t)NUMBER_TIME_STEPS - 1; j++) {
			for (size_t k = 0; k <= upper - lower; k++) {
				buffer[i * ((size_t)NUMBER_TIME_STEPS - 1) * (upper - lower + 1) + j * (upper - lower + 1) + k] = sub_matrices[i][j][k];
			}
		}
	}
	std::cout << "Sending " << rank << std::endl;
	MPI_Send(buffer, count, MPI_DOUBLE, 0, rank, MPI_COMM_WORLD);
}

void MPImanager::add_sub_matrix(size_t i, double ** sub_matrix) {
	sub_matrices[i] = sub_matrix;
}