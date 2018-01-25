#ifndef METHOD_H  // include guard
#define METHOD_H

#include "../variants/problem.h" // declare the problem structure
#include <mpi.h>
#include <vector>

class MPImanager;

using namespace std;


/**
*  A Method class to structure information used to solve the problem
*
* The Method class provides:
* \n-basic constructors for creating a Method object.
* \n-acessor methods to retrieve valuable information
* \n-mutator methods to change the problem grid system
*/
class Method {
private:
	double one_norm;
	double two_norm;
	double uniform_norm;

	double computational_time; /**< Private double computational_time. Elapsed time throughout the solution computation. */
protected:
	Problem problem; /**< Protected Problem problem. Space step of the solution. */
	std::string name; /**< Protected string name. Name of the method. */
	double q; /**< Protected double q. A coeficient which value depends of way the equation is written, it may vary from method to method. */
	bool last_iteration;

	MPI_Request requests[4];
	bool request_status[4];
public:
	// CONSTRUCTORS

	/**
	* Default constructor.  Intialize a Method object
	* @see Method(Problem problem)
	*/
	Method();

	/**
	* Alternate constructor. Initializes a Method with a given parabolic problem.
	* @see Method()
	*/
	Method(Problem problem);


	// PUBLIC ACCESSOR METHODS

	/**
	* Normal public get method.
	* get the method name
	* @return string. Method name.
	*/
	std::string get_name();

	/**
	* Normal public get method.
	* get the solution grid
	* @return Matrix. Computed solution grid.
	*/
	Matrix get_solution();

	/**
	* Normal public get method.
	* get the time step of the solution
	* @return double. Solution time step.
	*/
	double get_deltat();

	/**
	* Normal public get method.
	* get x values vector
	* @return Vector. x values Vector.
	*/
	Vector get_xvalues();

	/**
	* Normal public get method.
	* get the elapsed time value to compute a solution
	* @return double. Elapsed time throughout the computation.
	*/
	double get_computational_time();

	/**
	* Normal public get method.
	* get the second norm
	* @return double. Second norm value.
	*/
	double get_two_norm();

	/**
	* Normal public method.
	* Keeps track of the time to compute a solution
	*/
	void compute(MPImanager *mpi_manager, MPI_Comm world, size_t index);

	void set_value(size_t i, size_t j, double value);

	void compute_norms(Matrix analytical_matrix);

	// PUBLIC MUTATOR METHODS

	/**
	* A pure virtual member.
	* compute the solution following the rules of a given method.
	*/
	virtual void compute_solution(MPImanager *mpi_manager, size_t index) = 0;


};

#endif