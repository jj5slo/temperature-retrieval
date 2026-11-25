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
#include <csignal>
#include <cstdlib>
#include <algorithm>

#include "FixedParams.h"
#include "coordinate.h"
#include "PythonSasktran2.h"
#include "avoid_dupe.h"
// #include "run_python_sasktran2.h"


int main()
{
	std::signal(SIGPIPE, SIG_IGN);// Pythonプロセスが終了したときにエラーを出してから終了する

	try {

		const PlanetParam    earth(6370.0e3);
		const SatelliteParam himawari(35790.e3 + earth.radius(), 0.0, 140.7);

		/* ==== Pythonの起動 ==== */
		const std::string python = std::filesystem::absolute(".venv/bin/python").string();
		const std::string script = std::filesystem::absolute("python/python_daemon.py").string();
		PythonSasktran2 engine(python, script, Sasktran2Accuracy());	
		/* ==== */
		/* ==== 開始時刻保存, 保存先ディレクトリ作成 ==== */
		std::string DIR_RESULT = "/lhome/sano2/SANO/research/estimate-profile/temperature-retrieval/sandbox/";
		auto starttime = std::chrono::system_clock::now();
		auto startsec = std::chrono::duration_cast<std::chrono::seconds>(starttime.time_since_epoch()).count();
		std::string secid = std::to_string(startsec);
		
		int ifdupe = avoid_dupe(DIR_RESULT);
		if(ifdupe){
			DIR_RESULT = DIR_RESULT +"_"+ std::to_string(ifdupe);
			secid = secid + std::to_string(ifdupe);
		}
		std::cerr << "create_directory " << DIR_RESULT << std::endl;
		std::filesystem::create_directory(DIR_RESULT);
		
		std::ofstream timestamp_file(DIR_RESULT+"/timestamp.txt");
		if(!timestamp_file){
			std::cerr << "timestamp.txt could not make!" << std::endl;
			return 1;
		}
		timestamp_file << "start: " << startsec << std::endl;
		timestamp_file.close();
		/* ==== */
		
		double* radiances = new double[101];

		const double SZA_ON_GROUND     = 80.0;
		const double AZIMUTH_ON_GROUND = 120.0;
		const double ld_alpha = 0.0;
		Geocoordinate on_ground = LookingDirection(ld_alpha, 0.0).tangential_point(earth, himawari);

		for(int i=0; i<100; ++i){
			double tangential_height = (double)i * 1.0e3;
			
			LookingDirection ld(ld_alpha, tangential_height);
			Geocoordinate tp = ld.tangential_point(earth, himawari);
	
			const double latitude_difference = (on_ground.latitude() - tp.latitude()) / AndoLab::RAD2DEG;
			const double SZA_ON_GROUND_rad = SZA_ON_GROUND * M_PI / 180.0;
			const double AZIMUTH_ON_GROUND_rad = AZIMUTH_ON_GROUND * M_PI / 180.0;

			const double cos_sza = cos(SZA_ON_GROUND_rad) * cos(latitude_difference) - sin(SZA_ON_GROUND_rad) * cos(AZIMUTH_ON_GROUND_rad) * sin(latitude_difference);
			const double sza_deg     = acos(std::clamp(cos_sza, -1.0, 1.0)) * 180.0 / M_PI;
			const double azimuth_deg = atan2(
				sin(SZA_ON_GROUND_rad) * sin(AZIMUTH_ON_GROUND_rad),
				sin(SZA_ON_GROUND_rad) * cos(AZIMUTH_ON_GROUND_rad) * cos(latitude_difference)
				+ cos(SZA_ON_GROUND_rad) * sin(latitude_difference)
			) * 180.0 / M_PI;
	
			Sasktran2TangentGeometry geo =
				to_sasktran2_geometry(
					earth, himawari, tp, sza_deg, azimuth_deg
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
		
		/* ==== 終了時刻保存 ==== */
		auto endtime = std::chrono::system_clock::now();
		auto endsec = std::chrono::duration_cast<std::chrono::seconds>(endtime.time_since_epoch()).count();
		timestamp_file.open(DIR_RESULT+"/timestamp.txt", std::ios::app);
		if(timestamp_file){
			timestamp_file << "end: " << endsec << std::endl;
			timestamp_file.close();
		}
		/* ==== */
		
		for(int i=0; i<100; ++i){
			std::cout << i <<" "<< radiances[i] << std::endl;
		}
		std::ofstream result_file(DIR_RESULT+"/result.txt");
		if(!result_file){
			std::cerr << "result.txt could not make!" << std::endl;
			return 1;
		}
		result_file << "#altitude radiance" << std::endl;
		for(int i=0; i<100; ++i){
			result_file << i <<" "<< radiances[i] << std::endl;
		}
		result_file.close();

		return 0;
	}
	catch (const std::exception& e) {
		std::cerr << "ERROR: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_FAILURE;
}


