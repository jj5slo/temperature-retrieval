#ifndef __INTERFACE_H__
#define __INTERFACE_H__


// enum class SurfaceType {
// 	ABSORB,
// 	LAMBERT,
// 	LAMBERT_WAVELENGTH,/* 波長によって違う反射率を記載したファイルを用意 */
// 	LAMBERT_IGBP,/* brdf_surface_typeで指定する
// 	BRDF_CAM;
// };

constexpr double BOLTZMANN_CONSTANT = 1.380649e-23;

class ParamStdin{/* stdin に渡すパラメタをまとめたクラス（後で拡張しやすいように） */
private:
public:
//	enum solver {mystic_spherical};
	std::string atmosphere_file = "../data/atmmod/afglus.dat";

	double wavelength {400.0};
	double sza {0.0};
	double phi0 {0.0};
	double umu {0.0};
	double phi {0.0};

	/* ---- surface condition ---- */
	std::string SURFACE_TYPE = "ABSORB";/* 判定用。ファイルに書き込まない */
	// ABSORB,
	// LAMBERT,
	// 未実装 LAMBERT_WAVELENGTH,/* 波長によって違う反射率を記載したファイルを用意 */
	// 未実装 LAMBERT_IGBP,/* brdf_surface_typeで指定する */
	// BRDF_CAM;
	double albedo{0.0};
	std::string brdf_surface_type = "";
	std::string albedo_file = "";
	double brdf_cam_u10{15.0};
	double brdf_rpv_type{0};
	/* ---- */

	int mc_photons {1000000};

	std::string solver = "mystic";
	std::string mc_basename = "mc";
	std::string additional = "";

};

class ParamAtmosphere{/* libRadtranの大気プロファイルに設定できる諸量。配列にして使う。 */
private:
	int p_NoPs = 9; /* パラメタの数 */

public:
	double z;/* [km] */
	double p;/* 圧力[hPa] */
	double T;/* 温度[K] */
	double Nair;/* 空気分子の数密度[cm-3] */
	double No3 ;
	double No2 ;
	double Nh2o;
	double Nco2;
	double Nno2;

	// デフォルトコンストラクタ
	ParamAtmosphere()
		: z(0.0), p(0.0), T(0.0), Nair(0.0),
		  No3(0.0), No2(0.0), Nh2o(0.0),
		  Nco2(0.0), Nno2(0.0) {}

	// コピーコンストラクタ
	ParamAtmosphere(const ParamAtmosphere&) = default;

	// コピー代入演算子
	ParamAtmosphere& operator=(const ParamAtmosphere&) = default;

	// ムーブコンストラクタ
	ParamAtmosphere(ParamAtmosphere&&) = default;

	// ムーブ代入演算子
	ParamAtmosphere& operator=(ParamAtmosphere&&) = default;

	// デストラクタ
	~ParamAtmosphere() = default;

	/* interface-atmosphere.cpp */
	double* returnvector(void);
	int NoPs(void){
		return p_NoPs;
	}
	void set_p_from_Nair_T(void);/* Nair, T から状態方程式で p を設定し直す */
	void set_T_from_Nair_p(void);/* Nair, T から状態方程式で p を設定し直す */
	void set_T_p_from_Nair_hydrostatic(void);
};

ParamAtmosphere* readParamAtmosphere(std::string filename, int& Nlines);

void set_Nair_10_exponentially(int Nheights, ParamAtmosphere*& pAtm, int iTOP, double Coef);

/* set_T_p_from_Nair_hydrostatic.cpp */


/* interface_stdin.cpp */
int save_stdin(std::string path_stdin, ParamStdin param);
	/* 0:問題なし, 1:Error */

/* interface-atmosphere.cpp */
void saveParamAtmosphere(std::string filename, ParamAtmosphere *params, int Nlines, int Ndecimal = 6);/* 保存先、パラメータの配列、高度の総数、有効数字 */

#endif /* __INTERFACE_H__ */

