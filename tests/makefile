all: ./*.cpp
	mpicxx -std=c++14 -lpthread -W ./*.cpp -o testing
install:
	sudo apt-get install libboost-iostreams-dev libboost-system-dev libboost-filesystem-dev
run:
	make all
	mpiexec -np 2 --mca orte_base_help_aggregate 0 ./testing
memcheck:
	make all
	mpiexec -np 4 --mca orte_base_help_aggregate 0 valgrind --tool=memcheck --leak-check=full ./bin/assignment
export:
	if [ -d ../outputs ]; then \
	  	make clean; \
	fi; \
	make all
	./bin/assignment
clean:
	\rm  -r ./outputs
generate-doxygen-config:
	if [ ! -d ./doxygen/ ]; then \
		sudo mkdir -p ./doxygen/; \
	fi; \
	sudo doxygen -g ./doxygen/doxygen.config
generate-doxygen:
	doxygen ./doxygen/doxygen.config