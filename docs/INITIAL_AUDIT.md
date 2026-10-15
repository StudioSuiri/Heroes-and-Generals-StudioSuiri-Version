# Initial Audit — Heroes & Generals Sources

Date: 2026-10-14

## Repositories examined

| Repository | Forked from | Default branch / latest observed source commit | Files in recursive tree | Finding |
|---|---|---|---:|---|
| [Heroes-and-Generals-community-project fork](https://github.com/StudioSuiri/Heroes-and-Generals-StudioSuiri-Version) | [raouldeheer/Heroes-and-Generals-community-project](https://github.com/raouldeheer/Heroes-and-Generals-community-project) | `main`, `0e0ce4de7ef5919b14bc007f22384092d2fdf313`, 2023-12-14 | 236 file blobs | Main base: npm/TypeScript multi-package repository with assets, network protocol/client code and helper scripts. MIT. |
| [hgapi fork](https://github.com/StudioSuiri/hgapi-StudioSuiri-Version) | [raouldeheer/hgapi](https://github.com/raouldeheer/hgapi) | `main`, `385bb943054a125906ca4494de0066d10197a6e4`, 2024-01-24 | 30 file blobs | Companion Express/TypeScript API and war-map service. MIT. The package depends on published HAGCP packages and historically uses original HnG HQ endpoints. |
| [priv8hng_sisvalid fork](https://github.com/StudioSuiri/priv8hng_sisvalid-StudioSuiri-Version) | [l4v4ph0x/priv8hng_sisvalid](https://github.com/l4v4ph0x/priv8hng_sisvalid) | `master`, `3d9a3d76a5dc73e3554c851ea559863920c5595e`, 2018-08-26 | 41 file blobs | Historical C# Windows Forms launcher and configuration for an old private-server experiment. GPL-3.0. Not evidence of a modern server implementation. |

## Findings with highest engineering impact

### 1. The root HAGCP repository is the correct base

It has the broadest set of reusable code: protocol definitions and parsers, TCP socket handling, a network client, an experimental `Server` implementation, asset templates, map-data tools and tests. It is therefore the natural monorepo root.

### 2. The API is complementary, not the game server

The hgapi source implements HTTP endpoints and map-related APIs and reads war state through the HAGCP client. It includes route calculation, battle and player endpoints, cache helpers, logging, map export and websocket support. It should live as a separately deployable service, not be confused with the authoritative FPS battle simulation.

### 3. A dangerous experimental server exists — but it is not production-ready

In `packages/hagcp-network-client/src/server.ts`, the code comments explicitly call the login implementation testing-only. It sets `password = username`, emits synthetic player state, returns hard-coded values for ban/queue checks, and contains placeholder logic. This can support protocol research, but it is not safe or feature-complete for public deployment. Never expose it to the internet without replacing authentication, authorization, persistence and request validation.

### 4. Historical launcher requires source-only treatment

The old fork contains Visual Studio `.vs` caches and SQLite IDE state, compiled `.exe` and `.pdb` files, `.zip` bundles, and an old `unzip.exe`. They are excluded from consolidation. The C# source references a hard-coded legacy download host (`35.204.221.98:8080`) and launches `sisvali.exe`; treat its updater/download mechanism as unsafe until independently reviewed. Do not run the historic binaries or download from the hard-coded host.

### 5. What the repositories do not prove

These public repositories do not establish availability of the current HnG Group/Sunrise modern server source, a complete dedicated FPS server, a working modern account/progression service, or a private-server package compatible with the modern restored client. The experimental server file is a starting point for protocol study only.

## Consolidation result on branch `audit/consolidate-hng-sources`

- Base selected: HAGCP root repository.
- Added `components/hgapi/`: 17 TypeScript source files, package manifest/lockfile, tsconfig, PM2 config, .gitignore, MIT license and explanatory README.
- Added `components/legacy-private-launcher/`: historical C# source, project/solution, resources, settings, .gitignore, GPL-3.0 license and archival README. Precompiled executables, PDBs, zip, unzip utility and Visual Studio `.vs`/SQLite state were excluded.
- The recursive Git tree at consolidation commit `f2aec9c812d57a59ad035d6a3d8fb61a46f44152` contained 313 entries, including 50 component-directory entries (41 files and 9 directories).
- Each addition was committed on `audit/consolidate-hng-sources`; no changes were force-pushed to the upstream authors' repositories.
- One-time source verification passed on Node 18 using a standard GitHub-hosted Ubuntu runner (run [38681435040](https://github.com/StudioSuiri/Heroes-and-Generals-StudioSuiri-Version/actions/runs/38681435040)): HAGCP dependency install, `build:all`, tests and TypeScript lint passed; the imported `hgapi` component's `npm ci` and TypeScript build also passed.
- A Node 20 test was attempted and failed during install: the legacy `canvas@2.11.0` dependency has no matching prebuilt binary for the Node 20 ABI and the runner lacked native Pixman headers/libraries. Node 20 compatibility is not established.
- To honor the local-only testing policy, all GitHub Actions workflow files have now been removed from both `main` and the consolidation branch, including the inherited manual package-publishing workflow. Future pushes, pull requests and manual dispatches from this repository therefore cannot start those repository workflows. Routine validation should be run locally with the commands in the root README.
- GitHub showed 31 historical workflow runs during this cleanup; all had completed and none remained queued or active. A portion of the earlier failures were caused by the retired `actions/cache@v2.1.7`, lint findings fixed during the audit, and the Node 20 native `canvas` dependency issue. The source baseline passed on Node 18 as recorded above.

See [RECONSTRUCTION_MATRIX.md](RECONSTRUCTION_MATRIX.md) for a per-subsystem present/missing table and [LOCAL_TESTING_POLICY.md](LOCAL_TESTING_POLICY.md) for the local-only validation rules. The one-time build/test result validates that historical source snapshot only; it does not prove modern server compatibility or readiness for public deployment.

## Consolidation policy

- Preserve original upstream links and commit IDs.
- Keep original license text and component attribution.
- Keep each component separately identifiable and independently buildable where possible.
- Do not copy binary builds, IDE cache, old downloaded game files or private user data.
- Do not combine proprietary game assets into the MIT/GPL licensing claims.
- Treat the one-time green run as a source/build baseline only, not proof of modern-client compatibility, security, or production readiness. Future tests are local by default.


## Native reimplementation — second audit phase (2026-10-14)

The project direction is now a clean-room, native standalone game rather than an attempt to make the historic backend production-ready.

### Chosen baseline
- C++20 shared gameplay/protocol core.
- SDL3 desktop client; no Flash, browser, HTML overlay, Electron or Tauri.
- Headless UDP server on Linux, with the same code also buildable on Windows.
- Explicit versioned binary protocol, fixed-step authoritative simulation and local CTest target.
- Generic geometric placeholders first; no proprietary game art/audio imported.
- CMake build presets and PowerShell/Bash helper scripts for local checks, builds and staging.
- No GitHub Actions workflow files; local-only validation is the required policy.

### Source scaffold and known gap
The initial native scaffold adds packet framing/validation, a simple world snapshot, movement, aim, hitscan damage, health, death/respawn and one capture zone. It also includes client/server entrypoints and core tests for serialization, movement, capture, combat and capacity. The client currently draws a top-down greybox arena; it is a development harness, not yet a finished first-person shooter.

**Native status: not yet built or tested.** The next designated agent must compile and run CTest locally on Linux and Windows and return full output. Do not claim success before that happens. Build outputs stay in ignored build/ and no remote test jobs are allowed.

### What remains out of scope
Collision/cover, first-person camera, ammunition/reload, weapon data, objective modes and match timers, spawn selection/tickets, vehicles, native campaign map, assault teams, profiles/persistence, progression/economy, sound/animation, secure authentication, anti-cheat and public-hosting hardening.

See [NATIVE_DESIGN.md](NATIVE_DESIGN.md), [MECHANICS_CATALOG.md](MECHANICS_CATALOG.md), [REBUILD_ROADMAP.md](REBUILD_ROADMAP.md), [DEPENDENCY_SETUP.md](DEPENDENCY_SETUP.md), and [NODE_AGENT_HANDOFF.md](NODE_AGENT_HANDOFF.md) for design, mechanics, setup and acceptance path.
