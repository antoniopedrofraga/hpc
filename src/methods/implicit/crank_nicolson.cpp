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
Vector CrankNicolson::build_r(Vector previous_step) {
	size_t size = previous_step.getSize() - 2, j = 0;
	Vector r(size);
	for (size_t i = 0; i < size; i++) {
		j = i + 1;
		r[i] = (i == 0 || i == size - 1) ? previous_step[j] + q * (SURFACE_TEMPERATURE + previous_step[j - 1] - 2.0 * previous_step[j] + previous_step[j + 1])
		: previous_step[j] + q * (previous_step[j - 1] - 2.0 * previous_step[j] + previous_step[j + 1]);
	}
	return r;
}

