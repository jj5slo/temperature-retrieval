#ifndef SASKTRAN2_GEOMETRY_H__
#define SASKTRAN2_GEOMETRY_H__

struct Sasktran2Geometry {
	double tangent_altitude_m;
	double observer_altitude_m;
	double cos_sza;
	double relative_azimuth_rad;
};

/* to_sasktran2_geometry.cpp */
Sasktran2Geometry to_sasktran2_geometry(
	const PlanetParam& earth,
	const SatelliteParam& satellite,
	const Geocoordinate tangential_point,
	const double sza, const double azimuth
);


#endif
