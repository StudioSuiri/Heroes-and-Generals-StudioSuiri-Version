# Local-Only Testing and Build Policy

1. Do not add GitHub Actions workflows or hosted test/build jobs unless explicitly authorized by the repository owner.
2. Do not dispatch cloud runners, Codespaces jobs, remote builds or package-publish jobs for routine validation.
3. Run CMake builds, CTest and manual client/server sessions on the designated Windows/Linux nodes.
4. Dependency downloads happen only when the relevant local setup/build script is explicitly run; versions must be pinned.
5. Do not publish packages or releases from this research fork.
6. Record results with commit SHA, machine role, OS, compiler/CMake versions, exact commands and complete output. “Not run” and “passed” are different statuses.
7. Re-run impacted tests locally after code changes. An older green result does not validate new work.
8. Keep node-local build artifacts and logs under ignored build/ or another local evidence folder. Do not commit caches, binaries, secrets, private addresses or keys.

The legacy TypeScript/HAGCP tools and imported warmap API passed a one-time Node 18 source verification before the native work began. This is evidence only for that historical snapshot.

The new native target has not yet been compiled or tested in this session. The designated agent must run the native scripts locally and report the exact results.
