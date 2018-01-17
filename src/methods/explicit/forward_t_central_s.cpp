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
Vector FTCS::build_iteration(Vector current_step, Vector previous_step) {
	unsigned int size = previous_step.getSize();
	Vector result(size);
	result[0] = result[size - 1] = SURFACE_TEMPERATURE;
	for (unsigned int i = 1; i < size - 1; i++) {
		result[i] = previous_step[i] + q / 2.0 * (previous_step[i + 1] - 2.0 * previous_step[i] + previous_step[i - 1]);
	}
	return result;
}