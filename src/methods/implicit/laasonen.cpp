#include "laasonen.h"

// CONSTRUCTORS
/*=
 *Default constructor, with a given problem.
 */
Laasonen::Laasonen(Problem problem) : Implicit(problem) {
	name = LAASONEN;
}

// PROTECTED METHODS

/*
* protected method - build the r vector in A.x = r
*/
double * Laasonen::build_r(MPImanager * mpi_manager, double * previous_step, double &back, double &forward) {
	size_t upper = mpi_manager->upper_bound(), lower = mpi_manager->lower_bound();
	size_t size = upper - lower, j = 0;

	double * r = (double*) malloc((size + 1) * sizeof(double));
	for (size_t i = 0; i <= size; i++) {
		if (i == 0) {
			r[i] = q * back + previous_step[i];
		} else if (i == size) {
			r[i] = q * forward + previous_step[i];
		} else {
			r[i] = previous_step[i];
		}
	}
	return r;
}