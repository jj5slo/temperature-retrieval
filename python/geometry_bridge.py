
import sys
import math

import sasktran2 as sk


def main():
	if len(sys.argv) != 5:
		raise ValueError(
			"Usage: geometry_bridge.py "
			"TANGENT_ALT OBSERVER_ALT COS_SZA REL_AZIMUTH_RAD"
		)

	values = [float(v) for v in sys.argv[1:]]

	if not all(math.isfinite(v) for v in values):
		raise ValueError("Non-finite geometry parameter")

	(
		tangent_altitude_m,
		observer_altitude_m,
		cos_sza,
		relative_azimuth_rad,
	) = values

	ray = sk.TangentAltitudeSolar(
		tangent_altitude_m  = tangent_altitude_m,
		observer_altitude_m = observer_altitude_m,
		cos_sza             = cos_sza,
		relative_azimuth    = relative_azimuth_rad,
	)

	viewing_geometry = sk.ViewingGeometry()
	viewing_geometry.add_ray(ray)

	# stdout: machine-readable result only
	print("OK", flush=True)


if __name__ == "__main__":
	main()

