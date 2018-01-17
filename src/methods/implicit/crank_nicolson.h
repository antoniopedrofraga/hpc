#ifndef CRANK_NICOLSON_H  //include guard
#define CRANK_NICOLSON_H

#include "implicit.h" // declare that the Implicit class exists (inheritance)

/**
*  A CrankNicolson method class that contains a r vector builder.
*  \n This builder is used to calculate the r vector in A.x = r linear equation system.
*
* The CrankNicolson class provides:
* \n-a basic constructor for creating a CrankNicolson method object.
* \n-a method to compute the r vector.
*/
class CrankNicolson: public Implicit {
protected:
	// PROTECTED METHODS

	/**
	* Normal protected method.
	* get the number of rows
	* @param previous_step Vector representing the solution of the previous time step.
	* @return Vector. r vector to be used in A.x = r
	*/
	Vector build_r(Vector previous_step);
public:
	// CONSTRUCTORS

	/**
	* Default constructor.
	*/
	CrankNicolson(Problem problem);
};

#endif