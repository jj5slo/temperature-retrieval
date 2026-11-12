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
#include <sstream>
#include <cmath>
		

#include "FixedParams.h"
#include "coordinate.h"
#include "Sasktran2Geometry.h"
#include "PythonSasktran2.h"
// #include "run_python_sasktran2.h"


int main()
{
	try {
		PlanetParam earth;
		SatelliteParam satellite;

		const std::string python = std::filesystem::absolute(".venv/bin/python").string();
	
		const std::string script = std::filesystem::absolute("python/python_daemon.py").string();

		PythonSasktran2 engine(python, script);	
		
		double* radiances = new double[101];	

		for(int i=0; i<100; ++i){
			double tangential_height = (double)i * 1.0e3;
			
			LookingDirection ld(30.0, tangential_height);
	
			const double sza = 80.0;
			const double azimuth = 120.0;
	
			Sasktran2TangentGeometry geo =
				to_sasktran2_geometry(
					earth, satellite, ld, sza, azimuth
				);
	
			std::cout << "C++: starting Python\n" << std::flush;
	
			double radiance = engine.calculate(geo);
			
			std::cout << std::setprecision(17);
			std::cout << "Radiance [sr^-1]: " << radiance << '\n';
			std::cout << "Integration test PASSED\n";
	
			std::cout << std::setprecision(17);
			std::cout << "tangent_altitude_m = "   << geo.tangent_altitude_m << '\n';
			std::cout << "observer_altitude_m = "  << geo.observer_altitude_m << '\n';
			std::cout << "cos_sza = "              << geo.cos_sza << '\n';
			std::cout << "relative_azimuth_rad = " << geo.relative_azimuth_rad << '\n';

			radiances[i] = radiance;
		}
		
		for(int i=0; i<100; ++i){
			std::cout << i <<" "<< radiances[i] << std::endl;
		}

		return 0;
	}
	catch (const std::exception& e) {
		std::cerr << "ERROR: " << e.what() << '\n';
		return 1;
	}
}


