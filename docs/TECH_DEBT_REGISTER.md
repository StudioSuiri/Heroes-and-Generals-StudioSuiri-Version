# Technical Debt Register

Last static review: 2026-10-15. Status reflects source inspection only. No local compiler or CTest result is implied.

Severity:
- **P0** — must be addressed before any external/public server.
- **P1** — must be addressed before describing the native prototype as a stable playable build.
- **P2** — important before adding a major gameplay system.
- **P3** — acceptable for a greybox/research archive.

| ID | Priority | Area | Finding | Action / exit condition | Status |
|---|---|---|---|---|---|
| HNG-N-001 | P1 | Native build | New C++ sources have not been built on Linux/Windows. | Run local build scripts and CTest on both nodes at the same Git SHA; log compiler/CMake versions and full output. | Open |
| HNG-N-002 | P1 | Native client | Current scene is a top-down SDL 2D greybox, not an FPS renderer/camera. | Preserve it as a network harness; select and locally spike bgfx vs SDL_GPU before adding 3D terrain. | Open |
| HNG-N-003 | P1 | Test fixtures | Static inspection found the combat test moved the target beyond rifle test range. The fixture has been adjusted; it has not been run. | Confirm damage assertion passes locally, then add range-boundary and lethal/respawn cases. | Awaiting local validation |
| HNG-N-004 | P1 | Transport | Custom UDP wrapper provides datagram send/receive but no mature reliability, congestion control, authenticated identity or event channels. | Trial ENet behind a transport interface; use separate reliable/unreliable channels and test loss/reordering locally. Do not public-host the current protocol. | Open |
| HNG-N-005 | P1 | Simulation | No collision map, line-of-sight or wall/cover handling; hitscan checks only an aiming corridor. | Add a collision world and deterministic line-of-sight tests before tactical maps/cover. | Open |
| HNG-N-006 | P1 | Server health | Input validation is basic and there is no per-peer packet rate limit or robust session authentication. | Add per-peer budgets, malformed-input flood tests, reconnect semantics and auth before any untrusted network. | Open |
| HNG-N-007 | P2 | Client connection state | Client liveness/reconnection presentation is minimal; it retries Hello but does not expose measured RTT or robust packet-health status. | Add heartbeat/last-packet timeout and bounded telemetry after basic connection correctness is tested. | Open |
| HNG-N-008 | P2 | Snapshot handling | Current snapshots send whole world state every tick to every peer. | Keep for <=32 test players; later add interest filtering and/or snapshot deltas only after bandwidth measurements. | Deferred |
| HNG-N-009 | P2 | Simulation timing | The loop uses a fixed per-iteration dt and resets schedule after an overrun. It avoids a huge catch-up step but simulation time will slow if the host continually overruns. | Count tick overruns and measure 30 Hz stability; define bounded catch-up or deliberate slow-time policy. | Open |
| HNG-N-010 | P2 | Session lifecycle | UDP endpoint is treated as session identity and player IDs are predictable. | Keep LAN-only; use assigned session token/transport peer identity when moving beyond prototype. | Open |
| HNG-L-001 | P1 | Legacy API tests | The component's test script previously printed “no test specified” and exited 0, falsely suggesting tests passed. | Confirm locally that the new explicit placeholder script exits nonzero; add API tests only if this historical service ever remains operational. | Source corrected; local check pending |
| HNG-L-002 | P1 | Legacy endpoint | API client references historical HQ /status and settings.js endpoints. | Keep behind explicit config only if researching; native game must not depend on these endpoints. | Archive-only |
| HNG-L-003 | P1 | Legacy server | HAGCP server.ts is a testing stub with insecure password behavior and synthetic data. | Do not deploy or expose; retain only for protocol research. | Archive-only |
| HNG-L-004 | P2 | Package script | API package had post-deploy command that ran npm install and tsc. | Keep that lifecycle-style helper out of the archival package; native deployment uses local CMake artifacts. | Removed from source; local metadata check pending |
| HNG-L-005 | P2 | Native dependency isolation | Legacy canvas@2.11.0 and Node stack should not be pulled into native build. | Preserve separate components/ boundary; no npm dependency from native CMake. | Resolved by architecture |
| HNG-L-006 | P2 | Legacy launcher | Historical launcher refers to a hard-coded download host and omitted assets in its source-only copy. | Treat as archive; never run its updater or claim reproducible build. | Archive-only |
| HNG-D-001 | P2 | Content licensing | Historical source/assets have different rights; code licence is not an art licence. | Maintain asset provenance manifest and use permissive generic assets until rights are reviewed. | Open |
| HNG-D-002 | P2 | Build reproducibility | CMake presets/scripts have only been statically inspected here. | Validate Linux, Windows generator paths and optional SDL3 fetch locally. | Open |
| HNG-D-003 | P3 | Test framework | Current simple test runner has limited reporting/fixtures. | Keep while small; consider Catch2 when parametrized/integration tests justify another dependency. | Deferred |

## Required invariant

No row moves to “verified” because source was added. It must have evidence from the designated local node, including Git SHA, OS/compiler versions, command line, and full success/failure output. Do not create cloud workflows for this register; update it from local agent reports.
