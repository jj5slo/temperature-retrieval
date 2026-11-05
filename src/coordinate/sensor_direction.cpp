
#include<Vector3d.h>

#include"coordinate.h"


void sensor_direction(
	SatelliteParam satellite,
	AndoLab::Vector3d <double> crosspt,
	double *umu, /* output polar angle, 鉛直下から測った視線方向の角度 の cos */
	double *phi /* センサで見える方向の方位角。 [deg] */
){
	constexpr double RAD2DEG = 180.0 / M_PI;

	if (umu == nullptr || phi == nullptr)
		throw std::invalid_argument(
			std::string(__func__) + ": Null output pointer"
		);

	if (!std::isfinite(satellite.radius()) ||
		satellite.radius() <= 0.0 ||
		!std::isfinite(crosspt.x()) ||
		!std::isfinite(crosspt.y()) ||
		!std::isfinite(crosspt.z()) ||
		crosspt.abs() <= 0.0)
		throw std::invalid_argument(
			std::string(__func__) + ": Invalid geometry"
		);

	// 接線点から衛星へ向かうベクトル
	AndoLab::Vector3d<double> looking_satellite(
		satellite.radius() - crosspt.x(),
		-crosspt.y(),
		-crosspt.z()
	);

	if (looking_satellite.abs() <= 0.0)
		throw std::invalid_argument(
			std::string(__func__) + ": Observer coincides with point"
		);

	AndoLab::Vector3d<double> up = crosspt.n();// 局所鉛直上方向
	AndoLab::Vector3d<double> to_satellite = looking_satellite.n();// 接線点から衛星へ向かう単位ベクトル
	*umu = up % to_satellite;// 鉛直下方向と視線方向のなす角のcos
	AndoLab::Vector3d<double> view = (-1.0) * to_satellite;// 衛星から接線点を通って進む視線方向
	AndoLab::Vector3d<double> z_axis(0.0, 0.0, 1.0);// 地球自転軸（北極方向）
	AndoLab::Vector3d<double> east_raw = z_axis * up;// 局所東方向

	// 極では東方向を定義できない
	if (east_raw.abs() < 1.0e-12)
		throw std::invalid_argument(
			std::string(__func__) + ": Azimuth undefined at pole"
		);

	AndoLab::Vector3d<double> east = east_raw.n();
	AndoLab::Vector3d<double> north = up * east;// 局所北方向

	// 北を0度、東を90度とする方位角
	*phi = std::atan2(
		view % east,
		view % north
	) * RAD2DEG;
	// [0, 360) deg
	if (*phi < 0.0)
		*phi += 360.0;
}




