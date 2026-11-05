#include <iostream>
#include <cmath>
#include <stdexcept>

#include "Atmosphere.h"

int main() {
    try {
        constexpr int N = 4;
        constexpr int iFixedTop = 2;

        constexpr double GM = 3.986004356e14;
        constexpr double NA = 6.02214076e23;
        constexpr double MOL_W = 28.964420e-3;

        const double m = MOL_W / NA;

        PlanetParam earth(6370e3);
        Atmosphere atm[N];

        // Constant number density
        for (int i = 0; i < N; ++i) {
            atm[i].z = (40.0 + 5.0*i)*1000.0;
            atm[i].Nair = 1.0e21;
        }

        // Fixed boundary at 50 km
        atm[iFixedTop].T = 250.0;
        atm[iFixedTop].set_p_from_Nair_T();

        // Fixed region above the boundary
        atm[3].T = 230.0;
        atm[3].set_p_from_Nair_T();

        Atmosphere* ptr = atm;

        Nair_to_T_p_hydrostatic(
            N, earth, ptr, iFixedTop
        );

        // Analytical solution for constant Nair
        const double R = earth.radius();
        const double zTop = atm[iFixedTop].z;

        for (int i = 0; i < iFixedTop; ++i) {
            const double expected =
                250.0 + (m*GM/Atmosphere::kB) *
                (1.0/(R + atm[i].z)
                 - 1.0/(R + zTop));

            const double error = std::abs(atm[i].T - expected);

            std::cout
                << "z = " << atm[i].z / 1000.0 << " km"
                << "  T = " << atm[i].T
                << " K  expected = " << expected
                << " K  error = " << error
                << " K\n";

            if (error > 1e-4)
                throw std::runtime_error(
                    "Hydrostatic test failed"
                );

            if (!atm[i].check_equation_of_state())
                throw std::runtime_error(
                    "Equation of state failed"
                );
        }

        if (atm[2].T != 250.0 || atm[3].T != 230.0)
            throw std::runtime_error(
                "Fixed region was modified"
            );

        std::cout << "Hydrostatic test PASSED\n";
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << '\n';
        return 1;
    }
}
