#ifndef RICHARDSON_H  //include guard
#define RICHARDSON_H

#include "explicit.h" // declare that the Explicit class exists (inheritance)

/**
*  A Richardson method class that contains an iteration builder.
*  \n This builder is used to calculate a solution using the Richardson method.
*
* The Richardson class provides:
* \n-a basic constructor for creating a Richardson method object.
* \n-a method to compute a solution of the current iteration
*/
class Richardson: public Explicit {
protected:
	// PROTECTED METHODS

	/**
	* Normal protected method.
	* Calculate a next time step solution requiring a previous time step and a current time step solution.
	* @param current_step A vector representing the current time step solution.
	* @param previous_step A vector representing the previous time step solution.
	* @return Vector. The computed solution.
	*/
	Vector build_iteration(Vector current_step, Vector previous_step);
public:
	// CONSTRUCTORS

	/**
	* Default constructor. 
	*/
	Richardson(Problem problem);
};

#endif