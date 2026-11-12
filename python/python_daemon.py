import sys
import math
import traceback

from calculate_radiance import calculate_radiance


def main():
    # Python interpreterとSASKTRAN2は起動時に1回だけ読み込む

    for line in sys.stdin:
        line = line.strip()

        if line == "QUIT":
            break

        try:
            values = [float(x) for x in line.split()]

            if len(values) != 4:
                raise ValueError("Expected 4 parameters")

            if not all(math.isfinite(x) for x in values):
                raise ValueError("Non-finite parameter")

            radiance = calculate_radiance(values)

            if not math.isfinite(radiance):
                raise ValueError("Invalid radiance")

            print(f"{radiance:.17e}", flush=True)

        except Exception:
            traceback.print_exc(file=sys.stderr)
            print("ERROR", flush=True)


if __name__ == "__main__":
    main()

