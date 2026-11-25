
#ifndef PYTHON_SASKTRAN2_H
#define PYTHON_SASKTRAN2_H

#include <cstdio>
#include <string>
#include <sys/types.h>

#include "FixedParams.h"
#include "coordinate.h"

/* ==== Geometry ==== */
// TangentAltitudeSolar
struct Sasktran2TangentGeometry {
    double tangent_altitude_m;
    double observer_altitude_m;
    double cos_sza;
    double relative_azimuth_rad;
};

// SolarAnglesObserverLocation
struct Sasktran2ObserverGeometry {
    double observer_altitude_m;
    double cos_sza;
    double relative_azimuth_rad;
    double cos_viewing_zenith;
};

/* to_sasktran2_geometry.cpp */
Sasktran2TangentGeometry to_sasktran2_geometry(
	const PlanetParam& earth,
	const SatelliteParam& satellite,
	Geocoordinate tp,
	const double sza,
	const double azimuth
);
/* ==== */
/* ==== Accuracy ==== */
struct Sasktran2Accuracy {
	int num_sza = 7;
	int num_successive_orders_incoming = 110;
	int num_successive_orders_outgoing = 110;
	int num_successive_orders_iterations = 50;

	double successive_orders_relative_tolerance = 1e-6;
	double successive_orders_absolute_tolerance = 1e-12;
};
/* ==== */

class PythonSasktran2 {
private:
	pid_t pid_ = -1;
	FILE* input_ = nullptr;
	FILE* output_ = nullptr;

public:
	PythonSasktran2(
		const std::string& python,
		const std::string& script,
		const Sasktran2Accuracy& accuracy
	);

	~PythonSasktran2();

	PythonSasktran2(const PythonSasktran2&) = delete;
	PythonSasktran2& operator=(const PythonSasktran2&) = delete;

	double calculate(const Sasktran2TangentGeometry& geo);
};

#endif

