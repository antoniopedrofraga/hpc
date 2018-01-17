#include "implicit.h"

// CONSTRUCTORS
/*=
 *Default constructor, method to solve a problem with implicit procedures
 */
Implicit::Implicit(Problem problem) : Method(problem) {
	double delta_t = problem.get_deltat(), delta_x = problem.get_deltax();
	q = delta_t * DIFUSIVITY / pow(delta_x, 2);
}

// METHODS

/*
* public normal method - compute a solution using implicit procedures
*/
void Implicit::compute_solution() {
	Vector previous_step, current_step, r, t_values = problem.get_tvalues();
	unsigned int t_size = problem.get_tsize();
	double delta_t = problem.get_deltat(), time;

	// iterate through the several time steps
	for (size_t i = 1; i <= t_size; i++) {
		// if is the first iteration then the previous step is known (initial conditions)
		if (i == 1) { previous_step = problem.get_first_row(); }
		// build r vector
		r = build_r(previous_step);
		// use the r vector to calculate the current time step solution with the thomas algorithm
		current_step = thomas_algorithm(r, -q, (1.0 + 2.0 * q), -q);
		current_step.push_front_back(SURFACE_TEMPERATURE);
		previous_step = current_step;
		time = delta_t * (double)i;
		// save solution if time step == 0.1, 0.2, 0.3, 0.4 or 0.5
		problem.set_time_step(current_step, time);
	}
}

/*
* private normal method - thomas algorithm to compute a given time step solution
*/
Vector Implicit::thomas_algorithm(Vector r, double a, double b, double c) {
	unsigned int size = r.getSize();
	Vector p(size), y(size - 1), x(size);
	// building the y and p vectors (forward phase)
	for (size_t i = 0; i < size; i++) {
		if (i == 0) {
			y[i] = c / b;
			p[i] = r[i] / b;
		} else if (i == size - 1) {
			p[i] = (r[i] - a * p[i - 1]) / (b - a * y[i - 1]);
		} else {
			y[i] = c / (b - a * y[i - 1]);
			p[i] = (r[i] - a * p[i - 1]) / (b - a * y[i - 1]);
		}
	}
	// building the x vector in A.x = r linear equations system (backwards phase)
	x[size - 1] = p[size - 1];
	for (int i = size - 2; i >= 0; i--) {
		x[i] = p[i] - y[i] * x[i + 1];
	}
	return x;
}
