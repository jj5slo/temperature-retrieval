#!/usr/bin/env bash
set -euo pipefail

cmake -S . -B . \
    -G "Unix Makefiles" \
    -DCMAKE_BUILD_TYPE=Release
