#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <algorithm>
#include <string>

#include "coordinate.h"

// Old implementation
void sensor_direction_old(
    SatelliteParam satellite,
    AndoLab::Vector3d<double> crosspt,
    double* umu,
    double* phi
);



#include <Vector3d.h>
#include <cmath>
#include <stdexcept>

AndoLab::Vector3d<double> atmosphere_top_intersection(
    const PlanetParam& earth,
    SatelliteParam satellite,
    AndoLab::Vector3d<double> tangent_point,
    double top_altitude
){
    using Vec3 = AndoLab::Vector3d<double>;

    const double Rtop = earth.radius() + top_altitude;

    // 衛星位置 (x軸上)
    Vec3 observer(satellite.radius(), 0.0, 0.0);

    // 接線点から衛星へ向かう単位ベクトル
    Vec3 direction = observer - tangent_point;

    const double length = direction.abs();

    if (!std::isfinite(length) || length <= 0.0)
        throw std::invalid_argument(
            "atmosphere_top_intersection: Invalid direction"
        );

    direction = direction / length;

    // r(s) = tangent_point + s * direction
    // |r(s)|^2 = Rtop^2
    const double b = tangent_point % direction;

    double discriminant =
        b*b + Rtop*Rtop
        - (tangent_point % tangent_point);

    // 丸め誤差への対処
    if (discriminant < -1.0e-4)
        throw std::domain_error(
            "atmosphere_top_intersection: No intersection"
        );

    discriminant = std::max(0.0, discriminant);

    // 衛星側の交点
    const double s = -b + std::sqrt(discriminant);

    if (s < 0.0 || s > length)
        throw std::domain_error(
            "atmosphere_top_intersection: Intersection outside segment"
        );

    return tangent_point + s * direction;
}


// 上で定義した atmosphere_top_intersection() を
// このmainより前に配置すること。

double angle_difference(double a, double b)
{
    return std::abs(std::remainder(a - b, 360.0));
}



int main()
{
    constexpr double TOP_ALTITUDE = 100e3; // [m]
    constexpr double TOL_PHI = 1.0e-8;     // [deg]
    constexpr double TOL_UMU = 1.0e-12;

    PlanetParam earth(6370e3);
    SatelliteParam satellite(42164e3, 0.0, 140.7);

    std::ofstream fout("sensor_direction_comparison.csv");

    if (!fout)
        throw std::runtime_error("Cannot open output file");

    fout << std::setprecision(16);

    fout << "alpha_deg,height_km,"
         << "umu_old,umu_new,diff_umu,"
         << "phi_old,phi_new,diff_phi\n";

    int n_total = 0;
    int n_valid = 0;
    int n_mismatch = 0;
    int n_invalid = 0;

    double max_diff_phi = 0.0;
    double max_diff_umu = 0.0;

    int max_phi_alpha = 0;
    int max_phi_height = 0;

    const double NaN =
        std::numeric_limits<double>::quiet_NaN();

    for (int height_km = 0; height_km <= 100; ++height_km) {
        for (int alpha_deg = 0; alpha_deg < 360; ++alpha_deg) {

            ++n_total;

            try {
                LookingDirection look(
                    static_cast<double>(alpha_deg),
                    static_cast<double>(height_km) * 1000.0
                );

                // 接線点
                Geocoordinate tangent =
                    look.tangential_point(earth, satellite);

                // 視線と大気上端（100 km）の交点
                auto crosspt = atmosphere_top_intersection(
                    earth,
                    satellite,
                    tangent.r(),
                    TOP_ALTITUDE
                );

                double umu_old = NaN;
                double phi_old = NaN;
                double umu_new = NaN;
                double phi_new = NaN;

                sensor_direction_old(
                    satellite, crosspt, &umu_old, &phi_old
                );

                sensor_direction(
                    satellite, crosspt, &umu_new, &phi_new
                );

                const bool valid =
                    std::isfinite(umu_old) &&
                    std::isfinite(umu_new) &&
                    std::isfinite(phi_old) &&
                    std::isfinite(phi_new);

                double diff_umu = NaN;
                double diff_phi = NaN;

                if (valid) {
                    diff_umu = std::abs(umu_new - umu_old);
                    diff_phi = angle_difference(phi_new, phi_old);
                    ++n_valid;

                    max_diff_umu = std::max(max_diff_umu, diff_umu);

                    if (diff_phi > max_diff_phi) {
                        max_diff_phi = diff_phi;
                        max_phi_alpha = alpha_deg;
                        max_phi_height = height_km;
                    }

                    if (diff_phi > TOL_PHI ||
                        diff_umu > TOL_UMU) {

                        ++n_mismatch;

                        std::cout << std::setprecision(12)
                                  << "MISMATCH: alpha=" << alpha_deg
                                  << " height=" << height_km
                                  << " dPhi=" << diff_phi
                                  << " dUmu=" << diff_umu
                                  << '\n';
                    }
                }
                else {
                    ++n_invalid;

                    std::cout << "INVALID: alpha=" << alpha_deg
                              << " height=" << height_km
                              << " phi_old=" << phi_old
                              << " phi_new=" << phi_new
                              << '\n';
                }

                fout << alpha_deg << ","
                     << height_km << ","
                     << umu_old << ","
                     << umu_new << ","
                     << diff_umu << ","
                     << phi_old << ","
                     << phi_new << ","
                     << diff_phi << '\n';
            }
            catch (const std::exception& e) {
                ++n_invalid;

                std::cerr << "ERROR: alpha=" << alpha_deg
                          << " height=" << height_km
                          << " : " << e.what() << '\n';
            }
            catch (const std::string& e) {
                ++n_invalid;

                std::cerr << "ERROR: alpha=" << alpha_deg
                          << " height=" << height_km
                          << " : " << e << '\n';
            }
        }
    }

    std::cout << "\n===== Comparison Summary =====\n"
              << "Total points   : " << n_total << '\n'
              << "Valid points   : " << n_valid << '\n'
              << "Invalid points : " << n_invalid << '\n'
              << "Mismatches     : " << n_mismatch << '\n'
              << std::setprecision(16)
              << "Maximum dPhi   : " << max_diff_phi << " deg\n"
              << "Maximum dUmu   : " << max_diff_umu << '\n'
              << "Max dPhi at    : alpha=" << max_phi_alpha
              << " deg, height=" << max_phi_height
              << " km\n";

    return (n_mismatch == 0 && n_invalid == 0) ? 0 : 1;
}


