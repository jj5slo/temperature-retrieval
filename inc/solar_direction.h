/*
 * 太陽天頂角、方位角を計算する。
 *
 * 式は「長沢工, 日の出・日の入りの計算、地人書館」から(らしい。未確認)
 *
 */

#ifndef __SOLAR_DIRECTION_H__
#define __SOLAR_DIRECTION_H__

namespace AndoLab{

void solar_direction
(
 const double Latitude,
 const double Longitude,
 const int Day_of_Year,
 const double Hour_UT, /* UT[h]  分以下は小数で入れる */
 double *ZenithAngle, /* [deg] 太陽天頂角 */
 double *Azimuth /* [deg] 方位角。北が 0°、東回り。0-360° */
 );

}
#endif
