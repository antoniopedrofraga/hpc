#include <iostream>
#include "methods/analytical.h"
#include "methods/explicit/dufort_frankel.h"
#include "methods/explicit/richardson.h"
#include "methods/implicit/laasonen.h"
#include "methods/implicit/crank_nicolson.h"
#include "io/iomanager.h"

/*
	Main file - creates a problem, solving it with the different methods, ending by exporting the results. 
*/
int main() {
	IOManager io_manager;
	Problem default_problem(DELTA_T, DELTA_X);
	Analytical analytical(default_problem);
	DufortFrankel dufort_frankel(default_problem);
	Richardson richardson(default_problem);
	CrankNicolson crank_nicolson(default_problem);

	std::vector<Method*> solutions = {&analytical, &richardson, &dufort_frankel, &crank_nicolson};

	for (size_t index = 0; index < solutions.size(); index++) {
		solutions[index]->compute();
		if (solutions[index]->get_name() != ANALYTICAL) {
			solutions[index]->compute_norms(analytical.get_solution());
		}
	}

	std::vector<Method*> methods(solutions.begin() + 1, solutions.end());
	io_manager.export_outputs(solutions[0], methods);

	return 0;
}