
#include<Vector3d.h>

#include"coordinate.h"


void sensor_direction_old(
	SatelliteParam satellite,
	AndoLab::Vector3d <double> crosspt,
	double *umu, /* output polar angle, 鉛直下から測った視線方向の角度 の cos */
	double *phi /* センサで見える方向の方位角。 [deg] */
){
	
	// AndoLab::Vector3d <double> satellitedir(satellite.radius(), 0.0, 0.0);/* 地心から衛星方向を向いたベクトル */
	AndoLab::Vector3d <double> looking_satellite(satellite.radius() - crosspt.x(), -crosspt.y(), -crosspt.z());/* crossptから衛星方向を向いたベクトル */

	*umu = std::cos( angle_between(looking_satellite.n(), crosspt.n()) );/* 鉛直上方向と衛星方向のなす角 = 鉛直下方向と視線方向のなす角 のコサイン */
	

	AndoLab::Vector3d <double> horizontal_looking_satellite = looking_satellite - ((crosspt.n() % looking_satellite) * crosspt.n());/* crossptから衛星方向を向いたベクトルの局所水平成分 */
	AndoLab::Vector3d <double> northward(
			-crosspt.n().z()*crosspt.n().x(),
			-crosspt.n().z()*crosspt.n().y(),
			1.0 - crosspt.n().z()*crosspt.n().z()
			);/* z方向単位ベクトルの局所水平成分（北方向ベクトル） */

	double fromnorth = angle_between((-1.0)*horizontal_looking_satellite.n(), northward.n()) * Rad2deg;/* 衛星方向の北からの角度 (0 < fromnorth < 180 ) */
	

	AndoLab::Vector3d <double> eastward(
			-crosspt.n().y()*crosspt.n().x(),
			1.0 - crosspt.n().y()*crosspt.n().y(),
			-crosspt.n().y()*crosspt.n().z()
			);/* y方向単位ベクトルの局所水平成分（北方向ベクトル） */

	double fromeast = angle_between((-1.0)*horizontal_looking_satellite.n(), eastward.n()) * Rad2deg;/* 衛星方向の東からの角度 (0 < fromeast < 180 ) */
	
	if(0.0<fromeast && fromeast<90.0){
		*phi = fromnorth;/* East */
	}
	else if(90.0<fromeast && fromeast<180.0){
		*phi = 360.0 - fromnorth;/* West */
	}
	else{/* double なので滅多に起きないと思うが */
		if(fromeast == 0.0){
			*phi = 90.0;
		}
		else if(fromeast == 180.0){
			*phi = 270.0;
		}
	}
}

		


