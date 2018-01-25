#include "mpimanager.h"


MPImanager::MPImanager() {}

void MPImanager::initialize(int *argc, char ** argv[]) {
	MPI_Init(argc, argv);
	MPI_Comm_size(MPI_COMM_WORLD, &number_processes);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
}

void MPImanager::set_size(size_t size) {
	this->size = size;

	if ((int)size < number_processes) {
		MPI_Group world_group;
		MPI_Comm_group(MPI_COMM_WORLD, &world_group);

		MPI_Group new_group;
		int ranges[3] = {(int)size, number_processes - 1, 1 };
		MPI_Group_range_excl(world_group, 1, &ranges, &new_group);

		MPI_Comm_create(MPI_COMM_WORLD, new_group, &world);

		used_processes = size;
	} else {
		world = MPI_COMM_WORLD;
		used_processes = number_processes;
	}

	size_t lower = lower_bound(), upper = upper_bound();
	sub_matrices = alloc3d(SOLUTIONS_NR, (NUMBER_TIME_STEPS - 1), (upper - lower + 1));


}

MPI_Comm MPImanager::get_world() {
	return world;
}

bool MPImanager::part_of_world() {
	return rank < (int)used_processes;
}

void MPImanager::finalize() {
	free(sub_matrices);
	MPI_Finalize();
}

int MPImanager::get_rank() {
	return rank;
}

int MPImanager::get_number_processes() {
	return used_processes;
}

bool MPImanager::is_root() {
	return rank == 0;
}

bool MPImanager::is_last() {
	return rank == used_processes - 1;
}

bool MPImanager::one_process() {
	return 0 == used_processes;
}

size_t MPImanager::lower_bound() {
	return rank * size / used_processes;
}

size_t MPImanager::upper_bound() {
	return (rank + 1) * size / used_processes - 1;
}

void MPImanager::collect_results(vector<Method*> &solutions) {

	for (int p = 0; p < used_processes; p++) {
		size_t lower = p * size / used_processes, upper = (p + 1) * size / used_processes - 1;
		size_t count = solutions.size() * (NUMBER_TIME_STEPS - 1) * (upper - lower + 1);
		double buffer[count];

		if (p != 0) {
			MPI_Recv(buffer, count, MPI_DOUBLE, p, p, world, MPI_STATUS_IGNORE);
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

	for (size_t i = 0; i < SOLUTIONS_NR; i++) {
		for (size_t j = 0; j < (size_t)NUMBER_TIME_STEPS - 1; j++) {
			for (size_t k = 0; k <= upper - lower; k++) {
				buffer[i * ((size_t)NUMBER_TIME_STEPS - 1) * (upper - lower + 1) + j * (upper - lower + 1) + k] = sub_matrices[i][j][k];
			}
		}
	}

	MPI_Send(buffer, count, MPI_DOUBLE, 0, rank, world);
}

void MPImanager::add_sub_matrix(size_t i, double ** sub_matrix) {
	sub_matrices[i] = sub_matrix;
}