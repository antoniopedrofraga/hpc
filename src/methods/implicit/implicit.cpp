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

	if (mpi_manager->is_root() && !mpi_manager->one_process()) {
		calculate_v_w(mpi_manager);
	}

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
		current_step = thomas_algorithm(r, -q, (1.0 + 2.0 * q), -q);
		exchange_data(mpi_manager, current_step);

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
double * Implicit::thomas_algorithm(double * r, double a, double b, double c) {
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
	}
	return x;
}

void Implicit::calculate_v_w(MPImanager * mpi_manager) {
	double * v = (double*) malloc((size + 1) * sizeof(double)), double * w = (double*) malloc((size + 1) * sizeof(double));
	memset(&v[0], 0.0, (size + 1) * sizeof(double));
	memset(&w[0], 0.0, (size + 1) * sizeof(double));
	v[0] = -q;
	w[size] = -q;

	v = thomas_algorithm(v, -q, (1.0 + 2.0 * q), -q);
	w = thomas_algorithm(w, -q, (1.0 + 2.0 * q), -q);

	size_t y_size = mpi_manager->get_number_processes() * 2;
	double ** spike_matrix = (double *) malloc(sizeof(double) * y_size * y_size);
	memset(&spike_matrix[0][0], 0.0, sizeof(double) * y_size * y_size);

	for (size_t i = 0; i < y_size; i++) {
		spike_matrix[i][i] = 1.0;
		if (i != 0 && i != y_size - 1) {
			if (i % 2 == 0) {
				spike_matrix[i - 1][i] = v[size];
				spike_matrix[i - 2][i] = v[0]; 
			} else {
				spike_matrix[i + 2][i] = v[size];
				spike_matrix[i + 1][i] = v[0]; 
			}
		}
	}

	s = gaussian_elimination(spike_matrix, y_size);
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

void Implicit::exchange_data(MPImanager *mpi_manager, double * &d) {
	if (mpi_manager->one_process()) return;
	if (last_iteration) return;

	size_t y_size = mpi_manager->get_number_processes() * 2;
	double * y = (double *) malloc(sizeof(double) * y_size), g[2] = { d[0], d[size] };
	int rank = mpi_manager->get_rank();
	MPI_Gatherv(g, 2, MPI_DOUBLE, &y[0], y_size, 2, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	if (mpi_manager->is_root()) {
		double buffer[y_size];

		for (size_t i = 0; i < y_size; i++) {
			buffer[i] = s[]
		}
	}
}
