#ifndef EXPLICIT_H  //include guard
#define EXPLICIT_H

#include "../method.h" // declare that the Method class exists (inheritance)
#include "../../mpi/mpimanager.h"

/**
*  An explicit method class that contains default methods that only explicit methods use
*  \n The implementation is derived from the Method class
*
* The Explicit class provides:
* \n-a basic constructor for creating an explicit method object.
* \n-a method to compute a solution following explicit methods rules 
*/
class Explicit: public Method {
protected:
	// PROTECTED METHODS

	/**
	* A pure virtual member.
	* Build the solution of the next time step, using the previous time step and the next time step solutions
	* @param previous_step A vector containing the previous time step solution.
	* @param current_step A vector containing the current time step solution.
	* @return Vector. A vector representing the next time step solution.
	*/
	virtual double* build_iteration(double* current_step, double* previous_step, MPImanager *mpi_manager) = 0;
public:
	// CONSTRUCTORS

	/**
	* Default constructor. 
	*/
	Explicit(Problem problem);
	// PUBLIC METHODS

	/**
	* Normal public method.
	* Calculates a solution for the given problem by populating the solution grid with the correct values.
	*/
	void compute_solution(MPImanager *mpi_manager, size_t index);
};

#endif