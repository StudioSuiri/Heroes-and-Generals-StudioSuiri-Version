#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BUILD="$ROOT/build/linux-server"
STAGE="$ROOT/build/stage/linux-server"
"$ROOT/scripts/dev/build-linux-server.sh"
mkdir -p "$STAGE"
install -m 0755 "$BUILD/hng-server" "$STAGE/hng-server"
cat > "$STAGE/README.txt" <<'EOF'
HnG Native headless server — local/LAN prototype only.
Start: ./hng-server --bind 0.0.0.0 --port 26000 --tick-rate 30
Authentication, protocol security, anti-cheat and public-hosting hardening are NOT implemented.
Only run on loopback or a trusted private LAN.
EOF
printf 'Staged local Linux package at: %s\n' "$STAGE"
printf 'No SSH/scp or remote deployment was attempted.\n'
