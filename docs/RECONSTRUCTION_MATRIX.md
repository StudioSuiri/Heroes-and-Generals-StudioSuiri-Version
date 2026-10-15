# Reconstruction Matrix — What the public repositories actually provide

Audit date: 2026-10-14

This matrix separates evidence in the public forks from assumptions about the modern community restoration. “Present” means a source component exists in the audited public tree; it does **not** mean the component is complete, tested, secure, or compatible with HnG Group/Sunrise.

## Component inventory

| Subsystem | Public evidence | Status | Assessment / next work |
|---|---|---|---|
| Native packet-codec hardening | `native/src/core/protocol.cpp`, `native/tests/core_tests.cpp` | Source added; tests unrun | Shared codec enforces the payload shape for Hello/Goodbye/Welcome/Input/Snapshot, and test source covers malformed shape and wrap-safe sequence logic. Validate locally before marking green. |
| Clean-room native prototype | native/CMakeLists.txt, native/src/core, native/src/client and native/src/server | Source scaffold present; not yet compiled/tested | Independent C++20/SDL3 client and headless UDP server, separate from historical runtime. Follow docs/NODE_AGENT_HANDOFF.md and validate locally before expanding. |
| Code catalog | `docs/CODE_CATALOG.md` | Documented | Maps native source, legacy TypeScript packages and the historical API/launcher to their purpose and reuse boundary. |
| Reuse/rewrite/removal audit | `docs/REUSE_AND_REWRITE_AUDIT.md` | Documented | Identifies indispensable research, code to keep/adapt, archival-only components, known unsafe legacy code, and modern library candidates. No candidate is integrated without a local trial. |
| Dependency decision record | `docs/DEPENDENCY_DECISIONS.md` | Documented | SDL3 remains active; renderer and network transport are explicit trials; Jolt/SQLite/ImGui/audio additions are staged by need. |
| Native technical debt | `docs/TECH_DEBT_REGISTER.md` | Static review; native target unbuilt | Tracks transport security, test fixture, renderer, collision, timing, and session risks with local acceptance criteria. |
| Legacy API test script | `components/hgapi/package.json` | Corrected in source; local check pending | No test suite exists. `npm test` now exits nonzero to avoid a false green, and the `post-deploy` helper was removed. |
| Network packet schema and parsers | `packages/hagcp-network-client/src/protos/*.proto`, protocol classes and linking code | Present, historical | Strong research foundation. Add packet fixtures and version-tag the protocol assumptions. |
| TCP client transport | `packages/hagcp-network-client/src/client.ts`, `socket.ts` | Present, historical | Depends on old HQ discovery/settings endpoints. Put endpoint configuration behind an adapter and test against captured fixtures, not the live public internet. |
| Experimental TCP server | `packages/hagcp-network-client/src/server.ts` | Present, incomplete | Explicit testing-only login; uses `password = username`, hard-coded/synthetic account state and stubbed handlers. Not safe or complete enough for internet deployment. |
| Game/strategic template data | `packages/hagcp-assets/assets`, `json`, map-related data | Present, rights/versions need review | Data enables map, entity and template analysis. Code license does not automatically license original game content/artwork. Preserve attribution/provenance and avoid redistributing unclear assets. |
| Map drawing/conversion | `packages/hagcp-canvas`, `packages/hagcp-scripts` | Present, historical | Useful for strategic-layer tooling; validate generated maps against known saves and reference outputs. |
| War-map API | `components/hgapi/src/api/*`, `app.ts`, `client.ts` | Source imported; compatibility unverified | HTTP/WebSocket layer for war-map/battle/player data. It is not the real-time FPS simulation server. Historical HQ endpoints and NPM-published component dependencies are a blocker to a fully local rebuild. |
| Account authentication and password storage | Experimental stub only in the public tree | Missing/unsafe | Must be designed and implemented before a private service can safely support users. Never reuse production credentials with test code. |
| Authoritative player profile/progression persistence | Synthetic sample objects / incomplete handlers | Missing/incomplete | Requires schema, migrations, transactions, validation, backups, and reconciliation tests. |
| Matchmaking / battle allocation | No complete modern implementation identified | Missing / unproven | Need authoritative rules, compatible session protocol, capacity handling and reconnect behavior. |
| Real-time FPS battle simulation / dedicated game server | No complete source established by these three repositories | Missing / unproven | This is the biggest gap. Protocol schemas do not implement physics, weapons, hit validation, replication or game simulation. |
| Launcher / client bootstrap | Historical 2016–2018 C# Windows Forms launcher | Legacy only | Depends on an old client and hard-coded download host. Some resource files are intentionally omitted; this archival component is not a reproducible build. |
| Anti-cheat and moderation | No complete implementation established by these forks | Missing / unproven | Required before public operation; use server-side validation and an abuse/reporting model. |
| Build and automated tests | Network packet/connection tests exist; hgapi test script is a placeholder | One-time Node 18 build/test baseline passed | HAGCP install, build, tests and lint passed once; hgapi compiled once on Node 18. All workflow files were removed so validation and publishing remain local-only by default. Re-run locally after changes. |

## Can these repositories reconstruct a playable private server?

**Not by themselves, based on available evidence.** They are enough to study protocol formats, explore strategic state/map data, build some related client/API utilities and prototype parts of a server. They do not prove the availability of a complete modern, authoritative FPS server implementation.

The most promising incremental target is the **strategic/war-map slice**: run local template/map tooling with recorded fixtures, decouple historical HQ discovery, then build deterministic tests and a documented local service boundary. Separately, request documented architecture or permitted source access from the current restoration maintainers rather than assuming Patreon-funded work is public or licensed for reuse.

## Suggested execution order

1. Make builds deterministic and run tests locally; catalogue every failing test and obsolete dependency.
2. Put historical endpoint URLs behind explicit configuration, defaulting to no network access in tests.
3. Add protocol test fixtures for login challenge, state subscription, map changes, reconnects and version mismatch.
4. Rebuild the warmap API against local workspace packages or pin its package dependencies to the exact audited source commits. Keep it separated from the authoritative game server. Validate locally.
5. Produce an interface/service gap list for authentication, accounts, persistence, matchmaking and battle hosting.
6. Only after confirming lawful access to the necessary client/game assets and server behavior should the team decide between contributing to the existing restoration and writing a clean-room compatible implementation.

## Provenance and uncertainty

Audited repositories:
- https://github.com/raouldeheer/Heroes-and-Generals-community-project
- https://github.com/raouldeheer/hgapi
- https://github.com/l4v4ph0x/priv8hng_sisvalid

The private Patreon/member repository or source distribution, if any, was not accessible in this audit. Therefore this report does not assert what it contains, whether it contains the “real code,” or what reuse rights it grants. Ask its maintainers for an explicit inventory, licence and compatibility matrix before treating it as the missing authoritative backend.
