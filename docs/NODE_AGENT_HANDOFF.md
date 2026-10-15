# Node Agent Handoff — Windows Client + Linux Server

Goal: fresh local checkouts on the Windows desktop and Linux host, the same source revision, a local build/test on both nodes, then a first trusted-LAN match. Do not run remote testing jobs.

## Rules

- Do not create/enable GitHub Actions workflows or hosted tests.
- Do not change old TypeScript components while verifying the native target unless a native dependency is proven.
- Do not import proprietary game art/audio or fetch game clients/assets.
- Do not bind to a public IP or expose the prototype outside a trusted LAN.
- Keep build outputs/logs in ignored build/ or another node-local evidence folder.
- Read docs/LOCAL_TESTING_POLICY.md and native/README.md before running anything.

## Windows node — standalone desktop client

Requirements: Git, CMake 3.25+, Visual Studio 2022 with Desktop development with C++, PowerShell.

~~~powershell
git clone https://github.com/StudioSuiri/Heroes-and-Generals-StudioSuiri-Version.git C:\dev\hng-native
cd C:\dev\hng-native
git checkout main
git rev-parse HEAD
.\scripts\dev\check-env-windows.ps1
.\scripts\dev\build-windows-client.ps1
~~~

Save the SHA and command output locally, not in source control. The build downloads/builds pinned SDL3 only if no installed package is found and this local script is run.

## Linux node — headless server

Requirements: Git, CMake 3.25+, Ninja, GCC/Clang with C++20 and standard build tools. The server-only build requires no SDL or display server.

~~~bash
git clone https://github.com/StudioSuiri/Heroes-and-Generals-StudioSuiri-Version.git ~/dev/hng-native
cd ~/dev/hng-native
git checkout main
git rev-parse HEAD
chmod +x scripts/dev/*.sh
./scripts/dev/check-env-linux.sh
./scripts/dev/build-linux-server.sh
~~~

Confirm the SHA is exactly the same as the Windows node. Run:

~~~bash
./build/linux-server/hng-server --bind 0.0.0.0 --port 26000 --tick-rate 30
~~~

Only allow UDP 26000 from the trusted LAN. In a Windows client terminal:

~~~powershell
.\build\windows-client\Release\hng-client.exe --connect 192.168.1.50 --port 26000
~~~

Replace the example IP with the Linux node's LAN IPv4.

## Evidence to return

- Exact SHA on both nodes and confirmation they match.
- OS build, CPU/RAM, compiler, CMake and Ninja version.
- CMake configure/build output and CTest summary from each relevant node.
- Server startup line, player join/leave, window status, movement replication, shooting/damage and capture behavior.
- Compiler warnings, packet/socket errors and any tick overruns.

Never include secrets, private keys, credentials or public IPs. This handoff prepares work; it does not claim that either node has been configured or that the first LAN session has run.
