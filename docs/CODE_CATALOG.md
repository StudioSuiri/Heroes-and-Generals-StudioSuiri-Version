# Code Catalog — What We Have and What It Is For

Audit date: 2026-10-15.
This inventory is based on the tracked source structure/manifests and static inspection. It does not prove runtime compatibility, license rights to game data, or successful builds after the native changes.

## Active native product path

| Path | Contents | Importance | Keep/modify |
|---|---|---|---|
| `native/include/hng/*.hpp` and `native/src/core/` | Establish a single C++ vocabulary for packets, snapshots, inputs and simulation. Server authority is the right boundary. | Essential | Keep. Current code is a scaffold; compile and validate on Windows/Linux before expanding. |
| `native/src/server/main.cpp` | Headless host, join/leave/timeout, fixed tick and world snapshots. | Essential prototype | Keep. Add per-peer rate limits, authenticated/session identity and tick telemetry before untrusted traffic. |
| `native/src/client/main.cpp` | SDL3 input and 2D greybox renderer; connect/retry, sequenced input and state display. | Essential harness | Keep as a placeholder. It is not the long-term FPS renderer. Static changes now reject stale snapshots and detect a stale heartbeat; local verification is pending. |
| `native/tests/core_tests.cpp` | Dependency-free tests of packet codecs, snapshots, movement, capture, damage and player capacity. | Essential | Keep. The combat test positioning was corrected statically; local CTest must confirm it. Add death/respawn and malformed-state tests. |
| `native/CMakeLists.txt` and `native/CMakePresets.json` | Targets/options for shared core, client, server and CTest; Linux and Windows configurations. | Essential | Keep. Validate all presets locally and record both node environments. |
| `native/include/hng/protocol.hpp` | Packet types, input, welcome, player and snapshot data. | Essential | Keep. Treat the fields as a versioned app schema; avoid leaking transport APIs into it. |
| `native/src/core/protocol.cpp` | Explicit little-endian codec, length/version/magic validation, input/welcome/snapshot encoding. | Essential prototype | Keep; add malformed-packet boundary tests and local fuzz coverage. |
| `native/include/hng/udp_socket.hpp` and `native/src/core/udp_socket.cpp` | Custom POSIX/Winsock non-blocking datagram send/receive. | Temporary infrastructure | Replace behind a narrow transport seam after local validation; ENet is first candidate. Current code is not a production transport. |
| `native/include/hng/simulation.hpp` and `native/src/core/simulation.cpp` | Server-authoritative movement, simple geometric hitscan, health, respawn and one capture area. | Essential prototype | Keep rules as the reference vertical slice; split to systems when rules/tests justify it. Add collision/LOS before weapons/vehicles proliferate. |
| `native/src/core/{maps,weapons,vehicles,battle,campaign,persistence,assets}` README placeholders | Expected subsystem boundaries and proposed future filenames. | Planning only | Keep as a map of missing work, but do not treat README placeholders as implemented code. |
| `native/src/client/{input,render,audio,ui}` README placeholders | Input, renderer, sound and screen decomposition plans. | Planning only | Keep. The first real 3D renderer remains a decision, not an integrated dependency. |
| `native/src/server/{auth,matchmaking,persistence,services}` README placeholders | Planned auth, battle directory, persistence and moderation/service boundaries. | Planning only | Keep. None are implemented; do not present this as a public server. |
| `scripts/dev/*` and `docs/NODE_AGENT_HANDOFF.md` | Environment checks, local builds, dependency plan and Windows/Linux commands. | Essential for handoff | Keep local-only. SDL fetching/system package installation occurs only when the local agent explicitly runs the corresponding command. |

## Legacy TypeScript workspace — keep as reference

| Path | Contents and what may be learned | Product decision |
|---|---|---|
| `packages/hagcp-network-client/src/client.ts` / `socket.ts` | Historical TCP client, connection management and messages around the former protocol. | Do not use as the native connection layer. Extract only documented behavior or packet fixtures whose provenance/version are clear. |
| `packages/hagcp-network-client/src/protos/` and `protolinking/` | Historical message schema, packet parsing/linking and enum names. | Valuable for protocol research; not the protocol for HnG Native. |
| `packages/hagcp-network-client/src/server.ts` | Experimental/test server and mock account/player behavior. | Not for deployment. Authentication behavior is unsafe and it is not the modern authoritative FPS backend. Preserve for historical analysis only. |
| `packages/hagcp-assets/assets/`, `json/` and `images/` | Templates/map/game-state data from the HAGCP distribution. | Review individually. Do not assume original game assets/data may be redistributed merely because HAGCP source is MIT licensed. The native test map should use generic content. |
| `packages/hagcp-canvas/` | Node canvas rendering/conversion utilities and a dependency on native Node `canvas@2.11.0`. | Useful for offline map conversion/research; not needed by native renderer. Keep it out of the CMake dependency graph. |
| `packages/hagcp-utils/` | Byte cursor/datastore and small helpers built around old packet/data structures. | May be useful for comparing protocol logic; rewrite the small parts actually needed in C++ rather than introducing a TypeScript/Node runtime into the game. |
| `packages/hagcp-scripts/` | Download/conversion/analysis helpers for maps, captures and protocol inspection. | Archive/offline research tools only. Inspect network paths and file effects before using individual scripts. |
| Root `package.json` and `package-lock.json` | npm workspace scripts/build/test/lint for the historical TypeScript code. | Independent archival toolchain; never a prerequisite for native builds. The prior Node 18 validation is historical evidence for that snapshot only. |

## Historical API/launcher components

| Path | Useful part | Problems/decision |
|---|---|---|
| `components/hgapi/src/api/*` | Route semantics, war-map views, player/static info, possible map graph and shortest-route logic. | The old HTTP/WebSocket service is not the real-time battle server. Reimplement only domain rules that are actually useful; do not carry its Express deployment into the native game. |
| `components/hgapi/src/client.ts` | Shows how old service depended on HQ state and map updates. | Historical `/status` and `settings.js` dependencies are not a viable foundation for the new app. |
| `components/hgapi/package.json` | Dependency inventory (Express, WebSocket wrapper, legacy HAGCP packages, native Node canvas indirectly, utility modules). | Source metadata now has no-op-success test behavior corrected to explicit nonzero and lifecycle-style post-deploy helper removed. Local npm checks remain pending. |
| `components/legacy-private-launcher/` | Old C# Forms launcher, project files, resources and GPL-3.0 notices. | Archive only. Hard-coded old download host and omitted resource references mean it is not a clean/reproducible modern launcher. Never run the external updater. |

## Existing algorithms to reuse versus replace

- **Shortest path over a small supply graph:** the rule is valuable; it does not require carrying the old JS API. Implement a small typed graph/Dijkstra module in C++ with deterministic tests, or evaluate a small graph library only if map size/algorithm breadth grows.
- **Packet serialization:** the current native codec is small enough to own; keep it explicit. Do not reuse historical `BufferCursor` or protobufjs as a runtime dependency.
- **Map drawing:** the older canvas conversion tells us map data can be projected/rendered, but the actual native screen should use SDL3 and a selected 3D renderer.
- **HTTP/WebSocket API:** retain only if we later need an optional admin/telemetry service. It is not part of the gameplay critical path.
- **Physics:** no full physics subsystem exists in the audited legacy repos; use a tested simple collision model first and evaluate Jolt only for vehicle/rigid-body needs.
- **Assets:** reuse generic/licensed assets only after manifest/rights checks. Original game data is not made free by MIT source code.

## What is not currently in the codebase

True FPS camera, 3D renderer, world collision/line-of-sight, reusable weapon/reload model, multiple objective modes, battle timer/tickets, map content format, vehicle/seat simulation, native campaign graph and state transitions, profiles/economy/persistence, auth/matchmaking, robust networking security, anti-cheat, moderation and player-facing native menus beyond the greybox harness.

See `REUSE_AND_REWRITE_AUDIT.md` for the decision matrix and `TECH_DEBT_REGISTER.md` for priority/acceptance criteria.
