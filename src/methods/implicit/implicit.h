#ifndef IMPLICIT_H  //include guard
#define IMPLICIT_H

#include "../method.h" // declare that the Method class exists (inheritance)


/**
*  An implicit method class that contains default methods that only implicit methods use
*  \n The implementation is derived from the Method class
*
* The Implicit class provides:
* \n-a basic constructor for creating an implicit method object.
* \n-a method to compute a solution following implicit methods rules 
*/
class Implicit: public Method {
private:
	// PRIVATE METHODS

	/**
	* Normal private method.
	* Calculates the current time step with Tomas Algorithm.
	* Giving the A.x = r, in which A is a matrix, whereas b and r are vectors, it calculates the b vector, since A and b are known variables.
	* @see build_r(Vector previous_step)
	* @param r Vector calculated by the build_r method.
	* @param a Lower diagonal value of the tridiagonal matrix
	* @param b Center diagonal value of the tridiagonal matrix
	* @param c Upper diagonal value of the tridiagonal matrix
	* @return Vector. Vector that represents the current time step solution.
	*/
	Vector thomas_algorithm(Vector r, double a, double b, double c);
protected:
	// PROTECTED METHODS

	/**
	* A pure virtual member.
	* Build the r vector in a linear system of A.x = r in which A is a matrix, whereas b and r are vectors.
	* \n This method is used to compute a solution using the thomas algorithm, which can be used in a triadiogonal matrix.
	* @param previous_step A vector containing the previous time step solution.
	* @return Vector. The r vector, which can be used in to calculate the current time step solution with Tomas Algorithm.
	*/
	virtual Vector build_r(Vector previous_step) = 0; 
public:
	// CONSTRUCTORS

	/**
	* Default constructor. 
	*/
	Implicit(Problem problem);

	// PUBLIC METHODS

	/**
	* Normal public method.
	* Calculates a solution for the given problem by populating the solution grid with the correct values.
	*/
	void compute_solution(size_t lower, size_t upper);
};

#endif