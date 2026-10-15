# HnG Warmap API (hgapi)

This directory contains the source of the public [raouldeheer/hgapi](https://github.com/raouldeheer/hgapi) project, consolidated as a separate component from the HAGCP packages at the monorepo root.

## What it does

A Node.js / TypeScript HTTP API and WebSocket service for accessing / presenting Heroes & Generals strategic war-map state, battle/player information and static game data. It uses Express and imports HAGCP client/assets/utilities packages through npm package names.

## Important limitations

- This is a **warmap/API service**, not a complete authoritative FPS battle server.
- The imported historical `src/client.ts` calls the original `http://game.heroesandgenerals.com/status` and `settings.js` endpoints via HAGCP's `Client.connectToHQ`. Those endpoints are historical and may be unavailable; no present-day compatibility is implied.
- The service's `/status` route returns 503 if the optional HQ client is not connected.
- The repository has a placeholder test script that exits with an error; don't treat `npm test` returning as a test suite.
- This snapshot is historical. Validate current Node / TypeScript compatibility in an isolated environment.

## Project files

- `src/client.ts`: connects to the historical HQ client and listens for war-map updates.
- `src/app.ts`, `src/api.ts`, `src/api/*`: HTTP/WebSocket application and endpoints.
- `src/hgmap.ts`: serializes map state.
- `src/api/shortestRoute.ts`: strategic route calculations.
- `ecosystem.config.js`: historical PM2 configuration.
- `package.json`, `package-lock.json`, `tsconfig.json`: runtime/dependency/build configuration.
- `LICENSE`: original MIT license text.

## Historical upstream

- Source repository: https://github.com/raouldeheer/hgapi
- Consolidated source commit: `385bb943054a125906ca4494de0066d10197a6e4`
- Upstream MIT license: https://github.com/raouldeheer/hgapi/blob/main/LICENSE

## Local build investigation

From this directory, use a supported Node release and install dependencies in an isolated checkout:

```bash
npm ci
npm run build
```

This is a suggested validation command, **not a claim that the build currently passes**. The project relies on published HAGCP packages and uses old dependency versions. Do not deploy it with real credentials until endpoints, secrets handling and service behavior have been reviewed.
