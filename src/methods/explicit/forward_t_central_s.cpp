#include "forward_t_central_s.h"

// CONSTRUCTORS
/*=
 *Default constructor, method to solve a the first iteration of explicit methods.
 */
FTCS::FTCS(Problem problem)
: Explicit(problem) {
	name = FORWARD_TIME_CENTRAL_SPACE;
}

/*
* Normal public method - compute the first iteration of explicit methods
*/
double* FTCS::build_iteration(MPImanager *mpi_manager, double* previous_step, double &back, double &forward) {
	size_t upper = mpi_manager->upper_bound(), lower = mpi_manager->lower_bound();
	size_t size = upper - lower;
	int rank = mpi_manager->get_rank();
	double * result = (double*) malloc((size + 1) * sizeof(double));
	for (size_t i = 0; i <= size; i++) {
		wait(mpi_manager, i);

		double back_space = i == 0 ? back : previous_step[i - 1], 
		forward_space = i >= size ? forward : previous_step[i + 1];
		result[i] = previous_step[i] + q / 2.0 * (forward_space - 2.0 * previous_step[i] + back_space);

		if (!last_iteration) exchange_data(mpi_manager, i, result, back, forward);
	}

	return result;
}

void FTCS::wait(MPImanager * mpi_manager, size_t i) {
	size_t upper = mpi_manager->upper_bound(), lower = mpi_manager->lower_bound();
	size_t size = upper - lower;
	int rank = mpi_manager->get_rank();
	MPI_Status status;

	if (i == 0) {
		if (!mpi_manager->is_root() && request_status[0]) {
			MPI_Wait(&requests[0], &status);
		}
		if (!mpi_manager->is_last() && request_status[1]) {
			MPI_Wait(&requests[1], &status);
		}
	}

	if (i == size) {
		if (!mpi_manager->is_last() && request_status[2]) {
			MPI_Wait(&requests[2], &status);
		}
		if (!mpi_manager->is_root() && request_status[3]) {
			MPI_Wait(&requests[3], &status);
		}
	}
}

void FTCS::exchange_data(MPImanager *mpi_manager, size_t i, double * result, double &back, double &forward) {
	size_t upper = mpi_manager->upper_bound(), lower = mpi_manager->lower_bound();
	size_t size = upper - lower;
	int rank = mpi_manager->get_rank();

	if (i == 0) {
		if (!mpi_manager->is_root()) {
			MPI_Isend(&result[i], 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, &requests[0]);
			if (!request_status[0]) request_status[0] = true;
		}
		if (!mpi_manager->is_last()) {
			MPI_Irecv(&forward, 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, &requests[1]);
			if (!request_status[1]) request_status[1] = true;
		}
	}
	if (i == size) {
		if (!mpi_manager->is_last()) {
			MPI_Isend(&result[i], 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, &requests[2]);
			if (!request_status[2]) request_status[2] = true;
		}
		if (!mpi_manager->is_root()) {
			MPI_Irecv(&back, 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, &requests[3]);
			if (!request_status[3]) request_status[3] = true;
		}
	}
}