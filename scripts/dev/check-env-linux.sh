#!/usr/bin/env bash
set -euo pipefail
printf '=== HnG Native Linux environment ===\n'
for tool in git cmake ninja c++ ctest; do
  if command -v "$tool" >/dev/null 2>&1; then
    printf '%-8s %s\n' "$tool" "$(command -v "$tool")"
  else
    printf 'MISSING  %s\n' "$tool"
  fi
done
command -v cmake >/dev/null 2>&1 && cmake --version | head -n 1 || true
command -v ninja >/dev/null 2>&1 && ninja --version || true
command -v c++ >/dev/null 2>&1 && c++ --version | head -n 1 || true
printf '\nHeadless server requires no SDL3, browser or display server.\n'
printf 'For native Linux client, install SDL3/X11 or Wayland development prerequisites first.\n'
