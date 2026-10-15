# Dependency Setup Guide

Dependency installation is local and opt-in. No setup helper runs unless the node agent starts it.

## Linux server node

Requirements: C++20 compiler, CMake 3.25+, Ninja, Git and pkg-config. The headless server has no SDL/graphics dependency.

On Debian/Ubuntu, print the package plan first:

~~~bash
./scripts/dev/install-deps-linux.sh --server
~~~

To install after reviewing the package list:

~~~bash
./scripts/dev/install-deps-linux.sh --server --install
~~~

## Linux graphical client

The optional SDL3 source build may use native audio, X11/Wayland, input and graphics development libraries. Review the package plan:

~~~bash
./scripts/dev/install-deps-linux.sh --client
~~~

Then, only if you want the helper to install those dependencies:

~~~bash
./scripts/dev/install-deps-linux.sh --client --install
~~~

For distributions other than Debian/Ubuntu, install equivalent C++20/CMake/Ninja/Git and SDL3 build packages with the distribution's own package manager.

## Windows client/server node

Use Visual Studio 2022 with the Desktop development with C++ workload, CMake 3.25+ and Git. Ninja is optional because the Windows build script uses the Visual Studio 2022 generator. SDL3 is found from a local CMake package when installed; otherwise the build script opts into fetching the pinned SDL3 3.4.18 source locally. No vcpkg/Conan package graph is required for the initial scaffold.

Run the environment check before build:

~~~powershell
.\scripts\dev\check-env-windows.ps1
~~~

The script only reports tool availability and does not install software.

## Dependency design choice

Only the native client uses SDL3. The shared game core and server use standard C++ and OS UDP socket APIs. No web runtime, Flash compatibility, database, serialization framework, or networking framework is required for the initial LAN slice.
