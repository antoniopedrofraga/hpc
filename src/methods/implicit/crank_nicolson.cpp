#include "crank_nicolson.h"

// CONSTRUCTORS
/*=
 *Default constructor, with a given problem.
 */
CrankNicolson::CrankNicolson(Problem problem) : Implicit(problem) {
	name = CRANK_NICHOLSON;
	// q = delta_t * DIFUSIVITY / (pow(delta_x, 2) * 2) so we multiply it by 0.5
	q *= 0.5;
}
// PROTECTED METHODS

/*
* protected method - build the r vector in A.x = r
*/
double * CrankNicolson::build_r(MPImanager * mpi_manager, double * previous_step) {
	
	double * r = (double*) malloc((size + 1) * sizeof(double));
	for (size_t i = 0; i <= size; i++) {
		if (i == 0 && mpi_manager->is_root()) {
			r[i] = previous_step[i] + q * (2 * SURFACE_TEMPERATURE - 2.0 * previous_step[i] + previous_step[i + 1]);
		} else if (i == size && mpi_manager->is_last()) {
			r[i] = previous_step[i] + q * (2 * SURFACE_TEMPERATURE + previous_step[i - 1] - 2.0 * previous_step[i]);
		} else if (i == 0) {
			r[i] = previous_step[i] + q * (back - 2.0 * previous_step[i] + previous_step[i + 1]);
		} else if (i == size) {
			r[i] = previous_step[i] + q * (previous_step[i - 1] - 2.0 * previous_step[i] + forward);
		} else {
			r[i] = previous_step[i] + q * (previous_step[i - 1] - 2.0 * previous_step[i] + previous_step[i + 1]);
		}
	}
	return r;
}

