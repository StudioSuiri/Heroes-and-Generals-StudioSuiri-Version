#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$BASH_SOURCE[0]")/../.." && pwd)"
BUILD="$ROOT/build/linux-server"
command -v cmake >/dev/null || { echo "Missing CMake 3.25+." >&2; exit 2; }
command -v ninja >/dev/null || { echo "Missing Ninja." >&2; exit 2; }
cmake -S "$ROOT/native" -B "$BUILD" -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo -DHNG_BUILD_CLIENT=OFF -DHNG_BUILD_SERVER=ON -DHNG_BUILD_TESTS=ON -DHNG_FETCH_SDL3=OFF
cmake --build "$BUILD" --parallel
ctest --test-dir "$BUILD" --output-on-failure
printf '\nServer: %s\n' "$BUILD/hng-server"
printf 'This build and test ran locally on this machine only.\n'
