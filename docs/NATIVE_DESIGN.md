# Native Reimplementation Design

Status: architecture baseline for the clean-room native project. This design is deliberately separate from the public HAGCP/legacy code paths. It does not assert compatibility with the original client or current HnG Group servers.

## Product goal

Build a minimalist, standalone native Windows client and a headless Linux dedicated server that preserve the useful gameplay loop of Heroes & Generals while replacing the old presentation and service stack with a simpler modern implementation.

The target is not “a perfect clone on day one.” The target is a playable infantry match first, then progressively add the mechanics that made the original distinctive: objective modes, combined arms, campaign map, supply, assault teams and progression. Use generic assets until art provenance and rights are explicitly reviewed.

## Chosen technology baseline

- **Language:** C++20 for shared simulation, protocol, server and client.
- **Build:** CMake 3.25+ with CMake Presets; generated files stay local under build/.
- **Client:** SDL3 native desktop window, input and simple 2D renderer. This is an ordinary standalone executable, not Flash, HTML, Electron or Tauri.
- **Server:** Headless C++ process using a thin Winsock/POSIX UDP wrapper. It can run on Linux without SDL, a display system or browser packages.
- **Network:** Versioned compact binary messages over UDP for the prototype; explicit field serialization rather than sending struct memory. The first protocol is LAN-only and intentionally not production-secure.
- **Tests:** Dependency-free C++ test executable registered with CTest. The operator runs it locally on the designated Windows/Linux node. This repo intentionally has no GitHub Actions workflows.
- **Assets:** Generic geometric placeholders first; later add a small generic art set with recorded licenses and file hashes.
- **Persistence:** No database for MVP-0. SQLite is the preferred simple self-hosted option when durable profile/campaign state is required.

## Process layout

~~~text
Windows desktop
  hng-client (SDL3 window, input, rendering, native menus)
       | UDP, protocol v1
       v
Linux headless host
  hng-server (join/leave, authoritative fixed tick, snapshots)
       |
       +-- hng_core (protocol, simulation, data types, socket wrapper)
       +-- later: battle, campaign, persistence, moderation services
~~~

The same hng-server source should also compile on Windows for one-machine testing. The intended development target is Windows client + Linux server, both built from the same Git SHA. Acceptance begins with loopback, then trusted LAN.

## Gameplay scope

### MVP-0 (the scaffold in this repository)

- Two prototype factions and a 1600x900 greybox arena.
- One infantry archetype with generated shapes.
- Movement, aiming, a basic hitscan, health/damage, death/respawn.
- One neutral capture zone with contest and decay behavior.
- A fixed tick simulation owned by the server.
- Client inputs and server snapshots over UDP.
- Local unit tests for wire parsing, snapshot validation, movement, capture, combat and player caps.

Not yet represented: first-person camera, collision/cover, ammunition and reload, sound, animation, several objectives, match timer, scoring, vehicles, campaign, account/profile, persistence, anti-cheat and secure public hosting. The current camera/rendering is a simple arena harness; do not mistake it for the complete FPS.

### Authority boundary

Server owns:
- Assigned player IDs and faction assignment.
- Simulation tick, world position and bounds.
- Damage, health, kill/death count and respawn.
- Capture progress, and later team tickets, vehicle state, score, victory and rewards.

Client owns:
- Keyboard/mouse sampling, window, rendering, local audio, visual effects.
- Later local prediction/interpolation only as a rendering aid.

Client must never decide authoritative position, hit results, capture state, resource budgets or rewards.

## Protocol and performance design

1. Fixed-step server at 30 Hz initially; player rendering runs independently.
2. Sequence-numbered inputs; older/replayed inputs are ignored.
3. Server computes movement and damage from input intent; clients don't send claimed positions or damage.
4. Snapshot fields are explicitly encoded and validated; malformed/oversized packets are discarded.
5. Datagram target is <=1200 bytes to reduce fragmentation risks.
6. Add snapshot interpolation after the basic LAN round trip works; add prediction/reconciliation only after measurements show input latency requires it.
7. Measure RTT, tick duration/overruns, dropped packets, snapshot size/rate, allocations and CPU. Optimize the largest measured bottleneck first.
8. No JSON, database queries, disk I/O or per-frame verbose logs in the tick hot path.
9. No microservices, Redis, Kubernetes, client anti-cheat SDK or assembly optimization at MVP-0.
10. The current UDP protocol has no authentication, encryption, robust congestion control, reliable critical-event stream or public-hosting hardening. Keep it on loopback/trusted LAN until those exist.

Removing the browser/Flash interface removes a runtime boundary and makes the campaign UI native. It cannot by itself prove that historical network lag is solved. Network, simulation and rendering delay must be measured independently.

## Asset and UI strategy

Current output uses SDL-drawn shapes. Later native screens will include main menu, connect-to-server, settings, soldier selection, battle HUD, scoreboard/loadout, and a native campaign map. Campaign nodes/edges should be rendered by the same SDL/native presentation path, not an embedded browser.

The generic first pack can include a soldier silhouette, terrain/road/cover tiles, crosshair/icons, and one generic transport. Track the license, attribution and hashes of every imported file. Do not copy original art, audio or game data without a rights review.

## Data and extensibility

Use data files for map/objective layout, weapon tuning, soldier archetypes, vehicles and game mode rules. Keep IDs stable and schema-versioned. Use simple explicit structs while the domain is small; split systems only when there are real rules and tests to separate. Avoid framework-heavy abstractions before there are multiple implementations.

The initial campaign model should be a small graph: territory nodes, supply edges, faction ownership, assault-team tokens, movement orders, resource budgets and battle-result events. Do not copy all historical economy formulas upfront.

## Security and release gates

The current prototype is not internet-safe. Before a public community server:
- authenticated session identity and reconnect lifecycle,
- rate limits and input validation,
- malformed packet/fuzz testing,
- encryption/identity design and a reliable message policy for critical actions,
- anti-cheat and server-side physics/hit validation,
- moderation/admin audit, backups and secret management,
- operational metrics and a patch process.

## Definition of “native standalone”

- No browser executable required for main menu, match, scoreboard or strategic map.
- Windows user launches one ordinary game executable; SDL3 may be shipped as a normal library beside it if using its shared build.
- Linux host runs a headless game server with no graphics packages.
- Shared C++ core and explicitly versioned network format are used by both.
- Generic assets can be replaced later without changing protocol or campaign rules.
