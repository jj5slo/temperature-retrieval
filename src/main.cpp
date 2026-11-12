// #include <iostream>
// #include <exception>
// 
// int main(int argc, char *argv[]){
// 	std::cout << "Temperature Retrieval" << std::endl;
// 	std::cout << "Program started successfully." << std::endl;
// 
// 	return 0;
// }


#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <filesystem>
		

#include "FixedParams.h"
#include "coordinate.h"
#include "Sasktran2Geometry.h"
#include "run_python_sasktran2.h"


int main()
{
	try {
		PlanetParam earth;
		SatelliteParam satellite;

		LookingDirection ld(30.0, 45000.0);

		const double sza = 80.0;
		const double azimuth = 120.0;

		Sasktran2TangentGeometry geo =
			to_sasktran2_geometry(
				earth, satellite, ld, sza, azimuth
			);

		const std::string python =
			std::filesystem::absolute(".venv/bin/python").string();

		const std::string script =
			std::filesystem::absolute("python/geometry_bridge.py").string();

		std::cout << "C++: starting Python\n"
				  << std::flush;

		std::string output =
			run_python_sasktran2(python, script, geo);
		
		if (output != "OK\n") {
			throw std::runtime_error(
				"Unexpected Python output: " + output
			);
		}
		
		std::cout << "Integration test PASSED\n";

		return 0;
	}
	catch (const std::exception& e) {
		std::cerr << "ERROR: " << e.what() << '\n';
		return 1;
	}
}


