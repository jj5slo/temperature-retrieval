import sys
import math

import numpy as np
import sasktran2 as sk


def calculate_radiance(values):
	if len(values) != 4:
		raise ValueError(
			"Usage: calculate_radiance: "
			"TANGENT_ALT OBSERVER_ALT COS_SZA REL_AZIMUTH"
		)
	if not all(math.isfinite(v) for v in values):
		raise ValueError("Non-finite geometry parameter")

	(
		tangent_altitude_m,
		observer_altitude_m,
		cos_sza,
		relative_azimuth_rad,
	) = values

	if not (0.0 <= tangent_altitude_m < observer_altitude_m):
		raise ValueError("Invalid tangent/observer altitudes")
	if not (-1.0 <= cos_sza <= 1.0):
		raise ValueError("Invalid cos_sza")
	if not (0.0 <= relative_azimuth_rad <= math.pi):
		raise ValueError("Invalid relative azimuth")

	# Calculation settings
	config = sk.Config()
	config.multiple_scatter_source = (
#		sk.MultipleScatterSource.DiscreteOrdinates
		sk.MultipleScatterSource.SuccessiveOrders
	)
	# TODO
#	config.num_streams = 8# TODO
#	config.num_sza = 3
#	config.num_threads = 1

	# Model geometry
	model_geometry = sk.Geometry1D(
		cos_sza=cos_sza,
		solar_azimuth=0.0,
		earth_radius_m=6370e3,
		altitude_grid_m=np.arange(0.0, 100001.0, 1000.0),
		interpolation_method=sk.InterpolationMethod.LinearInterpolation,
		geometry_type=sk.GeometryType.Spherical,
	)

	# Viewing geometry
	viewing_geometry = sk.ViewingGeometry()
	viewing_geometry.add_ray(
		sk.TangentAltitudeSolar(
			tangent_altitude_m=tangent_altitude_m,
			relative_azimuth=relative_azimuth_rad,
			observer_altitude_m=observer_altitude_m,
			cos_sza=cos_sza,
		)
	)

	# Atmosphere: US76 + Rayleigh
	atmosphere = sk.Atmosphere(
		model_geometry,
		config,
		wavelengths_nm=np.array([470.0]),
		calculate_derivatives=False,
	)

	sk.climatology.us76.add_us76_standard_atmosphere(atmosphere)
	atmosphere["rayleigh"] = sk.constituent.Rayleigh()

	# Radiative transfer
	engine = sk.Engine(config, model_geometry, viewing_geometry)
	result = engine.calculate_radiance(atmosphere)

	radiance = float(
		result["radiance"].isel(
			wavelength=0,
			los=0,
			stokes=0,
		).item()
	)

	if not math.isfinite(radiance) or radiance <= 0.0:
		raise RuntimeError(
			f"Invalid radiance: {radiance}"
		)

	return radiance
