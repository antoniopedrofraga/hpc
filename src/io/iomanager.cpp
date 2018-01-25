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

void IOManager::export_outputs(Method * analytical, std::vector<Method*> methods, int number_processes) {
	//if (!create_output_dir()) return;
	std::cout << "Exporting outputs to " << boost::filesystem::canonical(output_path, ".") << std::endl;
	std::string name, output_name, deltat_string;
	for (unsigned int index = 0; index < methods.size(); index++) {
		name = (*methods[index]).get_name();
		deltat_string = double_to_string(3, methods[index]->get_deltat());
		output_name = output_path + '/' + name;
		output_name += "dt=" + deltat_string;
		std::cout << "Exporting " << name << " method outputs... ";
		plot_solutions(output_name, analytical, methods[index], number_processes);
		std::cout << "Finished!" << std::endl;
	}
	plot_times(output_path, analytical, methods, number_processes);
	export_csv(output_path, methods, number_processes);
	//std::vector<Method*> error_vector(methods.begin() + 1, methods.begin() + 4);
	//error_tables(output_name, error_vector);
}
/*
* private plot method - Exports a plot chart which compares the analytical solution with a given solution
*/

void IOManager::plot_solutions(std::string output_name, Method * analytical, Method * method, int number_processes) {
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
		gp << "p=" << number_processes <<  ".png\";\n";
		gp << "plot" << gp.file1d(analytical_matrix[index]) << "with lines title \"Analytical\" lw 2 lt rgb \"red\","
			<< gp.file1d(method_matrix[index]) << "with points title \"" << name << "\" pt 17 ps 1 lw 1" << std::endl;
	}
}

void IOManager::plot_times(std::string output_name, Method * analytical, std::vector<Method*> methods, int number_processes) {
	Gnuplot gp;
	std::vector<double> times;
	times.push_back(analytical->get_computational_time());

	for (unsigned int i = 0; i < methods.size(); i++) {
		times.push_back(methods[i]->get_computational_time());
	}

	std::string deltat_string = double_to_string(3, methods[0]->get_deltat());

	gp << "set tics scale 0; set border 3; set style line 1 lc rgb '#FFA500' lt 1 lw 2 pt 7 pi -1 ps 1.5; set clip two; set ylabel \"time [s]\";set xlabel \"\"; set term png; set xtics (\"Analytical\" 0, \"Laasonen\" 1, \"Crank Nicholson\" 2, \"FTCS\" 3)\n";
	gp << "set output \"" << output_path << "/timesdt=" << deltat_string << "p=" << number_processes << ".png\";\n";
	gp << "plot" << gp.file1d(times) << " notitle with linespoint ls 1" << std::endl;
}

void IOManager::export_analytical(Method * analytical, int number_processes) {
	// Object to export plots
	Gnuplot gp;

	// methods solutions
	Matrix analytical_matrix = analytical->get_solution();
	unsigned int rows = analytical_matrix.getNrows();
	unsigned int cols = analytical_matrix.getNcols();
	double time;
	std::string time_str;

	gp << "set key on box; set tics scale 0; set border 3; set ylabel \"Temperature [ºF]\";set xlabel \"x [ft]\"; set yrange [90:310]; set term png; set xtics (\"0\" 0, \"0.5\"" << cols / 2 << ", \"1\"" << cols - 1 << ")\n";
	for (unsigned int index = 1; index < rows; index++) {
		time = (double)index / 10.0;
		time_str = double_to_string(1, time);
		gp << "set output \"./outputs/analytical" + time_str;
		gp << "p=" << number_processes <<  ".png\";\n";
		gp << "plot" << gp.file1d(analytical_matrix[index]) << "with lines title \"Analytical\" lw 2 lt rgb \"red\""
			<< std::endl;
	}
}

void IOManager::export_csv(std::string output_name, std::vector<Method*> methods, int number_processes) {
	double time = methods[0]->get_deltat();
	std::string time_str = double_to_string(5, time);
	std::ofstream out;
	for (size_t i = 0; i < methods.size(); i++) {
		std::string name = output_name + "/csvs/" + methods[i]->get_name() + "dt=" + time_str + ".csv";
		out.open(name, std::ios_base::app);
		out << number_processes << " " << methods[i]->get_computational_time() << std::endl;
		out.close();
	}
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
