# Heroes and Generals Community Project — StudioSuiri Audit Fork

This is a fork of [raouldeheer/Heroes-and-Generals-community-project](https://github.com/raouldeheer/Heroes-and-Generals-community-project), a public TypeScript/npm monorepo of community research and tooling for Heroes & Generals.

## What is here

The upstream tree contains npm workspaces under `packages/*`, including:
- `hagcp-network-client`: TCP protocol framing, packet parser/linker, protocol schema files and a client used for historical HQ communication; it also contains an experimental server implementation.
- `hagcp-assets`: serialized template/game-state data and asset files used by client tooling.
- `hagcp-canvas`: map rendering helpers.
- `hagcp-utils`: byte and datastore utilities.
- `hagcp-scripts`: conversion/export and protocol investigation scripts.
- Tests for network connection and packet parsing.

## Important limitations

**This is not the complete source for the modern HnG Group/Sunrise servers.** A client library and protocol schemas do not equal a complete game backend.

The source at `packages/hagcp-network-client/src/server.ts` explicitly marks login as testing-only. It assigns `password = username`, returns synthetic player values, and uses placeholder ban/queue/mission logic. Do not expose that experimental server to the public internet or use real credentials. Authentication, persistence, authorization, matchmaking and authoritative battle simulation are not established as complete.

The network client reads historical endpoints such as `http://game.heroesandgenerals.com/status` and `settings.js`; their current availability is not assumed. The repository has not been verified to build or connect to a currently maintained community server.

## Build / test

This is an old TypeScript/npm workspace (the root manifest says Node >=12). Validate using an isolated checkout and a currently supported Node release. Build dependencies may be old or unavailable.

```bash
npm ci
npm test
npm run lint
npm run build:all
```

These are upstream scripts, not a claim that they all currently pass.

## Source provenance and license

- Upstream: https://github.com/raouldeheer/Heroes-and-Generals-community-project
- MIT license: [LICENSE](./LICENSE)
- Upstream head observed during audit: `0e0ce4de7ef5919b14bc007f22384092d2fdf313` (2023-12-14)
- Unified research branch: [HnG source consolidation](https://github.com/StudioSuiri/Heroes-and-Generals-StudioSuiri-Version/tree/audit/consolidate-hng-sources)

Source code being MIT-licensed does not automatically grant rights to proprietary game art, names, or other game assets. Review asset provenance and applicable rights before redistributing them.
