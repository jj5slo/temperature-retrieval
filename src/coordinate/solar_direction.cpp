#include <iostream>
#include <cmath>

namespace AndoLab{

constexpr double _SD_Deg2rad { M_PI / 180.0 };
constexpr double _SD_w { 2.0 * M_PI / 365.0 };

void solar_direction
(
 const double lat,
 const double lon,
 const int doy,
 const double hours,
 double *ZenithAngle,
 double *Azimuth
 ){

  /*
   * Formula by 日の出・日の入りの計算－天体の出没時刻の
   */
  const double Ts { hours + 9.0 }; /* 時刻[h]（中央標準時）*/
  const double J { doy + 0.5 }; /* day of year + 0.5 */

  const double delta = (0.33281
                        - 22.984 * std::cos(_SD_w*J)
                        - 0.34990 * std::cos(2*_SD_w*J)
                        - 0.13980 * std::cos(3*_SD_w*J)
                        + 3.7872 * std::sin(_SD_w*J)
                        + 0.03250 * std::sin(2*_SD_w*J)
                        + 0.07187 * std::sin(3*_SD_w*J) ) * _SD_Deg2rad; /* 太陽赤緯[rad] */

  const double e = 0.0072*std::cos(_SD_w*J)
    - 0.0528 * std::cos(2*_SD_w*J)
    - 0.0012 * std::cos(3*_SD_w*J)
    - 0.1229 * std::sin(_SD_w*J)
    - 0.1565 * std::sin(2*_SD_w*J)
    - 0.0041 * std::sin(3*_SD_w*J); /* 均時差[h] */
  const double t = ( 15.0*( Ts + (lon - 135.0)/15.0 + e ) - 180.0) * _SD_Deg2rad; /* 時角[rad] */
  const double h = std::asin( std::sin(lat*_SD_Deg2rad) * std::sin(delta)
                              + std::cos(lat*_SD_Deg2rad) * std::cos(delta) * std::cos(t) ); /* 太陽高度角[rad] */
  const double th_h = M_PI/2.0 - h;//太陽天頂角[rad]

  const double sinA = std::cos(delta) * std::sin(t) / std::cos(h);
  const double cosA = ( std::sin(h) * std::sin(lat*_SD_Deg2rad) - std::sin(delta)) / ( std::cos(h) * std::cos(lat*_SD_Deg2rad) );
  const double A = atan2( sinA, cosA );//方位角、南=0、西=90

  *ZenithAngle = th_h / _SD_Deg2rad;
  *Azimuth = fmod( A / _SD_Deg2rad + 180.0, 360.0 );
}

}
