#ifndef UTILS_H  // include guard
#define UTILS_H

#include <cmath> // PI calculation
#include <string> // string usage
#include <vector>

const double DELTA_T[2] = { 0.1, 0.001 }; /**< Macro double. The default time step. */
const double DELTA_X[2] = { 0.5, 0.005 }; /**< Macro double. The default space step. */

const std::vector<double> DELTA_T_LASSONEN = {0.01, 0.025, 0.05, 0.1}; /**< Macro double. Time steps to study in Laasonen Implicit Scheme. */

const double DIFUSIVITY = 0.1; /**< Macro double. The default value of difusivity. */
const double THICKNESS = 1.0; /**< Macro double. The default value of thickness. */
const double TIMELIMIT = 0.5; /**< Macro double. The default value of time limit. */

const double SURFACE_TEMPERATURE = 300.0; /**< Macro double. The default surface temperature. */
const double INITIAL_TEMPERATURE = 100.0; /**< Macro double. The default initial temperature. */

const double NUMBER_TIME_STEPS = 6.0; /**< Macro double. The default limit of time steps. 0, 0.1, 0.2, 0.3, 0.4, 0.5 */
const unsigned int NUMBER_OF_EXPANSIONS = 20; /**< Macro unsigned int. Number of expansions to calculate the analytical solution sum expansion. */

const double PI = std::atan(1) * 4; /**< Macro double. Approximated value of PI. */

const std::string OUTPUT_PATH = "./outputs"; /**< Macro string. Default outputs path. */

const std::string ANALYTICAL = "Analytical"; /**< Macro string. Forward in Time and Central in Space method name. */
const std::string FORWARD_TIME_CENTRAL_SPACE = "Forward-Time-Central-Space"; /**< Macro string. Forward in Time and Central in Space method name. */
const std::string RICHARDSON = "Richardson"; /**< Macro string. Richardson method name. */
const std::string DUFORT_FRANKEL = "DuFort-Frankel"; /**< Macro string. DuFort-Frankel method name. */
const std::string LAASONEN = "Laasonen"; /**< Macro string. Laasonen method name. */
const std::string CRANK_NICHOLSON = "Crank-Nicholson"; /**< Macro string. Crank-Nicholson method name. */

const size_t SOLUTIONS_NR = 4;

double ***alloc3d(int l, int m, int n);
double **alloc2d(int rows, int cols);
double * gaussian_elimination(double ** &a, double * &b, size_t n);

#endif
