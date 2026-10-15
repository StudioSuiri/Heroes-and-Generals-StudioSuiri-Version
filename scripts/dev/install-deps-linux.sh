#!/usr/bin/env bash
set -euo pipefail

MODE="plan"
PROFILE="server"
for arg in "$@"; do
  case "$arg" in
    --install) MODE="install" ;;
    --server) PROFILE="server" ;;
    --client) PROFILE="client" ;;
    --help)
      cat <<'EOF'
Usage: ./scripts/dev/install-deps-linux.sh [--server|--client] [--install]
Default behavior only prints the package plan. Pass --install to run apt-get.
--server: C++20 toolchain, CMake, Ninja and Git; no graphics dependencies.
--client: also install common SDL3 native build prerequisites on Ubuntu/Debian.
EOF
      exit 0
      ;;
    *) echo "Unknown argument: $arg" >&2; exit 2 ;;
  esac
done

if ! command -v apt-get >/dev/null 2>&1; then
  echo "This helper targets Debian/Ubuntu apt-get. Install equivalent packages for your distro." >&2
  exit 2
fi

PACKAGES=(build-essential cmake ninja-build git pkg-config)
if [[ "$PROFILE" == "client" ]]; then
  PACKAGES+=(libasound2-dev libpulse-dev libx11-dev libxext-dev libxrandr-dev
    libxcursor-dev libxfixes-dev libxi-dev libxss-dev libxkbcommon-dev
    libwayland-dev wayland-protocols libdecor-0-dev libdrm-dev libgbm-dev
    libegl1-mesa-dev libgles2-mesa-dev libudev-dev libdbus-1-dev
    libibus-1.0-dev libpipewire-0.3-dev)
fi

printf 'Profile: %s\n' "$PROFILE"
printf 'Packages: %s\n' "${PACKAGES[*]}"
if [[ "$MODE" == "plan" ]]; then
  printf '\nDry-run only. Review the package list; to install locally run:\n  %s --%s --install\n' "$0" "$PROFILE"
  exit 0
fi
if [[ "$(id -u)" -ne 0 ]] && ! command -v sudo >/dev/null 2>&1; then
  echo "Need root or sudo to install packages." >&2
  exit 2
fi
if [[ "$(id -u)" -eq 0 ]]; then
  apt-get update
  apt-get install -y "${PACKAGES[@]}"
else
  sudo apt-get update
  sudo apt-get install -y "${PACKAGES[@]}"
fi
printf '\nDependencies installed locally. No project build/test was started.\n'
