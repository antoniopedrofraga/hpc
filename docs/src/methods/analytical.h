#ifndef ANALYTICAL_H //include guard
#define ANALYTICAL_H

#include "method.h" // inheritance
#include "../mpi/mpimanager.h"

/**
*  An Analytical class to compute the solution with standard procedures
*  \n The implementation is derived from the Method Object
*
* The Analytical class provides:
* \n-a basic constructor for an object,
* \n-a method to compute a solution with the correct procedures
*/
class Analytical: public Method {
	unsigned int nr_of_expansions; /**< Private unsigned int nr_of_expansions. Limit of expansions to do in the sum used to compute the solution. */
public:
	// CONSTRUCTORS

	/**
	* Default constructor.  Intialize a Analytical object
	*/
	Analytical(Problem problem);

	// MUTATOR METHODS

	/**
	* Normal public method.
	* compute the solution with specific given rules
	*/
	void compute_solution(MPImanager *mpi_manager, size_t index);
};

#endif