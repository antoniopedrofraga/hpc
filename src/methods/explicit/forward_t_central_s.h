#ifndef FORWARD_TIME_CENTRAL_SPACE_H  //include guard
#define FORWARD_TIME_CENTRAL_SPACE_H

#include "explicit.h" // declare that the Explicit class exists (inheritance)

/**
*  A FTCS method class that contains an iteration builder.
*  \n This builder is used to calculate the first iteration of explicit methods, since it only requires the previous step solution to do it.
*
* The FTCS class provides:
* \n-a basic constructor for creating a FTCS method object.
* \n-a method to compute the current iteration
*/
class FTCS: public Explicit {
public:
	// CONSTRUCTORS

	/**
	* Default constructor. 
	*/
	FTCS(Problem problem);

	// PUBLIC METHODS

	/**
	* Normal public method.
	* Calculate a solution requiring only the previous time step solution.
	* @param current_step A vector with size 0, it's not required in this method.
	* @param previous_step A vector representing the previous time step solution
	* @return Vector. The computed solution.
	*/
	double* build_iteration(MPImanager *mpi_manager, double* previous_step);
};

#endif