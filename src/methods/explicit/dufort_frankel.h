#ifndef DUFORT_FRANKEL_H  //include guard
#define DUFORT_FRANKEL_H

#include "explicit.h" // declare that the Explicit class exists (inheritance)

/**
*  A DufortFrankel method class that contains an iteration builder.
*  \n This builder is used to calculate a solution using the Dufort-Frankel mathod.
*
* The DufortFrankel class provides:
* \n-a basic constructor for creating a DufortFrankel method object.
* \n-a method to compute a solution of the current iteration
*/
class DufortFrankel: public Explicit {
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
	DufortFrankel(Problem problem);
};

#endif