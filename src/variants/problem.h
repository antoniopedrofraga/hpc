#ifndef PROBLEM_H  //include guard
#define PROBLEM_H

#include "../variants/utils.h" //relevant utils
#include "../grid/matrix.h" // declare the structure of the matrix object

/**
*  A Problem class to structure relevant information related with the problem
*
* The Problem class provides:
* \n-basic constructors for creating a Problem object.
* \n-acessor methods to retrieve valuable information
* \n-mutator methods to change the solution system
*/
class Problem {
private:
	double delta_x; /**< Private double delta_x. Space step of the solution. */
	double delta_t; /**< Private double delta_t. Time step of the solution. */

	unsigned int x_size; /**< Private unsigned int x_size. Space size of the solution. */
	unsigned int t_size; /**< Private unsigned int t_size. Time size of the solution. */

	Vector x_values; /**< Private Vector x_values. Space correspondent value for each column index. */
	Vector t_values; /**< Private Vector t_values. Time correspondent value for each row index. */

	Matrix solution; /**< Private Matrix solution. Matrix containing the computed solution. */

	// PRIVATE MUTATOR METHODS

	/**
	* Normal private set method.
	* Intialize Vector x_values with the correct values.
	* @see x_values
	*/
	void set_x_values();

	/**
	* Normal private set method.
	* Intialize Vector t_values with the correct values.
	* @see t_values
	*/
	void set_t_values();
	
public:
	// CONSTRUCTORS
	/**
	* Default constructor.  Intialize an empty Problem object
	* @see Problem(double dt, double dx)
	*/
	Problem();

	/**
	* Intialize Problem object with specific time and space steps
	* @see Problem()
	* @param dt Time step to assign
	* @param dx Space step to assign
	* @exception out_of_range ("space step can't be negative or zero")
	* @exception out_of_range ("time step can't be negative or zero")
	*/
	Problem(double dt, double dx);

	// PUBLIC ACCESSOR METHODS
	/** Normal public get method that returns an unsigned int, the number of columns of the solution 
	* @return unsigned int. The number of columns of the solution.
	*/
	unsigned int get_xsize();

	/** Normal public get method that returns an unsigned int, the number of rows of the solution 
	* @return unsigned int. The number of rows of the solution.
	*/
	unsigned int get_tsize();

	/** Normal public get method that returns a double, the space step value of the solution 
	* @return double. The space step value of the solution.
	*/
	double get_deltax();

	/** Normal public get method that returns a double, the time step value of the solution 
	* @return double. The time step value of the solution.
	*/
	double get_deltat();

	/** Normal public get method that returns a Vector, containing the space values in each column 
	* @return Vector. The space values in each column.
	*/
	Vector get_xvalues();

	/** Normal public get method that returns a Vector, containing the time values in each row 
	* @return Vector. The time values in each row.
	*/
	Vector get_tvalues();

	/** Normal public get method that returns a Vector, containing the initial boundaries in the first row of the solution 
	* @return Vector. The initial boundaries in the first row of the solution.
	*/
	Vector get_first_row();

	/** Normal public get method that returns a Matrix, containing the solution solution. 
	* @return Matrix*. The solution solution.
	*/
	Matrix *get_solution();

	// PUBLIC MUTATOR METHODS

	/**
	* Normal public set method.
	* replace a row of the solution for a given Vector.
	* @param step Vector conatining the new values.
	* @param time Corresponding row to be replaced
	*/
	void set_time_step(Vector step, double time);

	/**
	* Normal public set method.
	* set the problem initial boundaries.
	*/
	void set_initial_conditions();
};

#endif