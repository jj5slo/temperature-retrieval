
import numpy as np
import sasktran2 as sk
from importlib.metadata import version

print("SASKTRAN2:", version("sasktran2"))

# Geometry parameters
EARTH_RADIUS_M = 6370e3
OBSERVER_ALTITUDE_M = 42164e3 - EARTH_RADIUS_M
TANGENT_ALTITUDE_M = 45000.0

SZA_DEG = 80.0
RELATIVE_AZIMUTH_DEG = 90.0

cos_sza = np.cos(np.deg2rad(SZA_DEG))
relative_azimuth = np.deg2rad(RELATIVE_AZIMUTH_DEG)

# Model Geometry
model_geometry = sk.Geometry1D(
    cos_sza=cos_sza,
    solar_azimuth=0.0,
    earth_radius_m=EARTH_RADIUS_M,
    altitude_grid_m=np.arange(0, 100001, 1000),
    interpolation_method=sk.InterpolationMethod.LinearInterpolation,
    geometry_type=sk.GeometryType.Spherical,
)

# Viewing Geometry
viewing_geometry = sk.ViewingGeometry()

ray = sk.TangentAltitudeSolar(
    tangent_altitude_m=TANGENT_ALTITUDE_M,
    relative_azimuth=relative_azimuth,
    observer_altitude_m=OBSERVER_ALTITUDE_M,
    cos_sza=cos_sza,
)

viewing_geometry.add_ray(ray)

# Output
print("\n=== Geometry Parameters ===")
print("Earth radius [m]:", EARTH_RADIUS_M)
print("Observer altitude [m]:", OBSERVER_ALTITUDE_M)
print("Tangent altitude [m]:", TANGENT_ALTITUDE_M)
print("SZA [deg]:", SZA_DEG)
print("Relative azimuth [deg]:", RELATIVE_AZIMUTH_DEG)

print("\n=== SASKTRAN2 Ray ===")
print(ray)

print("\nGeometry construction PASSED")

