# HnG Native — native reimplementation scaffold

This directory starts a clean-room native reimplementation inspired by the broad gameplay loop of Heroes & Generals. It is not a port of the closed server, it does not use its network protocol, and it deliberately does not use Flash, a browser UI, Electron or Tauri.

## Current vertical slice in source

- Shared C++20 core for packet framing and authoritative simulation.
- Headless UDP server with a fixed-rate world loop.
- Native SDL3 desktop client.
- A 1600x900 greybox arena using generated shapes, two prototype factions, movement, aim, hitscan damage, health, deaths, respawn, and one capture zone.
- A dependency-free C++ test executable registered with CTest.
- CMake targets and local build scripts for Linux server and Windows client.
- Planned folders for combat/movement systems, persistence, campaign, UI and generic art.

Important: the new native scaffold has not been compiled or executed during this repository-editing session. No claim of a passing native build is made until the local node agent runs the commands and records the actual result.

## Dependencies

- C++20 compiler: GCC 12+ or Clang 15+ on Linux as initial expected baselines; Visual Studio 2022 with Desktop development with C++ on Windows.
- CMake 3.25+.
- Ninja on Linux; Visual Studio's generator on Windows.
- SDL3 3.2+ only for the graphical client. The Windows/Linux client scripts opt into a pinned source fetch of SDL3 release-3.4.18 when executed locally if no installed SDL3 package exists.
- The server and core tests need no SDL, browser, database or other third-party runtime.

## Build locally

Linux headless dedicated server:

~~~bash
chmod +x scripts/dev/*.sh
./scripts/dev/check-env-linux.sh
./scripts/dev/build-linux-server.sh
~~~

Windows native client + server + tests, from PowerShell at repository root:

~~~powershell
.\scripts\dev\check-env-windows.ps1
.\scripts\dev\build-windows-client.ps1
~~~

The build scripts invoke CMake and CTest on the machine where you run them. They do not trigger hosted runners, package publishing or remote deployment.

## Run locally

Terminal 1:

~~~bash
./build/linux-server/hng-server --bind 0.0.0.0 --port 26000 --tick-rate 30
~~~

Terminal 2 (adjust path for the selected build configuration):

~~~bash
./build/linux-client/hng-client --connect 127.0.0.1 --port 26000
~~~

Controls: WASD or arrows to move, mouse to aim, left mouse button to fire, Escape to quit. With two computers, use the Linux node's trusted LAN IPv4 address as the connect argument. Allow UDP 26000 only on a trusted LAN while testing.

## Network protocol and security warning

See PROTOCOL_V1.md. Protocol v1 is a compact, explicit binary UDP format that avoids the original game's private protocol. It is currently a development-only LAN protocol: no authentication, encryption, proper reliability/congestion control, public-hosting hardening or anti-cheat exists. Do not port-forward it or expose it to the public internet.

## Relationship to the legacy repositories

The original HAGCP packages, components/hgapi, and historical launcher remain available as separately documented research material. They are not runtime dependencies of hng-client or hng-server. The native implementation should not inherit the old Flash interface, legacy client updater, historical HQ endpoints, or protocols just because their sources are present in the same monorepo.
