/*
 *
 *
 *
 */

#include "Sasktran2Geometry.h"
#include "FixedParams.h"
#include "coordinate.h"

#include <cmath>
#include <stdexcept>
#include <string>

Sasktran2TangentGeometry to_sasktran2_geometry(
	const PlanetParam& earth,
	const SatelliteParam& satellite,
	Geocoordinate tangential_point,
	double sza,
	double azimuth
){
	constexpr double DEG2RAD = M_PI / 180.0;

	const double earth_radius = earth.radius();
	const double satellite_radius = satellite.radius();

	const double tangent_altitude =
		tangential_point.altitude();

	const double observer_altitude =
		satellite_radius - earth_radius;

	// Input validation
	if (!std::isfinite(earth_radius) || !std::isfinite(satellite_radius) || earth_radius <= 0.0 || satellite_radius <= earth_radius) {
		throw std::invalid_argument( std::string(__func__) + ": Invalid earth or satellite radius" );
	}
	if (!std::isfinite(tangent_altitude) || tangent_altitude < 0.0 || tangent_altitude >= observer_altitude) {
		throw std::invalid_argument( std::string(__func__) + ": Invalid tangent altitude" );
	}
	if (!std::isfinite(sza) || !std::isfinite(azimuth) || sza < 0.0 || sza > 180.0) {
		throw std::invalid_argument( std::string(__func__) + ": Invalid solar angles" );
	}

	auto r = tangential_point.r();
	AndoLab::Vector3d<double> satellite_position( satellite_radius, 0.0, 0.0 );
	auto line_of_sight = r - satellite_position;
	const double r_norm = r.abs();
	const double los_norm = line_of_sight.abs();
	if (!std::isfinite(r_norm) || !std::isfinite(los_norm) || r_norm <= 0.0 || los_norm <= 0.0) {
		throw std::invalid_argument( std::string(__func__) + ": Invalid position" );
	}
	const double cos_angle = (r % line_of_sight) / (r_norm * los_norm);
	if (!std::isfinite(cos_angle) || std::abs(cos_angle) > 1.0e-8) {
		throw std::invalid_argument( std::string(__func__) + ": Position is not a tangent point" );
	}


	// Viewing direction at the tangent point
	double umu = 0.0;
	double viewing_azimuth = 0.0;

	sensor_direction(
		satellite,
		tangential_point.r(),
		&umu,
		&viewing_azimuth
	);

	if (!std::isfinite(umu) || !std::isfinite(viewing_azimuth)) {
		throw std::runtime_error( std::string(__func__) + ": Invalid viewing direction" );
	}

	// Solar-relative azimuth
	const double delta_azimuth = (azimuth - viewing_azimuth) * DEG2RAD;

	// [0, pi] radians
	const double relative_azimuth = std::acos(std::cos(delta_azimuth));

	Sasktran2TangentGeometry sk2geo;

	sk2geo.tangent_altitude_m   = tangent_altitude;
	sk2geo.observer_altitude_m  = observer_altitude;
	sk2geo.cos_sza              = std::cos(sza * DEG2RAD);
	sk2geo.relative_azimuth_rad = relative_azimuth;

	return sk2geo;
}


