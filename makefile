all: ./src/*.cpp
	mpicxx -std=c++11 -lstdc++ -lpthread -W ./src/*.cpp ./src/*/*.cpp ./src/*/*/*.cpp -o ./bin/assignment -lboost_iostreams -lboost_system -lboost_filesystem
install:
	sudo apt-get install libboost-iostreams-dev libboost-system-dev libboost-filesystem-dev
compile:
	make all
	make run
run:
	for npes in 1 2 3 4; do \
		mpiexec -np $$npes ./bin/assignment; \
	done;
memcheck:
	make all
	mpiexec -np 4 valgrind --tool=memcheck --leak-check=full ./bin/assignment
export:
	if [ -d ../outputs ]; then \
	  	make clean; \
	fi; \
	make all
	./bin/assignment
clean:
	\rm  -r ./outputs/*.png
plottimes:
	gnuplot -e "set ylabel \"time [s]\";set xlabel \"Number of processes\";set datafile separator ' '; set format x \"%.1g\"; set xtics 1; set term png; set output \"./outputs/times.png\"; plot \"./outputs/csvs/Crank-Nicholsondt=0.001.csv\" using 1:2 title 'Crank Nicholson' with linespoints ls 3 lw 2 lc rgb '#0060ad' pt 9 ps 1.5, \
     \"./outputs/csvs/Forward-Time-Central-Spacedt=0.001.csv\" using 1:2 title 'FTCS' with linespoints ls 2 lw 2 lc rgb '#dd181f' pt 5 ps 1.5, \
     \"./outputs/csvs/Laasonendt=0.001.csv\" using 1:2 title 'Laasonen' with linespoints ls 1 lw 2 lc rgb '#FFA500' pt 7 ps 1.5"
clean-csvs:
	rm ./outputs/csvs/*
generate-doxygen-config:
	if [ ! -d ./doxygen/ ]; then \
		sudo mkdir -p ./doxygen/; \
	fi; \
	sudo doxygen -g ./doxygen/doxygen.config
generate-doxygen:
	doxygen ./doxygen/doxygen.config
