#include "analytical.h"


// CONSTRUCTORS
/*=
 *Default constructor, creates an object to compute an analytical solution for the given parabolic problem.
 */
Analytical::Analytical(Problem problem)
: Method(problem) {
	name = ANALYTICAL;
	this->nr_of_expansions = NUMBER_OF_EXPANSIONS;
}

/*
 * Computes the analytical solution 
 */
void Analytical::compute_solution(MPImanager *mpi_manager, size_t index) {
	Vector t_values = problem.get_tvalues();
	Vector x_values = problem.get_xvalues();
	size_t lower = mpi_manager->lower_bound(), upper = mpi_manager->upper_bound();

	double** sub_matrix = alloc2d(NUMBER_TIME_STEPS - 1, upper - lower + 1);
	lower++; upper++;
	std::cout << " : Lower(" << lower << ") Upper(" << upper << ")" << std::endl;

	// iterates through the solution columns
	for (unsigned int t = 1; t < NUMBER_TIME_STEPS; t++) {
		// iterates through the solution rows
		double * current_step = (double*) malloc(((upper - lower) + 1) * sizeof(double));
		for (unsigned int x = lower; x <= upper; x++) {
			double sum = 0.0;
			// expansions
			for (unsigned int m = 1; m <= this->nr_of_expansions; m++) {
				double m_double = (double)m;
				sum += exp(-DIFUSIVITY * pow(m_double * PI / THICKNESS, 2) * t_values[t]) * (1 - pow(-1, m_double)) / (m_double * PI) * sin(m_double * PI * x_values[x] / THICKNESS);
			}
			// assigns the correct value to a position 
			current_step[x - lower] = SURFACE_TEMPERATURE + 2.0 * (INITIAL_TEMPERATURE - SURFACE_TEMPERATURE) * sum;
		}
		sub_matrix[t - 1] = current_step;
	}

	mpi_manager->add_sub_matrix(index, sub_matrix);
}

