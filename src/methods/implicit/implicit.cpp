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

	double delta_t = problem.get_deltat(), time;
	double * current_step = (double*) malloc((size + 1) * sizeof(double)), * previous_step = (double*) malloc((size + 1) * sizeof(double)), 
	*r, *x, *y;

	back = mpi_manager->is_root() ? SURFACE_TEMPERATURE : INITIAL_TEMPERATURE, forward = mpi_manager->is_last() ? SURFACE_TEMPERATURE : INITIAL_TEMPERATURE;
	v = (double*) malloc((size + 1) * sizeof(double)), w = (double*) malloc((size + 1) * sizeof(double));
	std::fill_n(&v[0], size + 1, 0.0);
	std::fill_n(&w[0], size + 1, 0.0);

	v[size] = -q;
	w[0] = -q;

	v = thomas_algorithm(v, -q, (1.0 + 2.0 * q), -q);
	w = thomas_algorithm(w, -q, (1.0 + 2.0 * q), -q);

	double ** sub_matrices = alloc2d(NUMBER_TIME_STEPS - 1, size + 1);

	if (mpi_manager->is_root() && !mpi_manager->one_process()) {
		calculate_spikes(mpi_manager);
	}

	// iterate through the several time steps
	for (size_t i = 1; i <= t_size; i++) {
		// if is the first iteration then the previous step is known (initial conditions)
		if (i == 1) {
			for (size_t t = 0; t <= size; t++) {
				previous_step[t] = INITIAL_TEMPERATURE;
			}
		}

		// build r vector
		r = build_r(mpi_manager, previous_step);

		// use the r vector to calculate the current time step solution with the thomas algorithm
		y = thomas_algorithm(r, -q, (1.0 + 2.0 * q), -q);

		x = exchange_data(mpi_manager, y[0], y[size]);

		if (x != NULL) {
			current_step = spikes_algorithm(mpi_manager, y, x);
		} else {
			current_step = y;
		}

		previous_step = current_step;
		time = delta_t * (double)i;
		// save solution if time step == 0.1, 0.2, 0.3, 0.4 or 0.5
		int position = t_values.find(time);
		if (position != -1) {
			sub_matrices[position - 1] = current_step;
		}

	}

	mpi_manager->add_sub_matrix(index, sub_matrices);

	free(r); free(x); free(y);
	free(v); free(w);
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

void Implicit::calculate_spikes(MPImanager * mpi_manager) {
	size_t y_size = mpi_manager->get_number_processes() * 2, npes = mpi_manager->get_number_processes();
	receive_count = (int*) malloc(npes * sizeof(int));
	receive_pos = (int*) malloc(npes * sizeof(int));

	std::fill_n(&receive_count[0], npes, 2);

	for (size_t i = 0; i < npes; i++) {
		receive_pos[i] = i * 2;
	}


	spike_matrix = alloc2d(y_size, y_size);
	std::fill_n(&spike_matrix[0][0], y_size * y_size, 0.0);

	for (size_t i = 0; i < y_size; i++) {
		spike_matrix[i][i] = 1.0;
		if (i != 0 && i != y_size - 1) {
			if (i % 2 == 0) {
				spike_matrix[i - 2][i] = v[0]; 
				spike_matrix[i - 1][i] = v[size];
			} else {
				spike_matrix[i + 2][i] = w[size];
				spike_matrix[i + 1][i] = w[0];
			}
		}
	}

}

double * Implicit::spikes_algorithm(MPImanager *mpi_manager, double * y, double * x) {
	double * current_step = (double *) malloc(sizeof(double) * (size + 1));
	size_t y_size = mpi_manager->get_number_processes() * 2;
	size_t s_index = mpi_manager->get_rank() * 2;

	for (size_t i = 0; i <= size; i++) {
		if (mpi_manager->is_root()) {
			current_step[i] = y[i] - v[i] * x[2];
		} else if (mpi_manager->is_last()) {
			current_step[i] = y[i] - w[i] * x[y_size - 3];
		} else {
			current_step[i] = y[i] - v[i] * x[s_index + 2] - w[i] * x[s_index - 1];
		}
	}
	return current_step;
}


double * Implicit::exchange_data(MPImanager *mpi_manager, double &head, double &tail) {
	if (mpi_manager->one_process()) return NULL;

	MPI_Comm world = mpi_manager->get_world();
	size_t y_size = mpi_manager->get_number_processes() * 2;
	double * y = (double *) malloc(sizeof(double) * y_size), g[2] = { head, tail }, *x = (double *) malloc(sizeof(double) * y_size);

	MPI_Gatherv(g, 2, MPI_DOUBLE, &y[0], receive_count, receive_pos, MPI_DOUBLE, 0, world);

	if (mpi_manager->is_root()) {
		x = gaussian_elimination(spike_matrix, y, y_size);
	}

	MPI_Bcast(&x[0], y_size, MPI_DOUBLE, 0, world);

	size_t index = mpi_manager->get_rank() * 2;
	if (!mpi_manager->is_root()) {
		back = x[index - 1];
	}
	if (!mpi_manager->is_last()) {
		forward = x[index + 2];
	}
	return x;
}
