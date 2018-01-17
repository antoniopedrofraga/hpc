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
void Explicit::compute_solution() {
	FTCS ftcs(problem);
	Vector current_step, previous_step, next_step;
	unsigned int t_size = problem.get_tsize(), x_size = problem.get_xsize();
	double delta_t = problem.get_deltat(), time;
	current_step = next_step = Vector(x_size + 1);
	// iterate through the several time steps
	for (size_t i = 1; i <= t_size; i++) {
		// if is the first iteration then the previous step is known (initial conditions)
		// and the current-step may be obtained with the forward in time and central in space method, which only requires the previous step to calculate the current time step solution
		if (i == 1) {
			previous_step = problem.get_first_row();
			current_step = ftcs.build_iteration(Vector(0), previous_step);
		}

		// use the current and previous time steps to calculate the next time step solution
		next_step = build_iteration(current_step, previous_step);
		previous_step = current_step;
		current_step = next_step;
		time = delta_t * (double)i;
		// save solution if time step == 0.1, 0.2, 0.3, 0.4 or 0.5
		problem.set_time_step(current_step, time);
	}
}