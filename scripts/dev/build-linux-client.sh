#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$BASH_SOURCE[0]")/../.." && pwd)"
BUILD="$ROOT/build/linux-client"
command -v cmake >/dev/null || { echo "Missing CMake 3.25+." >&2; exit 2; }
command -v ninja >/dev/null || { echo "Missing Ninja." >&2; exit 2; }
command -v git >/dev/null || { echo "Missing Git for optional local SDL3 fetching." >&2; exit 2; }
cmake -S "$ROOT/native" -B "$BUILD" -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo -DHNG_BUILD_CLIENT=ON -DHNG_BUILD_SERVER=ON -DHNG_BUILD_TESTS=ON -DHNG_FETCH_SDL3=ON
cmake --build "$BUILD" --parallel
ctest --test-dir "$BUILD" --output-on-failure
printf '\nClient: %s\nServer: %s\n' "$BUILD/hng-client" "$BUILD/hng-server"
printf 'SDL3 fetching/building, if needed, occurred locally on this machine.\n'
