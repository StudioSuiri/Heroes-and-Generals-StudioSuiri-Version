# Reuse, Rewrite and Removal Audit

Audit date: 2026-10-15
Scope: public source currently consolidated in this repository, with the native C++ track treated as the active product path.

This is a static code/design audit. It does not claim the native target compiles or that either node has run the tests. Library choices below are recommendations to trial locally, not dependencies already integrated.

## Executive decision

Use the old repositories as **research evidence**, not as the runtime foundation for the new game. The only code path that should become the shipped game is the native C++20 client/server. Preserve the historical repositories with their source provenance and licenses, but prevent them from creeping into the native runtime through package dependencies, old network endpoints, Flash/UI assumptions, or legacy launchers.

Recommended end-state:
- C++20 shared domain/protocol module.
- SDL3 for desktop window/input and audio during early development.
- A modern 3D renderer selected before building a true FPS camera/terrain pipeline.
- ENet as the first candidate to replace the hand-written UDP transport when reliable/unreliable delivery requirements are ready for implementation.
- Server-authoritative fixed-step simulation, with explicit data-driven map, weapon, objective and vehicle rules.
- SQLite only when profile/campaign persistence is actually needed.
- Generic assets with recorded licenses until replacement artwork is authored.

## 1. KEEP — indispensable or high-value material

| Component | Decision | Why it is useful | Conditions |
|---|---|---|---|
| `native/include/hng/*.hpp` and `native/src/core/` | Keep as active prototype | Establishes a single C++ vocabulary for packets, snapshots, inputs and simulation. Server authority is the right boundary. | Current code is a scaffold; compile and validate on Windows/Linux before expanding. |
| `native/src/server/main.cpp` | Keep as a headless host prototype | Tests the intended Linux service shape without SDL. The same target can be compiled on Windows for local tests. | It is not internet-safe. Add input/rate/session limits and a proper transport before public hosting. |
| `native/src/client/main.cpp` | Keep as a greybox harness | Native input/window/render path proves the app does not require Flash or a browser. | `SDL_Renderer` shape drawing is temporary. It is not a real FPS camera or 3D pipeline. |
| `native/tests/core_tests.cpp` | Keep | Cheap, dependency-free local tests are a good foundation for packet and deterministic rule tests. | The combat fixture was statically corrected because the old target movement took it beyond hitscan range. No result is claimed until CTest runs locally. |
| `native/CMakeLists.txt`, presets and local scripts | Keep | One build description for Windows/Linux avoids divergent app setup. | Build directories remain local; do not add cloud workflows. Confirm all presets/scripts on actual nodes. |
| HAGCP `.proto` files, packet classes, parsers and tests | Keep as protocol research only | They may reveal packet shapes, old enum names and state transitions useful for understanding historical behavior. | Do not make the new game speak the old private protocol. Verify version and data rights before extracting any content. |
| HAGCP map converters, `hagcp-scripts` and map utilities | Keep as offline research tools | May help inspect/export permitted datasets or build a comparison notebook. | Not a runtime dependency; isolate scripts that fetch historical endpoints or external files. |
| `components/hgapi/src/` and its MIT license | Keep as historical reference | The code exposes war-map data, routes and state transformation ideas. | Do not carry Express/WebSocket service or old HQ endpoint assumptions into the native match server. |
| `components/legacy-private-launcher/` | Keep only as an archival research artifact | Preserves historical C# source and its GPL-3.0 context for understanding old client launch assumptions. | Never execute old binaries/updaters or fetch from its hard-coded legacy download host. It is not a modern server. |
| Component licenses / upstream SHAs | Keep | Necessary to preserve provenance and distinguish code license from game-data/artwork rights. | Audit every future asset independently. |

## 2. GOOD IDEA, BUT IMPROVE IT

### Protocol framing and serialization
The native codec does several things correctly: explicit little-endian fields, a version byte, a magic value, explicit payload lengths, a datagram cap, per-message payload-shape validation, and no raw struct serialization. That is a reasonable tiny prototype format.

Improve before expanding:
- add tests for every malformed length, invalid type, maximum player count, invalid coordinates, and unknown version;
- represent all messages as documented versioned payloads;
- separate gameplay message types from the transport layer;
- use a reliable channel for join/leave, inventory/loadout changes, results and campaign orders, and unreliable sequenced messages for frequent inputs/snapshots where loss is acceptable;
- add a reconnect/session design, per-peer rate limits, replay/sequence handling and observability;
- fuzz the decoder locally before any external exposure.

**Do not build a generic RPC framework now.** Keep the small explicit schema. Replace only the low-level UDP transport when a vetted library provides reliability and congestion semantics that the project otherwise would need to invent.

### Server simulation
The authoritative simulation model is directionally correct. Input intents travel from client to server; position, health, damage, respawn and capture are server-owned. The code is currently a single file with simple floating-point movement and geometric hitscan.

Improve in this order:
1. write golden tests for movement, boundaries, objective contest, shot cooldown/range, death, respawn and disconnect;
2. add a map/collision model and line-of-sight;
3. move weapon and objective values to versioned data;
4. extract movement/combat/objective/spawn systems only when each has a testable rule boundary;
5. measure tick budget and packet rates before optimizing allocations or math.

Do not introduce an ECS or microservice architecture until entities/rules make the current direct model difficult to maintain. If entity/component complexity grows with vehicles, EnTT is a candidate to evaluate; it is not required for MVP-0.

### Client and UI
SDL3 is the right initial platform layer for a native standalone app and is also capable of input and audio. The current code uses SDL's simple 2D renderer, which is perfect for a network test harness, but it will not be the long-term 3D FPS renderer.

Before adding 3D terrain and animated models, select one rendering path:
- **Preferred trial: bgfx** if the goal is a cross-platform renderer with established D3D11/D3D12/Vulkan/Metal/OpenGL backends. Keep SDL3 for window/input. It is a separate library and will add build/tooling work.
- **Lean alternative: SDL_GPU** if minimizing top-level dependencies matters more than high-level renderer abstractions. It stays in SDL3 but needs a proper shader/asset pipeline.
- Keep `SDL_Renderer` only for greybox/debug screens; do not pile 3D hacks onto it.

Use Dear ImGui with its SDL3 backend for debug tooling (entity view, RTT/tick graphs, spawn controls), not as a requirement for the permanent in-game HUD unless its look and interaction suit the game. Main menu/HUD/campaign map remain native client screens.

### Logging and diagnostics
Current standard output/stderr is adequate while there is one server. Add structured, rate-limited logging and telemetry after measuring actual tick/packet behavior. Do not log every input packet. Add per-interval counters for tick overrun, RTT, loss estimate, bytes sent and active peers.

### Build dependency handling
The CMake/Presets structure is the right direction. Keep dependency downloads opt-in and local. Pin exact releases/commits and record a licence/provenance manifest. Do not have every machine independently pull unpinned `master` or run remote automation on each push.

## 3. REPLACE OR REMOVE FROM THE PRODUCT PATH

The following should remain in the archive for provenance, but must not become native game dependencies:

| Existing code/approach | Decision | Reason |
|---|---|---|
| `packages/hagcp-network-client/src/server.ts` experimental server | Remove from any deployment/runtime plan; retain only for research | The audit identifies testing-only login behavior, password derived from username, synthetic player data and placeholder handler behavior. It is not a usable modern dedicated game server. |
| `components/hgapi/src/client.ts` calls to historical `game.heroesandgenerals.com/status` and `settings.js` | Do not reuse as service discovery | These are historical HQ endpoints and may be unavailable or semantically incompatible. The native server should have explicit local/LAN configuration. |
| `components/hgapi` Express/WebSocket APIs as the game transport | Do not port into the hot path | Web APIs are useful for administrative/dashboard uses, not the latency-sensitive authoritative match loop. No browser/admin service is needed in MVP-0. |
| The old `priv8hng_sisvalid` updater/launcher execution path | Never use in the new launcher | It targets a historical client/external download host, has omitted resource references in this archive and does not establish modern compatibility. |
| Legacy `canvas@2.11.0` in HAGCP map tooling | Keep isolated; do not add to native build | It is a native Node package with system/compiler constraints and is not needed by the C++ game. The prior Node 20 CI attempt demonstrated the setup burden. |
| `hagcp-warmap-api` script `test` that printed “no test specified” and exited success | Corrected in source on 2026-10-15 | The script now emits an explicit “no test suite” message and exits nonzero. This avoids a false green; a local npm invocation is still pending. |
| npm `post-deploy` script doing `npm install && tsc` | Removed from package metadata on 2026-10-15 | It encouraged lifecycle-driven dependency mutation/building and is not part of the native deployment model. A local metadata/build check is still pending. |
| Custom UDP socket implementation as the long-term transport | Temporary only | The current wrapper handles send/receive, but it has no delivery/reassembly/congestion policy, authenticated identity or mature channel semantics. Replace through an adapter after local prototype validation. |
| Original Flash/browser UI and separate launcher flow | Remove from new product | The stated product goal is a single native standalone client with a native campaign map. |

Archive != delete. Keep historical source, license files and provenance, but the new game must not compile/link against the archived TypeScript or C# components.

## 4. FEATURES TO BORROW FROM ESTABLISHED LIBRARIES

The current project manually implements some low-level infrastructure that should not become a permanent home-grown framework. Candidate libraries below are supported by upstream documentation, but must be locally trialed before adoption.

| Concern | Recommended option | Why | Adoption timing / trade-off |
|---|---|---|---|
| Window/input/desktop abstraction | SDL3 | Already selected, cross-platform, native, covers input/window and audio. | Keep now. Pin release 3.4.18 and use installed package or explicitly enabled local fetch. |
| 3D renderer | bgfx; SDL_GPU as lean alternative | bgfx abstracts D3D11/D3D12/Vulkan/GL/Metal; SDL_GPU reduces top-level dependency count and stays close to SDL. | Decide before 3D map/terrain. Prototype one triangle, camera, depth test and textured mesh on both target nodes first. |
| Game networking transport | ENet first candidate; GameNetworkingSockets if more robust features justify its dependency footprint | ENet offers a lightweight established UDP transport with reliable and unreliable packets/channels. Valve GameNetworkingSockets offers richer reliable/unreliable messaging, fragmentation, encryption and P2P/NAT features, but its documented build brings crypto/protobuf and other dependency considerations. | Do not copy library code piecemeal. Add through a narrow transport interface after the current local handshake test is understood. Pin a release or commit and preserve licence. |
| Collision/rigid-body vehicle physics | Jolt Physics | Cross-platform modern C++ physics/collision engine, MIT. | Later, when vehicles/rigid bodies are real requirements. For MVP-0, a small, tested capsule/grid collision system is simpler and cheaper. |
| Debug/developer panels | Dear ImGui (SDL3 platform backend) | Permissive MIT UI library with official SDL3 backend. Great for profiling and simulation controls. | Optional development-only dependency. Not required for game UI or server. |
| Audio playback/mixing | SDL3 audio initially; evaluate miniaudio as an alternative | miniaudio is a small cross-platform single-file C library with no external dependencies; SDL3 also avoids another dependency. | Do not add until shot/reload/objective feedback is ready. Pick one audio stack, not both without a measured need. |
| Persistent profiles/campaign | SQLite | Small embedded DB, public-domain core, no database daemon. | Later phase only. Keep SQL outside the 30 Hz simulation tick. |
| Test framework | Current tiny `require` runner; Catch2 if the suite becomes large | No mandatory third-party dependency today. Catch2 is useful for fixtures, parameterized tests and richer reporting but adds dependency/version work. | Keep current runner for MVP-0; choose Catch2 when tests justify another dependency. |
| Serialization | Keep explicit codec for tiny high-rate messages; consider protobuf/FlatBuffers only for broad durable schemas | Current explicit format is small and predictable. Code-generated schemas help only once message count/versioning pressure justifies their build complexity. | No JSON for the frame path. JSON may be fine for offline map/weapon definitions later. |

Upstream references:
- SDL release list and current stable 3.4.18: https://github.com/libsdl-org/SDL/releases
- SDL CMake integration: https://wiki.libsdl.org/SDL3/README-cmake
- bgfx renderer/backend and platform overview: https://github.com/bkaradzic/bgfx
- ENet source and MIT license: https://github.com/lsalzman/enet
- Valve GameNetworkingSockets and build dependencies: https://github.com/ValveSoftware/GameNetworkingSockets and https://github.com/ValveSoftware/GameNetworkingSockets/blob/master/BUILDING.md
- Jolt Physics and MIT licence: https://github.com/jrouwe/JoltPhysics
- Dear ImGui licence and SDL3 backend: https://github.com/ocornut/imgui and https://github.com/ocornut/imgui/blob/master/backends/imgui_impl_sdl3.cpp
- miniaudio feature/implementation overview: https://github.com/mackron/miniaudio
- SQLite licence: https://www.sqlite.org/copyright.html

## 5. MISSING FEATURES — ideal implementation by subsystem

| Missing subsystem | Ideal implementation | Why this is better than recreating the entire old architecture |
|---|---|---|
| True FPS camera and 3D scene | Renderer backend + explicit camera, scene/map format and asset manifest; generic glTF assets for tests. | Native screen stack, one render loop and a well-defined asset format; no Flash/browser surface. |
| Character collision/cover/LOS | Server-authoritative capsule/world collision over simple map collision data; Jolt considered only if physics complexity grows. | Keeps gameplay deterministic enough to test; avoids elaborate rigid bodies too early. |
| Weapon/loadout | Data-driven weapon definitions; shared hit validation/damage; finite ammo/reload; server-owned cooldown and ammo. | Add many guns by data, not duplicated class hierarchies. |
| Tactical modes | Battle definition and state machine; generic objective graph and victory policy; match timer/tickets. | Encounter, Skirmish and Assault become variants of rules data rather than separate monoliths. |
| Spawn/reinforcement | Spawn points plus server-owned ticket/resource budget and spawn protection. | Supports tactical/resource consequences without reproducing all historical economy rules on day one. |
| Vehicles | Separate vehicle movement/seat/damage systems and shared event/damage data; later Jolt if warranted. | Start with transport, one tank; skip aerodynamics/track simulation until proven necessary. |
| Campaign map | A graph of territory nodes and supply edges, faction owner, assault-team tokens, movement orders and battle events. Render in the SDL3 application. | Recreates strategic gameplay without separate Flash service or browser runtime. |
| Profile/economy | SQLite repositories and schema migrations after battle state is stable; profile/loadout/progression first, campaign resource economy later. | Avoids database latency and premature account/backend work. |
| Network quality | Established transport, sequence discipline, snapshot interpolation, then prediction/reconciliation based on RTT/jitter/loss measurements. | Corrects actual network problems instead of adding guesswork or over-optimizing a trivial LAN loop. |
| Public multiplayer safety | Authentication/session tokens, rate limits, anti-replay, fuzzing, moderation/admin logs, reliable event policy, monitoring and security review. | Current protocol is not safe to expose; this needs a deliberate release gate. |

## 6. Prioritized decision plan

1. **Now:** keep native C++20/SDL3/CMake and greybox. Run the new tests locally and repair compile/runtime failures. No new libraries before that result.
2. **Next:** refactor the custom UDP transport behind a narrow interface and perform a local ENet trial on loopback and LAN. Compare packet loss/reordering behavior and dependency footprint; then decide whether to replace it.
3. **Before a real FPS scene:** make a local renderer spike comparing bgfx and SDL_GPU on the existing Windows and Linux nodes. Do not mix renderer migration with campaign work.
4. **Next gameplay milestone:** collision/LOS, weapon data/reload, multiple objectives, battle state and clean disconnect/reconnect handling.
5. **Then:** generic transport/armour, seats and vehicle damage; consider Jolt only when collision/vehicle needs warrant it.
6. **Campaign:** native territory graph, assault-team movement, resource caps and battle-result feedback loop.
7. **Only after it is fun:** SQLite persistence, profiles, progression, economy, more content.
8. **Before public server:** mature transport/security, fuzzing, rate limits, moderation, backups and operational playtest.

No proprietary artwork/vehicle models/map textures should be imported unless their provenance and reuse rights are explicitly verified. MIT or GPL code licences do not automatically grant permission to redistribute game assets.
