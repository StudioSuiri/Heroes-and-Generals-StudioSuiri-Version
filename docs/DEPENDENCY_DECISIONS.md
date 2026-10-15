# Dependency Decisions — Native Game

Last researched: 2026-10-15. This is a staged decision record, not a declaration that all libraries have been integrated or built. Every new dependency must be locally built on both designated nodes, version-pinned and added to the licence/provenance inventory.

## Core rule

Prefer one small, maintained library per infrastructure concern over a home-grown framework. But do not add dependencies for hypothetical future features. Keep the first vertical slice lean and measure before adding renderer/physics/profiling systems.

## Decision matrix

| Concern | Current | Decision | Rationale |
|---|---|---|---|
| Language/build | C++20, CMake | KEEP | Shared game/domain types and one project definition for Windows/Linux. |
| Window/input | SDL3 | KEEP | Native app foundation, no browser. Version pin is release-3.4.18, the stable release listed on the official SDL page at audit time. |
| 2D placeholder renderer | SDL_Renderer | TEMPORARY | Fine for protocol and greybox harness; not intended for final 3D FPS presentation. |
| 3D renderer | None | TRIAL: bgfx vs SDL_GPU | bgfx provides D3D11/D3D12/Vulkan/OpenGL/Metal backends and is more of a complete rendering-library choice; SDL_GPU minimizes the number of top-level dependencies but means owning more GPU/shader pipeline code. Compare on both nodes before committing. |
| Custom UDP socket | Hand-written Winsock/POSIX wrapper | TEMPORARY; trial ENet | Current layer has no reliability, congestion control, cryptographic identity or channels. ENet is a smaller established MIT UDP library; use an explicit app protocol on top. If the required security/transport feature set grows, evaluate Valve GameNetworkingSockets, acknowledging its larger crypto/protobuf/build dependency footprint. |
| Physics/collision | Simple radius/ray-like arithmetic only | Start small; trial Jolt later | Add map collision/line-of-sight for infantry first. Jolt is a cross-platform C++ rigid-body/collision engine under MIT; only add when vehicles/rigid bodies justify it. |
| Debug UI | None | Optional Dear ImGui | MIT and official SDL3 platform backend make it useful for developer tools; keep it optional and avoid coupling gameplay state to UI widget state. |
| Audio | None in native target | SDL3 audio first | Already part of the platform stack. Evaluate miniaudio only if SDL3 audio does not meet the needed playback/mixing model. |
| Persistence | In-memory | SQLite later | Embed only when persistence becomes necessary; public-domain core, no separate DB daemon. |
| Unit tests | Hand-written require test runner | KEEP for MVP-0 | No third-party build dependency. Evaluate Catch2 later for fixture/parameterization/reporting if test count grows. |
| Content schema | Hard-coded prototype constants | Data files later | Maps/objectives/weapons should move to versioned data after the rule model stabilizes. Use explicit offline config parsing; never JSON in a per-frame network path. |
| Logs/telemetry | stdout/stderr | Keep small now; structured logs later | Basic startup/join/errors are enough for greybox. Add per-interval metrics; avoid logging each packet/frame. |

## Library references and practical notes

- **SDL3**: [official releases](https://github.com/libsdl-org/SDL/releases), [CMake integration](https://wiki.libsdl.org/SDL3/README-cmake). The release list identified 3.4.18 as the latest stable at this audit. Use SDL3 as the platform/audio layer and do not add SDL2 beside it.
- **bgfx**: [upstream repository](https://github.com/bkaradzic/bgfx), [rendering overview](https://github.com/bkaradzic/bgfx/blob/master/docs/overview.rst). It documents D3D11/12, Vulkan, OpenGL and Metal backends. Build integration is more involved than SDL's simple 2D renderer; run a local spike before choosing it.
- **SDL_GPU**: [SDL3 CMake integration docs](https://wiki.libsdl.org/SDL3/README-cmake). It keeps the renderer close to the existing SDL dependency but requires a GPU pipeline/shader workflow. Avoid assuming it is a drop-in high-level engine.
- **ENet**: [upstream and MIT licence](https://github.com/lsalzman/enet). A candidate for reliable/unreliable UDP channels and fragmentation; it is not a whole matchmaking/account/anti-cheat system. Pin a specific commit/tag and review project activity before adoption.
- **Valve GameNetworkingSockets**: [upstream](https://github.com/ValveSoftware/GameNetworkingSockets), [build dependencies](https://github.com/ValveSoftware/GameNetworkingSockets/blob/master/BUILDING.md). More capabilities, including robust message delivery and encryption options, but documented build dependencies include crypto and protobuf (and some features involve further optional dependencies). Evaluate if requirements justify that weight.
- **Jolt Physics**: [upstream and MIT licence](https://github.com/jrouwe/JoltPhysics). Good candidate for later vehicle collision/rigid bodies; not required to make the first infantry loop.
- **Dear ImGui**: [upstream and MIT licence](https://github.com/ocornut/imgui), [SDL3 backend](https://github.com/ocornut/imgui/blob/master/backends/imgui_impl_sdl3.cpp). Recommended for debugging tools first, not a requirement for in-game HUD.
- **miniaudio**: [upstream](https://github.com/mackron/miniaudio). Small single-file cross-platform audio implementation with no external package dependency; an alternative, not a second required audio system.
- **SQLite**: [official licence page](https://www.sqlite.org/copyright.html). Public-domain engine; introduce only when a persisted data model is needed.

## Adoption gates

A dependency is not “chosen” until:
1. its licence/provenance is recorded;
2. a specific tag or commit is pinned (not a floating branch);
3. Linux and Windows local build scripts can acquire/build it in an opt-in fashion;
4. it meets the feature/performance need in a small test harness;
5. existing code replaced by it is either removed from the product path or left explicitly as reference.
