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
	size_t t_size = problem.get_tsize(), x_size = problem.get_xsize(),
	upper = mpi_manager->upper_bound(), lower = mpi_manager->lower_bound();
	Vector t_values = problem.get_tvalues();

	double delta_t = problem.get_deltat(), time;
	double * current_step = new double[x_size + 1], * previous_step = new double[x_size + 1], 
	back = mpi_manager->is_root() ? SURFACE_TEMPERATURE : INITIAL_TEMPERATURE, forward = mpi_manager->is_last() ? SURFACE_TEMPERATURE : INITIAL_TEMPERATURE;

	double ** sub_matrices = alloc2d(NUMBER_TIME_STEPS - 1, upper - lower + 1);
	// iterate through the several time steps
	for (size_t i = 1; i <= t_size; i++) {
		// if is the first iteration then the previous step is known (initial conditions)
		if (i == 1) { 
			memset((double*)previous_step, INITIAL_TEMPERATURE, sizeof(previous_step)); 
		}
		// use the current and previous time steps to calculate the next time step solution
		current_step = build_iteration(NULL, previous_step, mpi_manager, back, forward);
		previous_step = current_step;
		time = delta_t * (double)i;
		// save solution if time step == 0.1, 0.2, 0.3, 0.4 or 0.5
		vector<double>::iterator it = find(t_values.begin(), t_values.end(), time);
		if (it != t_values.end()) {
			size_t pos = it - t_values.begin();
			sub_matrices[pos - 1] = current_step;
		}
		//problem.set_time_step(current_step, time);

		//cout << "Rank " << mpi_manager->get_rank() << " computing " << i << endl;
	}

	mpi_manager->add_sub_matrix(index, sub_matrices);
}