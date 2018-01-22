#ifndef LAASONEN_H  //include guard
#define LAASONEN_H

#include "implicit.h" // declare that the Implicit class exists (inheritance)

/**
*  A Laasonen method class that contains a r vector builder.
*  \n This builder is used is used to calculate the r vector in A.x = r linear equation system.
*
* The Laasonen class provides:
* \n-a basic constructor for creating a Laasonen method object.
* \n-a method to compute the r vector.
*/
class Laasonen: public Implicit {
protected:
	// PROTECTED METHODS

	/**
	* Normal protected method.
	* get the number of rows
	* @param previous_step Vector representing the solution of the previous time step.
	* @return Vector. r vector to be used in A.x = r
	*/
	double * build_r(MPImanager * mpi_manager, double * previous_step, double &back, double &forward);
public:
	// CONSTRUCTORS

	/**
	* Default constructor.
	*/
	Laasonen(Problem problem);
};

#endif