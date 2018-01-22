#ifndef IO_MANAGER_H  //Include guard
#define IO_MANAGER_H

#include "../libs/gnuplot-iostream.h" // lib to be able to use gnuplot from c++
#include "../methods/method.h" // provides knowledge about method objects structure
#include <vector>

/**
*  An input/output manager class to handle plot exportations and future implementations of input handling
*
* The IOManager class provides:
* \n-plot method which compares the analytical solution with a set of given methods, ploting them with a custom configuration using gnuplot
*/
class IOManager {
private:
	std::string output_path;  /**< Private string output_path. Contains the ouput directory path name. */
	std::vector<double> laasonen_times; 	/**< Private Vector laasonen_times. Contains the computation time of each laasonen solution, with a different time step. */
	std::vector<double> default_deltat_times;	/**< Private Vector default_deltat_times. Contains the computation time of each method solution, with a time step of 0.01. */

	// PRIVATE PLOT METHODS

	/**
	* Method to create ouput folder if the folder does not exist
	* @return bool. true if successfull, false if not
	*/
	bool create_output_dir();

	/**
	* Exports a plot chart that compares the analytical solution to any other solution using gnuplot
	* @param string output_name File name to be exported
	* @param Method* analytical The analytical solution
	* @param Method* method Any method solution
	*/
	void plot_solutions(std::string output_name, Method * analytical, Method * method);

	void plot_times(std::string output_name, Method * analytical, std::vector<Method*> methods);

	/**
	* Exports a plot that compares the norms of each solution
	* @param string output_name File name to be exported
	* @param vector<Method*> vector of methods to plot the second norm
	*/
	void error_tables(std::string output_name, std::vector<Method*> method);

	// AUX METHODS

	/**
	* Converts a double to a string with a precison of 2 decimal places
	* @param double value Number to be converted
	* @param int precision Precision to have
	* @return string. String containing the converted number
	*/
	std::string double_to_string(int precision, double value);
public:
	// CONSTRUCTORS

	/**
	* Default constructor.  Initialize an IOManager object.
	*/
	IOManager();

	// PUBLIC OUTPUTS METHODS

	/**
	* Exports outputs regarding plots images and error tables for each computed solution, comparing them to the analytical solution
	* @param Method* analytical The analytical solution
	* @param vector<Method*> methods Vector containing the solutions 
	*/
	void export_outputs(Method * analytical, std::vector<Method*> methods);


	void export_analytical(Method * analytical);
};

#endif