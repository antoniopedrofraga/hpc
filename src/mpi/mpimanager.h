#ifndef MPI_MANAGER_H //include guard
#define MPI_MANAGER_H

#include <mpi.h>
#include "../variants/utils.h"
#include "../methods/method.h"
#include <vector>
using namespace std;

class MPImanager {
private:
	int rank;
	int number_processes;
	size_t size;
	double *** sub_matrices;
	
public:
	MPImanager(size_t size);
	void initialize(int *argc, char ** argv[]);
	void finalize();
	int get_rank();
	int get_number_processes();
	bool is_root();
	bool is_last();
	bool one_process();
	size_t lower_bound();
	size_t upper_bound();
	void add_sub_matrix(size_t i, double ** sub_matrix);

	void collect_results(vector<Method*> &solutions);
	void send_results();
};

#endif