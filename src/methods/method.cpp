#include "method.h"

// CONSTRUCTORS
/*=
 *Default constructor
 */
Method::Method() {}

/*
 * Alternate constructor - creates a method with a given problem
 */
Method::Method(Problem problem) {
	this->problem = problem;
	this->last_iteration = false;
	this->lapacke = false;

	for (size_t i = 0; i < 4; i++)
		this->request_status[i] = false;
}

/*
 * public method - compute a solution keeping track of spent time
 */
void Method::compute(MPImanager *mpi_manager, MPI_Comm world, size_t index) {
	double start = 0.0, end = 0.0;
	
	MPI_Barrier(world);
	start = MPI_Wtime();

	compute_solution(mpi_manager, index);

	MPI_Barrier(world);
	end = MPI_Wtime();

	computational_time = double(end - start);
}

/*
 * public method - compute norms of the error matrix;
 */
void Method::compute_norms(Matrix analytical_matrix) {
	Matrix method_matrix = get_solution();
	unsigned int rows = method_matrix.getNrows(), cols = method_matrix.getNcols();
	Matrix error_matrix(rows, cols);

	for (unsigned int i = 0; i < rows; i++) {
		for (unsigned int j = 0; j < cols; j++) {
			error_matrix[i][j] = analytical_matrix[i][j] - method_matrix[i][j];
		}
	}

	one_norm = error_matrix.one_norm();
	two_norm = error_matrix.two_norm();
	uniform_norm = error_matrix.uniform_norm();
}

// PUBLIC ACCESSOR METHODS

/*
 * public accessor method - get the method name
 */
std::string Method::get_name() {
	return name;
}

/*
 * public accessor method - get the two norm value
 */
double Method::get_two_norm() {
	return two_norm;
}

/*
 * public accessor method - get the solution grid
 */
Matrix Method::get_solution() {
	return (*problem.get_solution());
}

/*
 * public accessor method - get the solution time step
 */
double Method::get_deltat() {
	return problem.get_deltat();
}

/*
 * public accessor method - get x values vector
 */
Vector Method::get_xvalues() {
	return problem.get_xvalues();
}

/*
 * public accessor method - get computational time
 */
double Method::get_computational_time() {
	return computational_time;
}

void Method::set_value(size_t i, size_t j, double value) {
	problem.set_value(i, j, value);
}

void Method::enable_lapacke() {
	this->lapacke = true;
}


bool Method::is_lapacke() {
	return lapacke;
}

