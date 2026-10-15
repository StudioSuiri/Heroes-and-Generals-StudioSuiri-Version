$ErrorActionPreference = "Stop"
$Root = (Resolve-Path (Join-Path $PSScriptRoot "../..")).Path
$Build = Join-Path $Root "build/windows-client"
if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) { throw "CMake 3.25+ is required." }
if (-not (Get-Command git -ErrorAction SilentlyContinue)) { throw "Git is required because this local build may fetch the pinned SDL3 source." }
cmake -S (Join-Path $Root "native") -B $Build -G "Visual Studio 17 2022" -A x64 -DHNG_BUILD_CLIENT=ON -DHNG_BUILD_SERVER=ON -DHNG_BUILD_TESTS=ON -DHNG_FETCH_SDL3=ON
if ($LASTEXITCODE -ne 0) { throw "CMake configure failed with exit code $LASTEXITCODE" }
cmake --build $Build --config Release --parallel
if ($LASTEXITCODE -ne 0) { throw "Build failed with exit code $LASTEXITCODE" }
ctest --test-dir $Build -C Release --output-on-failure
if ($LASTEXITCODE -ne 0) { throw "Local CTest failed with exit code $LASTEXITCODE" }
Write-Host "Client: $Build\Release\hng-client.exe"
Write-Host "Server: $Build\Release\hng-server.exe"
Write-Host "All work above ran locally on this Windows node; no cloud runner was started."
