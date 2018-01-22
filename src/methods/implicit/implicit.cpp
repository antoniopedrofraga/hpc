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
	size_t t_size = problem.get_tsize();
	
	upper = mpi_manager->upper_bound(), lower = mpi_manager->lower_bound();
	size = upper - lower;
	back = mpi_manager->is_root() ? SURFACE_TEMPERATURE : INITIAL_TEMPERATURE, forward = mpi_manager->is_last() ? SURFACE_TEMPERATURE : INITIAL_TEMPERATURE;

	double delta_t = problem.get_deltat(), time;

	double * current_step = (double*) malloc((size + 1) * sizeof(double)), * previous_step = (double*) malloc((size + 1) * sizeof(double)), 
	*r = (double*) malloc((size + 1) * sizeof(double));

	double ** sub_matrices = alloc2d(NUMBER_TIME_STEPS - 1, size + 1);

	// iterate through the several time steps
	for (size_t i = 1; i <= t_size; i++) {
		// if is the first iteration then the previous step is known (initial conditions)
		if (i == 1) {
			for (size_t t = 0; t <= size; t++) {
				previous_step[t] = INITIAL_TEMPERATURE;
			}
		} else if (i == t_size) {
			last_iteration = true;
		}

		// build r vector
		r = build_r(mpi_manager, previous_step);

		// use the r vector to calculate the current time step solution with the thomas algorithm
		current_step = thomas_algorithm(mpi_manager, r, -q, (1.0 + 2.0 * q), -q);

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
double * Implicit::thomas_algorithm(MPImanager * mpi_manager, double * r, double a, double b, double c) {
	double * x = (double*) malloc((size + 1) * sizeof(double)), y[size], p[size + 1];

	// building the y and p vectors (forward phase)
	for (size_t i = 0; i <= size; i++) {
		if (i == 0) {
			y[i] = c / b;
			p[i] = r[i] / b;
		} else if (i == size) {
			p[i] = (r[i] - a * p[i - 1]) / (b - a * y[i - 1]);
		} else {
			y[i] = c / (b - a * y[i - 1]);
			p[i] = (r[i] - a * p[i - 1]) / (b - a * y[i - 1]);
		}
	}
	// building the x vector in A.x = r linear equations system (backwards phase)
	for (int i = size; i >= 0; i--) {
		x[i] = (size_t)i == size ? p[i] : p[i] - y[i] * x[i + 1];
		exchange_data(mpi_manager, i, x);
	}
	return x;
}

void Implicit::wait(MPImanager * mpi_manager, size_t i) {
	int rank = mpi_manager->get_rank();
	MPI_Status status;

	if (i == 0) {
		if (!mpi_manager->is_last() && request_status[2]) {
			MPI_Wait(&requests[2], &status);
		}
		if (!mpi_manager->is_root() && request_status[3]) {
			MPI_Wait(&requests[3], &status);
		}
	}

	if (i == size) {
		if (!mpi_manager->is_root() && request_status[0]) {
			MPI_Wait(&requests[0], &status);
		}
		if (!mpi_manager->is_last() && request_status[1]) {
			MPI_Wait(&requests[1], &status);
		}
	}

}

void Implicit::exchange_data(MPImanager *mpi_manager, size_t i, double * result) {
	if (last_iteration) return;

	int rank = mpi_manager->get_rank();

	if (i == 0) {
		if (!mpi_manager->is_root()) {
			MPI_Isend(&result[i], 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, &requests[0]);
			if (!request_status[0]) request_status[0] = true;
		}
		if (!mpi_manager->is_last()) {
			MPI_Irecv(&forward, 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, &requests[1]);
			if (!request_status[1]) request_status[1] = true;
		}
	}
	if (i == size) {
		if (!mpi_manager->is_last()) {
			MPI_Isend(&result[i], 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, &requests[2]);
			if (!request_status[2]) request_status[2] = true;
		}
		if (!mpi_manager->is_root()) {
			MPI_Irecv(&back, 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, &requests[3]);
			if (!request_status[3]) request_status[3] = true;
		}
	}
}
