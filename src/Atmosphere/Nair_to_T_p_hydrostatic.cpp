#include "Atmosphere.h"

void Nair_to_T_p_hydrostatic(/* 破壊的 */
	const int&         atm_Nheights,
	const PlanetParam& earth,
	      Atmosphere*& atm,     /* Nairのみ変更された大気 */
	const int&         iFixedTop/* 温度を固定する高度 */
){

	if (iFixedTop <= 0 || iFixedTop >= atm_Nheights)
		throw std::out_of_range("Invalid iFixedTop");
	
	for (int i = 0; i <= iFixedTop; ++i) {
		if (!std::isfinite(atm[i].z) || !std::isfinite(atm[i].Nair) || atm[i].Nair <= 0.0){
			throw std::invalid_argument(std::string(__func__) + "Invalid atmosphere");
		}
		if (i > 0 && atm[i].z <= atm[i-1].z){
			throw std::invalid_argument(std::string(__func__) + "Altitude must be strictly increasing");
		}
	}
	if (!std::isfinite(atm[iFixedTop].T) || atm[iFixedTop].T <= 0.0){
		throw std::invalid_argument(std::string(__func__) + "Invalid boundary temperature");
	}

	constexpr double GM_E      { 3.986004356e14 };/* 理科年表プレミアム 2025 */
	constexpr double KB        { 1.380649e-23 };
	constexpr double NA        { 6.02214076e23 };
	constexpr double ATM_MOL_W { 28.964420 };/* JIS W 0201 ISO標準大気 */
	
	const double m = ATM_MOL_W * 1.0e-3 / NA;
	const double R = earth.radius();/* [m] */
	
	for(int i=iFixedTop; i>0; --i){
		const double z1 = atm[i-1].z;
		const double z0 = atm[i].z;
		const double dz = z0 - z1;
		const double k = std::log(atm[i-1].Nair / atm[i].Nair) / dz;/* eを底とする指数係数 [np m^-1] */
		
		/* 台形積分 */
		constexpr int NSUB = 100;
		const double dx = dz / NSUB;
		double integral = 0.0;
		for (int j=0; j <= NSUB; ++j){
			const double x = dx*j;
			const double radius = R + z1 + x;
			const double f = std::exp(-k*x) / (radius*radius);
			const double weight = (j == 0 || j == NSUB) ? 0.5 : 1.0;/* 端は重み0.5 */
			integral += weight * f;
		}
		integral *= dx;

		/* T_{i-1} = T_{i}\exp (-k_i \Delta z_i) + \frac{mGM_E}{k_B} \int_0^{\Delta z_i} \frac{\exp(-k_i x)}{(R_E + z_i + x)^2} dx */
		atm[i-1].T = atm[i].T * std::exp(-k*dz) + (m*GM_E / KB)*integral;

		atm[i-1].set_p_from_Nair_T();
	}
	return;
}


