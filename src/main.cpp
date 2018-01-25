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

	MPImanager * mpi_manager = new MPImanager();
	mpi_manager->initialize(&argc, &argv);

	for (size_t i = 0; i < 2; i++) {

		IOManager io_manager;
		Problem * default_problem = new Problem(DELTA_T[i], DELTA_X[i]);
		mpi_manager->set_size(default_problem->get_xsize() - 1);

		if (mpi_manager->part_of_world()) {

			size_t lower = mpi_manager->lower_bound(), upper = mpi_manager->upper_bound();

			Analytical * analytical = new Analytical(*default_problem);
			FTCS * ftcs = new FTCS(*default_problem);
			Laasonen * laasonen = new Laasonen(*default_problem);
			CrankNicolson * crank_nicolson = new CrankNicolson(*default_problem);

			std::vector<Method*> solutions = {analytical, laasonen, crank_nicolson, ftcs};

			for (size_t index = 0; index < solutions.size(); index++) {
				solutions[index]->compute(mpi_manager, mpi_manager->get_world(), index);
			}

			if (mpi_manager->is_root()) {
				std::cout << "Collecting results." << std::endl;
				mpi_manager->collect_results(solutions);
				io_manager.export_analytical(solutions[0]);
				std::vector<Method*> methods(solutions.begin() + 1, solutions.end());
				io_manager.export_outputs(solutions[0], methods, mpi_manager->get_number_processes());
			} else {
				std::cout << "Sending results." << std::endl;
				mpi_manager->send_results();
			}
		}

		MPI_Barrier(MPI_COMM_WORLD);
	}

	mpi_manager->finalize();

	return 0;
}