#include "iomanager.h"

// CONSTRUCTORS
/*=
 * Default constructor
 */

IOManager::IOManager() {
	output_path = OUTPUT_PATH;
}

// PLOT METHODS

/*
* private output method - creates output folder 
*/

bool IOManager::create_output_dir() {
	char answer = '.';

	// if plot folder exists ask if the user wants to replace the previous results
	if(boost::filesystem::exists(output_path)) {
		while (answer != 'y' && answer != 'Y' && answer != 'n' && answer != 'N') {
			std::cout << "Output folder already exists, this program might overwrite some files, procceed? [Y:N] - ";
			answer = std::getchar();
			std::cout << std::endl;
		}
		return answer == 'y' || answer == 'Y' ? true : false;
	} else {
		if(boost::filesystem::create_directory(output_path)) {
			std::cout << "Output folder was created!" << std::endl;
			return true;
		} else {
			std::cout << "Output folder couldn't be created! Not exporting outputs." << std::endl;
			return false;
		}
	}
}

/*
* public output method - iterates through all the solutions in order to export them in varied formats
*/

void IOManager::export_outputs(Method * analytical, std::vector<Method*> methods) {
	if (!create_output_dir()) return;
	std::cout << "Exporting outputs to " << boost::filesystem::canonical(output_path, ".") << std::endl;
	std::string name, output_name, deltat_string;
	for (unsigned int index = 0; index < methods.size(); index++) {
		name = (*methods[index]).get_name();
		deltat_string = name == LAASONEN ? double_to_string(3, methods[index]->get_deltat()) : double_to_string(2, methods[index]->get_deltat());
		output_name = output_path + '/' + name;
		if (name == LAASONEN) {
			output_name += "dt=" + deltat_string;
			laasonen_times.push_back(methods[index]->get_computational_time());
		} 
		if (! (name == LAASONEN && methods[index]->get_deltat() != 0.01) && name != RICHARDSON) {
			default_deltat_times.push_back(methods[index]->get_computational_time());
		}
		std::cout << "Exporting " << name << " method outputs... ";
		plot_solutions(output_name, analytical, methods[index]);
		std::cout << "Finished!" << std::endl;
	}
	std::vector<Method*> error_vector(methods.begin() + 1, methods.begin() + 4);
	error_tables(output_name, error_vector);
	plot_default_deltat_times();
	plot_laasonen_times();
}

/*
* private plot method - Exports a plot chart which compares the analytical solution with a given solution
*/

void IOManager::plot_solutions(std::string output_name, Method * analytical, Method * method) {
	// Object to export plots
	Gnuplot gp;

	// methods solutions
	Matrix analytical_matrix = analytical->get_solution(), method_matrix = method->get_solution();
	unsigned int rows = method_matrix.getNrows();
	unsigned int cols = method_matrix.getNcols();
	double time;
	std::string time_str, name = method->get_name();

	// defining gnuplot configuration with the correct syntax
	gp << "set key on box; set tics scale 0; set border 3; set ylabel \"Temperature [ºF]\";set xlabel \"x [ft]\"; set yrange [90:310]; set term png; set xtics (\"0\" 0, \"0.5\"" << cols / 2 << ", \"1\"" << cols - 1 << ")\n";
	for (unsigned int index = 1; index < rows; index++) {
		time = (double)index / 10.0;
		time_str = double_to_string(1, time);

		gp << "set output \"" << output_name << "t="  << time_str;
		gp << ".png\";\n";
		gp << "plot" << gp.file1d(analytical_matrix[index]) << "with lines title \"Analytical\" lw 2 lt rgb \"red\","
			<< gp.file1d(method_matrix[index]) << "with points title \"" << name << "\" pt 17 ps 1 lw 1" << std::endl;
	}
}

/*
* private plot method - Exports a plot chart with laasonen solutions computational times
*/

void IOManager::plot_laasonen_times() {
	// Object to export plots
	Gnuplot gp;

	gp << "set tics scale 0; set border 3; set style line 1 lc rgb '#0060ad' lt 1 lw 2 pt 7 pi -1 ps 1.5; set clip two; set ylabel \"Computational Time [ms]\";set xlabel \"Δ t [h]\"; set term png; set xtics (\"0.01\" 0, \"0.025\" 1, \"0.05\" 2, \"0.1\" 3)\n";
	gp << "set output \"" << output_path << "/laasonen_times.png\";\n";
	gp << "plot" << gp.file1d(laasonen_times) << " notitle with linespoint ls 1" << std::endl;
}

/*
* private plot method - Exports a plot chart with solutions computational times
*/

void IOManager::plot_default_deltat_times() {
	// Object to export plots
	Gnuplot gp;

	gp << "set tics scale 0; set border 3; set style line 1 lc rgb '#0060ad' lt 1 lw 2 pt 7 pi -1 ps 1.5; set clip two; set ylabel \"Computational Time [ms]\";set xlabel \"Δ t [h]\"; set term png; set xtics (\"Dufort-Frankel\" 0, \"Crank-Nicholson\" 1, \"Laasonen\" 2)\n";
	gp << "set output \"" << output_path << "/default_deltat_times.png\";\n";
	gp << "plot" << gp.file1d(default_deltat_times) << " notitle with linespoint ls 1" << std::endl;
}

/*
* private table method - Exports an error table to a .lsx file which compares the analytical solution with a given solution
*/
void IOManager::error_tables(std::string output_name, std::vector<Method*> methods) {

	// Object to export plots
	Gnuplot gp;
	std::vector<double> norms;

	for (unsigned int i = 0; i < methods.size(); i++) {
		norms.push_back(methods[i]->get_two_norm());
	}
	std::swap(norms[1],norms[2]);


	gp << "set tics scale 0; set border 3; set style line 1 lc rgb '#FFA500' lt 1 lw 2 pt 7 pi -1 ps 1.5; set clip two; set ylabel \"2nd Norm\";set xlabel \"\"; set term png; set xtics (\"Dufort-Frankel\" 0, \"Laasonen\" 1, \"Crank Nicholson\" 2)\n";
	gp << "set output \"" << output_path << "/norms.png\";\n";
	gp << "plot" << gp.file1d(norms) << " notitle with linespoint ls 1" << std::endl;
}

// AUX METHODS

/*
* aux method - Converts a double into a string
*/

std::string IOManager::double_to_string(int precision, double value) {
	std::stringstream stream;
	stream << std::fixed << std::setprecision(precision) << value;
	return stream.str();
}