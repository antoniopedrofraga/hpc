#include "explicit.h"
#include "forward_t_central_s.h" // method to use in the first iteration

// CONSTRUCTORS
/*=
 *Default constructor, method to solve a problem with explicit procedures
 */
Explicit::Explicit(Problem problem) : Method(problem) {
	double delta_t = problem.get_deltat(), delta_x = problem.get_deltax();
	q = (2 * delta_t * DIFUSIVITY) / pow(delta_x, 2);
}

// METHODS

/*
* public method - compute a solution using explicit procedures
*/
void Explicit::compute_solution(MPImanager *mpi_manager, size_t index) {
	size_t t_size = problem.get_tsize(), upper = mpi_manager->upper_bound(),
	lower = mpi_manager->lower_bound();
	size_t x_size = upper - lower;
	Vector t_values = problem.get_tvalues();

	double delta_t = problem.get_deltat(), time;
	double * current_step = (double*) malloc((x_size + 1) * sizeof(double)), * previous_step = (double*) malloc((x_size + 1) * sizeof(double)), 
	back = mpi_manager->is_root() ? SURFACE_TEMPERATURE : INITIAL_TEMPERATURE, forward = mpi_manager->is_last() ? SURFACE_TEMPERATURE : INITIAL_TEMPERATURE;

	double ** sub_matrices = alloc2d(NUMBER_TIME_STEPS - 1, x_size + 1);
	// iterate through the several time steps
	for (size_t i = 1; i <= t_size; i++) {
		// if is the first iteration then the previous step is known (initial conditions)
		if (i == 1) {
			for (size_t t = 0; t <= x_size; t++) {
				previous_step[t] = INITIAL_TEMPERATURE;
			}
		} else if (i == t_size) {
			last_iteration = true;
		}
		// use the current and previous time steps to calculate the next time step solution
		current_step = build_iteration(NULL, previous_step, mpi_manager, back, forward);
		previous_step = current_step;
		time = delta_t * (double)i;
		// save solution if time step == 0.1, 0.2, 0.3, 0.4 or 0.5
		int position = t_values.find(time);
		if (position != -1) {
			sub_matrices[position - 1] = current_step;
		}
	}


	mpi_manager->add_sub_matrix(index, sub_matrices);
}