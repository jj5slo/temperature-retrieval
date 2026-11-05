#ifndef ATMOSPHERE_H__
#define ATMOSPHERE_H__

#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <vector>

#include "FixedParams.h"

class Atmosphere {
public:
	// SI constants
	static constexpr double kB = 1.380649e-23; // [J/K]

	// Atmospheric state
	double z    = 0.0;       // Altitude [m]
	double p    = 0.0;       // Pressure [Pa]
	double T    = 0.0;       // Temperature [K]
	double Nair = 0.0;    // Air number density [m^-3]

	// (Fixed) molecular densities [m^-3]
	double No2 = 0.0;     // O2
	double No3  = 0.0;    // O3
	double Nh2o = 0.0;    // H2O
	double Nco2 = 0.0;    // CO2
	double Nno2 = 0.0;    // NO2

	// Humidity
	double specific_humidity = 0.0; // [kg/kg]

	// Special member functions
	Atmosphere() = default;
	Atmosphere(const Atmosphere&) = default;
	Atmosphere& operator=(const Atmosphere&) = default;
	Atmosphere(Atmosphere&&) = default;
	Atmosphere& operator=(Atmosphere&&) = default;
	~Atmosphere() = default;

	// Equation of state
	void set_p_from_Nair_T() {
		if (!std::isfinite(Nair) || !std::isfinite(T) || Nair < 0.0 || T <= 0.0)
			throw std::invalid_argument(
				"Invalid Nair or T"
			);

		p = Nair * kB * T;
	}

	void set_T_from_Nair_p() {
		if (!std::isfinite(Nair) || !std::isfinite(p) || Nair <= 0.0 || p <= 0.0)
			throw std::invalid_argument(
				"Invalid Nair or p"
			);

		T = p / (Nair * kB);
	}

	void set_Nair_from_p_T() {
		if (!std::isfinite(p) || !std::isfinite(T) || p < 0.0 || T <= 0.0)
			throw std::invalid_argument(
				"Invalid p or T"
			);

		Nair = p / (kB * T);
	}

	// Consistency check: p = Nair * kB * T
	bool check_equation_of_state(
		double relative_tolerance = 1e-6
	) const {
		if (p < 0.0 || Nair < 0.0 || T <= 0.0)
			return false;

		const double expected_p = Nair * kB * T;
		const double scale = std::fmax(
			std::fmax(p, expected_p), 1e-100
		);

		return std::fabs(p - expected_p)
			   <= relative_tolerance * scale;
	}

};

/* Nair_to_T_p_hydrostatic.cpp */
void Nair_to_T_p_hydrostatic(/* 破壊的 */
	const int&         atm_Nheights,
	const PlanetParam& earth,
	      Atmosphere*& atm,      /* Nairのみ変更された大気 */
	const int&         iFixedTop /* 温度を固定する高度 */
);


#endif
