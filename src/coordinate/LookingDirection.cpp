/* 
 * LookingDirection.cpp
 *
 */
#include <cmath>
#include <stdexcept>
#include <string>

#include"coordinate.h"

LookingDirection::LookingDirection(const double Alpha, const double Alt){
	set(Alpha, Alt);
}

void LookingDirection::set(const double Alpha, const double Alt){
	pAlpha = Alpha;
	pAltitude = Alt;
}

double LookingDirection::alpha(void){
	return pAlpha;
}
double LookingDirection::altitude(void){
	return pAltitude;
}

Geocoordinate LookingDirection::tangential_point(PlanetParam earth, SatelliteParam satellite){
	const double R = earth.radius() + pAltitude;
	const double Rs = satellite.radius();

	if (!std::isfinite(pAltitude) ||
	    !std::isfinite(pAlpha) ||
	    !std::isfinite(Rs) ||
	    !std::isfinite(earth.radius()))
	{
		throw std::invalid_argument( std::string(__func__) + ": Non-finite geometry parameter" );
	}
	if (earth.radius() <= 0.0 || Rs <= 0.0 || R <= 0.0) {
		throw std::invalid_argument( std::string(__func__) + ": Invalid radius" );
	}
	if (R > Rs) {
		throw std::invalid_argument( std::string(__func__) + ": Tangent altitude exceeds observer altitude" );
	}

	const double cos_b = R / Rs;
	const double sin_b = std::sqrt( 1.0 - cos_b*cos_b );
	AndoLab::Vector3d <double> r( R*cos_b, 0.0, R*sin_b );

	return Geocoordinate(earth, satellite, rotate_alpha( r, pAlpha*Deg2rad ));
	//  return AndoLab::Vector3d <double> (
	//      R*cos_b, -R*sin_b*sin(alpha), R*sin_b*cos(alpha) );
}
	
