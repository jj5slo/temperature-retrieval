#include <cmath>
#include <Vector3d.h>

#include "coordinate.h"
/* TODO satellite.latitude() も反映するようにする */
/* TODO satelliteをx軸にとるのをやめる、または内部に衛星情報を持ってgeocoord.convert(satellite2)とかで変換できるようにする（こちらのほうが現実的）（途中で使う衛星を変えられるようにする） */
Geocoordinate::Geocoordinate(
		PlanetParam earth, SatelliteParam satellite, AndoLab::Vector3d <double> r){
	set(earth, satellite, r);
}

Geocoordinate::Geocoordinate(
		PlanetParam earth, SatelliteParam satellite, const double Lat, const double Long, const double Alt){
	set(earth, satellite, Lat, Long, Alt);
}

void Geocoordinate::set(PlanetParam earth, SatelliteParam satellite, AndoLab::Vector3d <double> r){
	pPlanet = earth;
	pSatellite = satellite;
	pR = r;

	pAltitude = r.r() - earth.radius();

	/* φは東経140.7°が基準になっている */
	double Phi = r.phi() + satellite.longitude()*Deg2rad;
	if ( Phi > M_PI ){
		Phi -= 2.0*M_PI;
	}
	pLongitude = Phi * Rad2deg;

	pLatitude = ( M_PI/2.0 - r.theta() ) * Rad2deg;

}

void Geocoordinate::set(
		PlanetParam earth, SatelliteParam satellite, const double Lat, const double Long, const double Alt){
	pPlanet = earth;
	pSatellite = satellite;

	pLatitude = Lat;
	pLongitude = Long;
	pAltitude = Alt;

	const double r = pAltitude + earth.radius();
	const double th = (90.0 - Lat) * Deg2rad;
	double ph = (Long - satellite.longitude()) * Deg2rad;
	if ( ph < -M_PI ){
		ph += 2.0*M_PI;
	}

	pR.set(r, th, ph, AndoLab::coordinate::Spherical);
}

/* pRの点を、衛星(x軸上)から見て、yz平面へ !! 視線方向へ !! 射影した点 */
AndoLab::Vector3d <double> Geocoordinate::projection_on_yz(void){

	AndoLab::Vector3d <double> r_geo { pSatellite.radius(), 0.0, 0.0 }; /* ひまわりの位置 */

	/* r0 = R r^(θ, φ)
	 * d^ = (r0 - Rgeo)/|r0 - Rgeo|
	 * (Rgeo + t d^)・x^ = 0 となるtを探す
	 * Rgeo.x + t d^.x = 0 ←→ t = - Rgeo.x / d^.x;
	 * (Rgeo + t d^)・z^ / |Rgeo + t d^| = cosα
	 **/

	AndoLab::Vector3d <double> dn = (pR - r_geo).n();
	double t = - r_geo.x() / dn.x();
	return r_geo + t * dn;
}


double Geocoordinate::alpha(void){/* const */
	/* ひまわりから見てyz軸上へ射影した点の、z軸からの角度α
	 * -y方向(ヨーロッパ側)がαが正、y方向(アメリカ側)がαは負
	 * 　　ｚ
	 * ＼α↑
	 * 　＼│
	 * ──┴─→y
	 */
	AndoLab::Vector3d <double> yz_pts = projection_on_yz();
	return std::atan2( -yz_pts.y(), yz_pts.z() );
}


double Geocoordinate::east_longitude(void){
	double east_longi;

	if ( pLongitude > 0.0 ){ /* 東経のときはそのまま返す */
		east_longi = pLongitude;
	} else { /* 西経のとき（負のとき）は、-180°からの距離を180°に足す */
		east_longi = 180.0 + ( pLongitude - (-180.0) );
	}
	return east_longi;
}
