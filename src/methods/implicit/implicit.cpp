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
void Implicit::compute_solution(MPImanager *mpi_manager, size_t index) {
	Vector t_values = problem.get_tvalues();

	size_t t_size = problem.get_tsize(), upper = mpi_manager->upper_bound(),
	lower = mpi_manager->lower_bound();
	size_t x_size = upper - lower;

	double delta_t = problem.get_deltat(), time;

	double * current_step = (double*) malloc((x_size + 1) * sizeof(double)), * previous_step = (double*) malloc((x_size + 1) * sizeof(double)), 
	*r = (double*) malloc((x_size + 1) * sizeof(double)), 
	back = mpi_manager->is_root() ? SURFACE_TEMPERATURE : INITIAL_TEMPERATURE, forward = mpi_manager->is_last() ? SURFACE_TEMPERATURE : INITIAL_TEMPERATURE;

	double ** sub_matrices = alloc2d(NUMBER_TIME_STEPS - 1, x_size + 1);

	// iterate through the several time steps
	for (size_t i = 1; i <= t_size; i++) {
		// if is the first iteration then the previous step is known (initial conditions)
		if (i == 1) {
			for (size_t t = 0; t <= x_size; t++) {
				previous_step[t] = INITIAL_TEMPERATURE;
			}
		}
		// build r vector
		r = build_r(mpi_manager, previous_step, back, forward);

		// use the r vector to calculate the current time step solution with the thomas algorithm
		current_step = thomas_algorithm(r, -q, (1.0 + 2.0 * q), -q, x_size + 1);

		exchange_data(mpi_manager, current_step, back, forward);

		//current_step.push_front_back(SURFACE_TEMPERATURE);
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

/*
* private normal method - thomas algorithm to compute a given time step solution
*/
double * Implicit::thomas_algorithm(double * r, double a, double b, double c, size_t size) {
	double * x = (double*) malloc((size) * sizeof(double)), y[size - 1], p[size];

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


void Implicit::exchange_data(MPImanager * mpi_manager, double* &current_step, double &back, double& forward) {
	size_t upper = mpi_manager->upper_bound(), lower = mpi_manager->lower_bound();
	size_t size = upper - lower, j = 0;
	double value, last;
	int rank = mpi_manager->get_rank();

	if (!mpi_manager->is_root()) {
		MPI_Send(&current_step[0], 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD);
	}

	if (!mpi_manager->is_last()) {
		MPI_Recv(&forward, 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Send(&current_step[size], 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD);
	}

	if (!mpi_manager->is_root()) {
		MPI_Recv(&back, 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
}

