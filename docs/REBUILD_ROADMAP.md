# Native Rebuild Roadmap

The goal is a small playable slice before expanding scope. Each phase is testable independently. No phase requires the old Flash UI, private protocol or proprietary asset bundle.

## Phase 0 — Buildable shared core

Deliver:
- C++20 core library, versioned packet codec and UDP wrapper.
- Headless server and SDL3 client.
- Greybox shapes, movement, aiming, hitscan damage, respawn and one capture zone.
- Local tests and local build instructions.
- No accounts, campaign, vehicles or imported game assets.

Local acceptance:
- Configure/build headless server without SDL.
- CTest covers packet round-trip, malformed packet rejection, snapshot serialization, movement bounds, capture, combat and player capacity.
- A Windows client can join a Linux server over trusted LAN.
- Record OS/compiler/CMake/Git SHA and exact logs.

## Phase 1 — Playability fundamentals

- Decide first-person versus over-the-shoulder camera; the current top-down greybox is a network/gameplay test harness.
- Add magazine, reload, weapon parameters, hit feedback and battle timer.
- Add 3 objectives or a sector line with deterministic attack/defence victory rules.
- Add collision and line-of-sight before advanced ballistics.
- Add snapshot interpolation after measurements and only then consider client prediction/reconciliation.
- Track RTT, packet loss, snapshot rate and tick overruns.

Acceptance: 10-minute LAN match stable with 2–6 clients at 30 server ticks/s; combat/objective decisions remain server-authoritative.

## Phase 2 — Tactical battle loop

- Data-driven map, spawn zones, team tickets, scoring and match results.
- Native menu, host/join, settings and scoreboard.
- Small set of generic rifle/submachine-gun/anti-vehicle test weapons.
- Generic sound/animation hooks without adding dependencies to the simulation core.
- Acceptance: 6–12 concurrent LAN players, no stuck sessions on reconnect, timing metrics captured.

## Phase 3 — Combined arms

- One unarmed transport, then one generic armoured vehicle.
- Server-side movement, seat enter/exit, collision, damage and destruction.
- Tie vehicle deployment to a team resource budget.
- Add line-of-sight, projectile/hitscan choice and explosive damage only with tests.
- Acceptance: infantry remains useful and objectives cannot be dominated by vehicles alone in baseline scenarios.

## Phase 4 — Native campaign

- Territories/cities as graph nodes; supply edges; faction owner and capital metadata.
- Assault teams, resource count, movement orders, battle lock and battle creation.
- Native campaign map within the same standalone application.
- Keep campaign actions/results separate from real-time tactical ticks.
- Acceptance: one deterministic territory advance triggers a battle and its result updates ownership without external HQ services.

## Phase 5 — Persistence and progression

- Add SQLite schema/migrations/backups when profile or campaign state warrants it.
- Store profiles/loadouts and campaign results; validate each client-originated request.
- Add XP/unlocks after battle outcomes and objective contribution are defined.
- Add economy only when its balance rules can be described in tests.
- Acceptance: restart persistence, migrations and restore tested locally.

## Phase 6 — Multiplayer hardening

Before public exposure:
- Authenticated sessions, rate limiting and identity lifecycle.
- Protocol version negotiation and malformed-packet fuzzing.
- Anti-replay, reconnect and packet loss/reordering tests.
- Reliability/congestion policy for critical state changes.
- Server-side anti-cheat validation, moderation, backups and operational metrics.
- Network exposure, secret management and release security review.

The current protocol is a LAN prototype, not public-server safe.

## Performance principles

- Fixed-step server simulation and independent client render loop.
- No network/disk/database I/O in the render loop or simulation hot path.
- Keep allocation bounded when profiling shows it matters.
- Explicit binary encoding, not JSON per frame and not raw structs.
- Snapshot interest filtering once map/player scale requires it.
- Measure before micro-optimizing; avoid SIMD/assembly absent a profile.
- Native UI means no browser process and no Flash runtime.
