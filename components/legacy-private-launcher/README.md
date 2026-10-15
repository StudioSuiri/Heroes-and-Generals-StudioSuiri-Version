# Historical private-server launcher (archival source)

This directory holds the source-only subset of [l4v4ph0x/priv8hng_sisvalid](https://github.com/l4v4ph0x/priv8hng_sisvalid), an old Windows Forms launcher associated with a private-server experiment from the 2016–2018 period.

## What this is — and is not

- It is a legacy **launcher**, not a complete game-server implementation.
- It targets .NET Framework 2.0 / Windows Forms and may require a period-compatible Windows/.NET toolchain to build.
- The project does not establish compatibility with HnG Group/Sunrise or the 2023-era restored client.
- The source expects a historical client executable named `sisvali.exe` and contains a hard-coded download base URL in `w_main.cs`. The updater/download flow is not trusted or safe to execute without a separate review.
- Do not launch old precompiled executables, retrieve content from the hard-coded host, or enter credentials into this legacy application.

## What was intentionally excluded

The original public repository had Visual Studio `.vs` cache/SQLite state, compiled `.exe`/ `.pdb`/vshost files, a bundled ZIP, an unzip executable, and build output. Those items are intentionally not copied into this monorepo. Source files, project configuration, solution, resources, and the original GPL-3.0 license are retained.

## Build status

Not built or tested in this consolidation. The project is archival research material only. A build should be done in an isolated Windows VM with network access disabled first, after reviewing all resource and launcher behavior.

## Provenance and license

- Upstream: https://github.com/l4v4ph0x/priv8hng_sisvalid
- Observed upstream head: `3d9a3d76a5dc73e3554c851ea559863920c5595e` (2018-08-26)
- License: GPL-3.0; see [LICENSE](./LICENSE). Keep applicable notices and source availability requirements if distributing modified versions.
