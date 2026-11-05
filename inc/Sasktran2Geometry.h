#ifndef SASKTRAN2_GEOMETRY_H__
#define SASKTRAN2_GEOMETRY_H__

#include "FixedParams.h"
#include "coordinate.h"

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
	const Geocoordinate tangential_point,
	const double sza, const double azimuth
);


#endif
