# Heroes & Generals — Community Research Monorepo

This repository consolidates **public, community-authored research and tooling** related to Heroes & Generals. It is not an official Reto-Moto/TLM source release, and it does not contain a verified complete implementation of the modern game backend.

## Status

This is an **initial source consolidation and audit snapshot**, not a release. The original project remains in the repository root; the public `hgapi` source and the historical launcher research source are integrated under `components/`, with provenance and licenses preserved.

### Components

| Path | Role | Upstream | License observed upstream | Status |
|---|---|---|---|---|
| Root packages / `packages/*` | Heroes & Generals Community Project (HAGCP): TypeScript packages, assets/templates, client protocol definitions, TCP socket/protocol tooling, map utilities, and a limited experimental server implementation | [raouldeheer/Heroes-and-Generals-community-project](https://github.com/raouldeheer/Heroes-and-Generals-community-project) | MIT | Historical snapshot; build/tests/lint passed once on Node 18; rerun locally after changes |
| `components/hgapi/` | Warmap API and service that consumes the HAGCP network client and assets packages | [raouldeheer/hgapi](https://github.com/raouldeheer/hgapi) | MIT | Source, manifests, license and component README imported; TypeScript build passed once on Node 18 |
| `components/legacy-private-launcher/` | Historical Windows Forms launcher for an old private-server experiment | [l4v4ph0x/priv8hng_sisvalid](https://github.com/l4v4ph0x/priv8hng_sisvalid) | GPL-3.0 | Source-only archival subset imported; binaries/caches excluded; not a modern dedicated server |

## Important technical distinction

The HAGCP root includes a **network client/protocol model** and a file named `packages/hagcp-network-client/src/server.ts`. That server code explicitly labels authentication as testing-only, derives the password from the username, and contains placeholder/synthetic player data. It must **not** be considered a production-ready backend or exposed to the internet as-is.

The warmap API (`components/hgapi`) is a companion API and map/war-state viewer service. It is not the complete FPS simulation server. The old `priv8hng_sisvalid` repository is a 2016–2018-era launcher project, not the modern HnG 2023 restoration backend.

**Nothing in this monorepo proves that the private Patreon-maintained modern server source is included.** We must not claim compatibility with current HnG Group servers until tested against the exact client/server version and with permission from the maintainers where required.

## Audit and validation

- [Initial audit report](docs/INITIAL_AUDIT.md)
- [Reconstruction readiness matrix](docs/RECONSTRUCTION_MATRIX.md)

**Local-first testing policy:** routine builds and tests should be run in a local checkout. The automatic GitHub Actions test workflows used for the one-time source verification have been removed so pushes and pull requests do not start remote test jobs. See the audit report for the recorded result and known limitations.

From the repository root, run `npm ci`, `npm run build:all`, `npm test`, and `npm run lint`. For the imported API, run `cd components/hgapi`, then `npm ci` and `npm run build`. These commands are instructions for local validation, not a claim that every runtime integration has been verified.

## Upstream repositories and provenance

- [HAGCP upstream](https://github.com/raouldeheer/Heroes-and-Generals-community-project)
- [HAGCP MIT license](https://github.com/raouldeheer/Heroes-and-Generals-community-project/blob/main/LICENSE)
- [hgapi upstream](https://github.com/raouldeheer/hgapi)
- [hgapi MIT license](https://github.com/raouldeheer/hgapi/blob/main/LICENSE)
- [Historical private launcher upstream](https://github.com/l4v4ph0x/priv8hng_sisvalid)
- [GPL-3.0 license text](https://github.com/l4v4ph0x/priv8hng_sisvalid/blob/master/LICENSE)
- [HnG Group](https://www.hng-group.com/) — separate community restoration and server operator

The component directories retain their own license files. Copyright notices and licensing conditions must be preserved when redistributing or modifying their code. Assets in the HAGCP package may describe or contain game data/artifacts whose rights are distinct from the MIT-licensed source code; do not infer that the MIT license grants rights to proprietary Heroes & Generals artwork or game content.

## Initial audit findings

1. The HAGCP snapshot is a multi-package TypeScript/npm workspace. It includes protocol `.proto` files, packet parsers/linking code, asset/template data, map conversion/rendering utilities, scripts and tests. Last observed upstream commit in this fork: `0e0ce4de7ef5919b14bc007f22384092d2fdf313` (2023-12-14).
2. The hgapi snapshot is a Node/TypeScript Express service with warmap, battle, player and static information endpoints and WebSocket support. Its client attempts to read `http://game.heroesandgenerals.com/status` and `settings.js`, i.e. historical official endpoints that may no longer exist.
3. The legacy launcher contains old compiled binaries, PDBs, archives and Visual Studio cache files upstream. The consolidated archival component excludes those generated artifacts and retains source/configuration only; some referenced resources remain intentionally absent.
4. A one-time Node 18 source verification passed the HAGCP build/tests/lint and the hgapi TypeScript build. This is a baseline for these sources, not proof of live-server compatibility. Dependency versions are old and changes should be validated locally in an isolated environment.
5. A private instance requires a compatible implementation of login/account services, player persistence/progression, matchmaking, battle orchestration and game-server simulation. This source set does not establish that all those pieces are present.

## Recommended next steps

1. Finish the source consolidation and keep licenses/provenance at component boundaries.
2. Inventory dependencies, build scripts, tests, network endpoints and external data assumptions.
3. Disable/de-risk legacy endpoints before running anything; never use real passwords or expose the experimental server code.
4. Run isolated builds/tests and record exact failures.
5. Compare needed services with the publicly documented HnG Group project architecture; ask maintainers before assuming their private backend can be copied or deployed.
