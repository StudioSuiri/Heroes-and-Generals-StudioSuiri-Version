# Historical private-server launcher (archival source)

This directory holds a source-focused subset of [l4v4ph0x/priv8hng_sisvalid](https://github.com/l4v4ph0x/priv8hng_sisvalid), an old Windows Forms launcher associated with a private-server experiment from the 2016–2018 period.

## What this is — and is not

- It is a legacy **launcher**, not a complete game-server implementation.
- It targets .NET Framework 2.0 / Windows Forms and may require a period-compatible Windows/.NET toolchain.
- It does not establish compatibility with HnG Group/Sunrise or the 2023-era restored client.
- The source contains a hard-coded historical download base URL in `w_main.cs` and launches a legacy executable named `sisvali.exe`. Treat the updater/download flow as unsafe until separately reviewed.
- Do not execute old binaries, retrieve content from the hard-coded host, or enter credentials into this legacy application.

## Source-only import scope

The C# source, project/solution files, resource XML, settings and original GPL-3.0 license are retained. This consolidation **does not include** the historical compiled EXEs/PDBs, ZIP/unzip binary, Visual Studio `.vs` cache/SQLite state, or unverified external artwork/icon files. Some `.resx` entries in the historical project refer to those omitted files, so **the archival project is not yet a reproducible build**; these references are recorded for future cleanup rather than silently repackaging third-party assets.

## Build status

Not built or tested. Do not assume it compiles in its current archival state. Any further build work should happen in an isolated Windows VM with network access disabled, after reviewing download, updater, process-launch and embedded resource behavior. A future clean-room port should remove the old download host and use clearly licensed placeholder assets.

## Provenance and license

- Upstream: https://github.com/l4v4ph0x/priv8hng_sisvalid
- Observed upstream head: `3d9a3d76a5dc73e3554c851ea559863920c5595e` (2018-08-26)
- License: GPL-3.0; see [LICENSE](./LICENSE). Keep applicable notices and source availability requirements if distributing modified versions.
