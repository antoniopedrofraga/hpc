#include "problem.h"

// CONSTRUCTORS
/*=
 *Default constructor
 */
Problem::Problem() {}


/*
 * Alternate constructor - creates a problem with a specific time and space step
 */

Problem::Problem(double dt, double dx) {
	//check the input
	if (dx <= 0) throw std::out_of_range("space step can't be negative or zero");
	if (dt <= 0) throw std::out_of_range("time step can't be negative or zero");

	// set time and space steps
	delta_x = dx;
	delta_t = dt;

	// set time and space size
	x_size = (int)(THICKNESS / delta_x);
	t_size = (int)(TIMELIMIT / delta_t);

	// initializes the solution and set space value in each column and time value in each row
	solution = Matrix(NUMBER_TIME_STEPS, x_size + 1);
	set_x_values();
	set_t_values();

	// set the initial boundaries
	set_initial_conditions();
}

//PRIVATE MUTATOR METHODS

/*
* private mutator method - set space value in each column
*/
void Problem::set_x_values() {
	x_values = Vector(x_size + 1);
	for (unsigned int index = 0; index <= x_size; index++) {
		x_values[index] = delta_x * index;
	}
}

/*
* private mutator method - set time value in each column
*/
void Problem::set_t_values() {
	t_values = Vector(NUMBER_TIME_STEPS);
	for (unsigned int time = 0; time < NUMBER_TIME_STEPS; time++) {
		t_values[time] = double(time) / 10.0;
	}
}

//PUBLIC MUTATOR METHODS

/*
* public mutator method - set initial boundaries
*/
void Problem::set_initial_conditions() {
	for (unsigned int index = 1; index < x_size; index++) {
		solution[0][index] = INITIAL_TEMPERATURE;
	}
	for (unsigned int time = 0; time < NUMBER_TIME_STEPS; time++) {
		solution[time][0] = solution[time][x_size] = SURFACE_TEMPERATURE;
	}
}

/*
* public mutator method - set solution row
*/
void Problem::set_time_step(Vector step, double time) {

	//checks if value is in vector
	int position = t_values.find(time);
	if (position != -1) {
		solution.set_row(position, step);
	}
}

void Problem::set_value(size_t i, size_t j, double value) {
	if (NUMBER_TIME_STEPS <= i) {
		std::cout << "i = " << i << " and size = " << NUMBER_TIME_STEPS << std::endl;
	} else if (x_size + 1 <= j) {
		std::cout << "j = " << j << " and size = " << x_size + 1 << std::endl;
	} else {
		solution[i][j] = value;
	}
}

//PUBLIC ACCESSOR METHODS

/*
* public accessor method - get first row of the solution
*/
Vector Problem::get_first_row() {
	return solution[0];
}

/*
* public accessor method - get number of columns of the solution
*/
unsigned int Problem::get_xsize() {
	return x_size;
}

/*
* public accessor method - get number of rows of the solution
*/
unsigned int Problem::get_tsize() {
	return t_size;
}

/*
* public accessor method - get space step
*/
double Problem::get_deltax() {
	return delta_x;
}

/*
* public accessor method - get time step
*/
double Problem::get_deltat() {
	return delta_t;
}

/*
* public accessor method - get Vector of corresponding space values for each column index
*/
Vector Problem::get_xvalues() {
	return x_values;
}

/*
* public accessor method - get Vector of corresponding time values for each row index
*/
Vector Problem::get_tvalues() {
	return t_values;
}

/*
* public accessor method - get solution solution
*/
Matrix *Problem::get_solution() {
	return &solution;
}

