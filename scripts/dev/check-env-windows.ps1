$ErrorActionPreference = "Continue"
Write-Host "=== HnG Native Windows environment ==="
foreach ($tool in @("git", "cmake", "ninja", "cl")) {
    $found = Get-Command $tool -ErrorAction SilentlyContinue
    if ($found) { Write-Host ("FOUND   {0}: {1}" -f $tool, $found.Source) }
    else { Write-Host ("NOT IN PATH: {0}" -f $tool) }
}
if (Get-Command cmake -ErrorAction SilentlyContinue) { cmake --version | Select-Object -First 1 }
if (Get-Command git -ErrorAction SilentlyContinue) { git --version }
Write-Host ""
Write-Host "Required for the Windows client: Visual Studio 2022 C++ workload, CMake 3.25+, Git."
Write-Host "The optional CMake SDL3 fetch downloads source only when the local build script is run."
Write-Host "This check does not start a build, test, remote workflow or package publish."
