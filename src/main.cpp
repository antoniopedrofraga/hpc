#include <iostream>
#include "methods/analytical.h"
#include "methods/explicit/forward_t_central_s.h"
#include "methods/implicit/laasonen.h"
#include "methods/implicit/crank_nicolson.h"
#include "io/iomanager.h"
#include "mpi/mpimanager.h"

/*
	Main file - creates a problem, solving it with the different methods, ending by exporting the results. 
*/
int main(int argc, char * argv[]) {

	IOManager io_manager;
	Problem * default_problem = new Problem(DELTA_T, DELTA_X);
	MPImanager * mpi_manager = new MPImanager(default_problem->get_tsize());

	Analytical * analytical = new Analytical(*default_problem);
	FTCS * ftcs = new FTCS(*default_problem);
	/*Laasonen * laasonen = new Laasonen(*default_problem);
	CrankNicolson * crank_nicolson = new CrankNicolson(*default_problem);*/

	mpi_manager->initialize(&argc, &argv);
	size_t lower = mpi_manager->lower_bound(), upper = mpi_manager->upper_bound();

	std::vector<Method*> solutions = {analytical, ftcs/*, laasonen, crank_nicolson*/};

	for (size_t index = 0; index < solutions.size(); index++) {
		solutions[index]->compute(lower, upper);

		if (solutions[index]->get_name() != ANALYTICAL) {
			solutions[index]->compute_norms(analytical->get_solution());
		}
	}

	if (mpi_manager->is_root()) {
		mpi_manager->collect_results(solutions.size());
		std::vector<Method*> methods(solutions.begin() + 1, solutions.end());
		io_manager.export_outputs(solutions[0], methods);
	} else {
		mpi_manager->send_results();
	}

	mpi_manager->finalize();

	return 0;
}