#include <iostream>
#include "methods/analytical.h"
#include "methods/explicit/forward_t_central_s.h"
#include "methods/implicit/laasonen.h"
#include "methods/implicit/crank_nicolson.h"
#include "io/iomanager.h"

/*
	Main file - creates a problem, solving it with the different methods, ending by exporting the results. 
*/
int main() {
	IOManager io_manager;
	Problem default_problem(DELTA_T, DELTA_X);
	Analytical * analytical = new Analytical(default_problem);
	FTCS * ftcs = new FTCS(default_problem);
	Laasonen * laasonen = new Laasonen(default_problem);
	CrankNicolson * crank_nicolson = new CrankNicolson(default_problem);

	std::vector<Method*> solutions = {analytical, ftcs, laasonen, crank_nicolson};

	for (size_t index = 0; index < solutions.size(); index++) {
		solutions[index]->compute();
		if (solutions[index]->get_name() != ANALYTICAL) {
			solutions[index]->compute_norms(analytical->get_solution());
		}
	}

	std::vector<Method*> methods(solutions.begin() + 1, solutions.end());
	io_manager.export_outputs(solutions[0], methods);

	return 0;
}