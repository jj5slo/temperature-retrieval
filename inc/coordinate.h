/*
 * coordinate.h
 *
 * 座標変換系の定義
 *
 */
#ifndef __COORDINATE_H__
#define __COORDINATE_H__

#include<iostream>

#include<cmath>
#include<Vector3d.h>

#include"FixedParams.h"

constexpr double m2km { 1.0e-3 };
constexpr double Deg2rad { M_PI / 180.0 };
constexpr double Rad2deg { 180.0 / M_PI };

class LookingDirection;
class Geocoordinate;

/* translate-coordinate.cpp */
/* 一般的な座標変換 */
/* PMC から流用 */
AndoLab::Vector3d <double> rotate_alpha(
		AndoLab::Vector3d <double> r, const double alpha);/* Ando */
AndoLab::Vector3d <double> rotate_theta(
		AndoLab::Vector3d <double> r, const double theta);/* Ando */


/* LookingDirection.cpp */

class LookingDirection{
private:
	double pAltitude{0.0};	/* [m] */
	double pAlpha{0.0};		/* [deg] */
public:
	/* LookingDirection.cpp */
	/* アクセサ */
	LookingDirection(){}
	LookingDirection(const double Alpha, const double Alt);
	void set(const double Alpha, const double Alt);
	double alpha(void);
	double altitude(void);

	/* tangential point の座標を求める */
	Geocoordinate tangential_point(PlanetParam earth, SatelliteParam satellite);
};


/* Geocoordinate.cpp */

class Geocoordinate{
private:
	double pLatitude;  /* [deg] */
	double pLongitude; /* [deg] */
	double pAltitude;  /* [m] */
	AndoLab::Vector3d <double> pR;

	PlanetParam pPlanet;
	SatelliteParam pSatellite;

	AndoLab::Vector3d <double> projection_on_yz(void);

public:
	Geocoordinate(void):
		pLatitude(0.0), pLongitude(0.0), pAltitude(0.0), pSatellite(42164.e3,0.0,0.0)  { }

	Geocoordinate(PlanetParam earth, SatelliteParam satellite, AndoLab::Vector3d <double> r);
	Geocoordinate(PlanetParam earth, SatelliteParam satellite, const double Latitude, const double Longitude, const double Altitude);
			/* lat, lon, alt はterrestrialで指定、Vector3d r は衛星基準で指定。 */

	/* アクセサ */
	void set(PlanetParam earth, SatelliteParam satellite, AndoLab::Vector3d <double> r);
	void set(PlanetParam earth, SatelliteParam satellite, const double Latitude, const double Longitude, const double Altitude);
	double latitude(void){ return pLatitude; }
	double longitude(void){ return pLongitude; }
	double east_longitude(void); /* 全て東経で述べた経度 */
	double altitude(void){ return pAltitude; }
	double altitude_in_km(void){ return pAltitude*m2km; }
	AndoLab::Vector3d <double> r(void){ return pR; }
	PlanetParam planet(void){ return pPlanet; }
	SatelliteParam satellite(void){ return pSatellite; }

	/* 衛星からみた角度αに直す */
	double alpha(void);

	/* LookingDirection に直す */
//	LookingDirection LookingDirection(earth, satellite);
	/*	リムでないところを見ているときにはAltitudeが負の値になるようにしたい(基準はその方向のtangentialpoint?) */

};


/* across_point_atmosphere.cpp */
AndoLab::Vector3d <double> cross_point_at_altitude(PlanetParam earth, AndoLab::Vector3d <double> r, AndoLab::Vector3d <double> d, const double H);
AndoLab::Vector3d <double> *Across_point_atmosphere(PlanetParam earth, SatelliteParam satellite, AndoLab::Vector3d <double> r, double TOA/* km */);


/* sensor_direction.cpp */
void sensor_direction(
	SatelliteParam satellite,
	AndoLab::Vector3d <double> crosspt,
	double *umu, /* output polar angle, 鉛直下から測った視線方向の角度 の cosine */
	double *phi /* センサで見える方向の方位角。 [deg] */
);





#endif
