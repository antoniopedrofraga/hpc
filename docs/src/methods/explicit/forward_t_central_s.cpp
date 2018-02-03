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
double* FTCS::build_iteration(MPImanager *mpi_manager, double* previous_step) {
	int rank = mpi_manager->get_rank();
	double * result = (double*) malloc((size + 1) * sizeof(double));
	for (size_t i = 0; i <= size; i++) {
		wait(mpi_manager, i);

		double back_space = i == 0 ? back : previous_step[i - 1], 
		forward_space = i >= size ? forward : previous_step[i + 1];
		result[i] = previous_step[i] + q / 2.0 * (forward_space - 2.0 * previous_step[i] + back_space);

		if (!last_iteration) exchange_data(mpi_manager, i, result);
	}

	return result;
}
