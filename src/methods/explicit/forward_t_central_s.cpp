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
double* FTCS::build_iteration(double* current_step, double* previous_step, MPImanager *mpi_manager) {
	size_t upper = mpi_manager->upper_bound(), lower = mpi_manager->lower_bound(), size = upper - lower + 1, upper_limit = problem.get_xsize() - 2;
	int rank = mpi_manager->get_rank();
	double * result = new double[size], back_space = -1.0, forward_space = -1.0;

	if (!mpi_manager->is_root()) {
		std::cout << rank << " sending to " << rank - 1 << std::endl;
		MPI_Send(&previous_step[0], 1, MPI_DOUBLE, rank, rank - 1, MPI_COMM_WORLD);
	}
	if (!mpi_manager->is_last()) {
		std::cout << rank << " sending to " << rank + 1 << std::endl;
		MPI_Send(&previous_step[size - 1], 1, MPI_DOUBLE, rank, rank + 1, MPI_COMM_WORLD);
	}

	if (lower == 0) {
		back_space = SURFACE_TEMPERATURE;
	} else {
		std::cout << rank << " receiving from " << rank - 1 << std::endl;
		MPI_Recv(&back_space, 1, MPI_DOUBLE, rank - 1, rank - 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}

	if (upper == upper_limit) {
		forward_space = SURFACE_TEMPERATURE;
	} else {
		std::cout << rank << " receiving from " << rank + 1 << std::endl;
		MPI_Recv(&forward_space, 1, MPI_DOUBLE, rank + 1, rank + 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}


	for (size_t i = 0; i <= size; i++) {
		double back = i == 0 ? previous_step[i - 1] : back_space, forward = i + 1 > size ? forward_space : previous_step[i + 1];
		result[i] = previous_step[i] + q / 2.0 * (forward - 2.0 * previous_step[i] + back);
	}
	return result;
}