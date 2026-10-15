$ErrorActionPreference = "Stop"
$Root = (Resolve-Path (Join-Path $PSScriptRoot "../..")).Path
& (Join-Path $Root "scripts/dev/build-windows-client.ps1")
if ($LASTEXITCODE -ne 0) { throw "Local Windows build failed." }

$Build = Join-Path $Root "build/windows-client/Release"
$Stage = Join-Path $Root "build/stage/windows-client"
New-Item -ItemType Directory -Force -Path $Stage | Out-Null
Copy-Item (Join-Path $Build "hng-client.exe") $Stage -Force
Copy-Item (Join-Path $Build "hng-server.exe") $Stage -Force -ErrorAction SilentlyContinue
Copy-Item (Join-Path $Build "SDL3.dll") $Stage -Force -ErrorAction SilentlyContinue
@"
HnG Native standalone client — local/LAN prototype only.
Start: .\hng-client.exe --connect 127.0.0.1 --port 26000
Account system, public server security and anti-cheat are not implemented.
"@ | Set-Content -Encoding UTF8 (Join-Path $Stage "README.txt")
Write-Host "Staged local Windows package at $Stage"
Write-Host "No remote copy/deployment was attempted."
