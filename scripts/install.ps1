<#
.SYNOPSIS
    Build sloptour (Release) and install it as the `sloptour` command on Windows.

.EXAMPLE
    powershell -ExecutionPolicy Bypass -File scripts\install.ps1
    powershell -ExecutionPolicy Bypass -File scripts\install.ps1 -Prefix C:\tools\sloptour
    powershell -ExecutionPolicy Bypass -File scripts\install.ps1 -Uninstall
#>
[CmdletBinding()]
param(
    [string]$Prefix = (Join-Path $env:LOCALAPPDATA "Programs\sloptour"),
    [switch]$NoPath,
    [switch]$Uninstall
)

$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent $PSScriptRoot
$Build = Join-Path $Root "build\install"
$BinDir = Join-Path $Prefix "bin"
$Binary = Join-Path $BinDir "sloptour.exe"

function Say([string]$Message) { Write-Host "==> $Message" -ForegroundColor Cyan }
function Fail([string]$Message) { Write-Host "error: $Message" -ForegroundColor Red; exit 1 }

function Invoke-Checked {
    param([string]$Command, [string[]]$Arguments)
    & $Command @Arguments
    if ($LASTEXITCODE -ne 0) { Fail "$Command $($Arguments -join ' ') failed with exit code $LASTEXITCODE" }
}

function Get-UserPath { [Environment]::GetEnvironmentVariable("Path", "User") }

function Set-UserPath([string]$Value) { [Environment]::SetEnvironmentVariable("Path", $Value, "User") }

function Remove-FromPath {
    $entries = (Get-UserPath) -split ";" | Where-Object { $_ -and ($_.TrimEnd("\") -ne $BinDir.TrimEnd("\")) }
    Set-UserPath ($entries -join ";")
}

if ($Uninstall) {
    if (Test-Path $Binary) {
        Remove-Item $Binary -Force
        Say "removed $Binary"
    } else {
        Write-Host "nothing installed at $Binary"
    }
    Remove-FromPath
    Say "removed $BinDir from the user PATH"
    exit 0
}

function Enter-VisualStudioShell {
    if (Get-Command cl.exe -ErrorAction SilentlyContinue) { return }
    $vswhere = Join-Path ${env:ProgramFiles(x86)} "Microsoft Visual Studio\Installer\vswhere.exe"
    if (-not (Test-Path $vswhere)) {
        Fail "Visual Studio 2022 (Desktop development with C++) is required: winget install Microsoft.VisualStudio.2022.BuildTools --override `"--add Microsoft.VisualStudio.Workload.VCTools --includeRecommended --passive`""
    }
    $installation = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
    if (-not $installation) { Fail "no Visual Studio installation with the C++ tools was found" }
    $devShell = Join-Path $installation "Common7\Tools\Launch-VsDevShell.ps1"
    Say "entering the Visual Studio developer environment"
    & $devShell -Arch amd64 -HostArch amd64 -SkipAutomaticLocation | Out-Null
}

function Assert-Tools {
    $missing = @("cmake", "ninja", "conan", "git") | Where-Object { -not (Get-Command $_ -ErrorAction SilentlyContinue) }
    if ($missing) {
        Fail ("missing: " + ($missing -join ", ") + "`n       install them with:  winget install Kitware.CMake Ninja-build.Ninja Git.Git Python.Python.3.12; pip install conan")
    }
}

Enter-VisualStudioShell
Assert-Tools

if (-not (& conan profile path default 2>$null)) {
    Say "creating the default conan profile"
    Invoke-Checked conan @("profile", "detect", "--force")
}

Say "installing dependencies with conan (first run builds them, this can take a while)"
$env:SLOPTOUR_NO_USER_PRESETS = "1"
Invoke-Checked conan @("install", $Root, "--output-folder", $Build, "--build=missing",
    "-s", "build_type=Release", "-s", "compiler.cppstd=23", "-s", "compiler.runtime=static",
    "-c", "tools.cmake.cmaketoolchain:generator=Ninja")

$toolchain = Get-ChildItem -Path $Build -Recurse -Filter conan_toolchain.cmake |
    Where-Object { $_.FullName -like "*generators*" } | Select-Object -First 1
if (-not $toolchain) { Fail "conan did not generate a toolchain" }

Say "configuring"
Invoke-Checked cmake @("-S", $Root, "-B", (Join-Path $Build "cmake"), "-G", "Ninja",
    "-DCMAKE_TOOLCHAIN_FILE=$($toolchain.FullName)", "-DCMAKE_BUILD_TYPE=Release", "-DCMAKE_SKIP_RPATH=ON",
    "-DSLOPTOUR_BUILD_TESTS=OFF", "-DSLOPTOUR_ENABLE_CLANG_TIDY=OFF", "-DSLOPTOUR_WARNINGS_AS_ERRORS=OFF")

Say "building"
Invoke-Checked cmake @("--build", (Join-Path $Build "cmake"), "--parallel")

Say "installing into $BinDir"
Invoke-Checked cmake @("--install", (Join-Path $Build "cmake"), "--prefix", $Prefix)
& $Binary --help | Out-Null
if ($LASTEXITCODE -ne 0) { Fail "the installed binary does not start" }

if (-not $NoPath) {
    $current = (Get-UserPath) -split ";"
    if ($current -notcontains $BinDir) {
        Set-UserPath ((@($current | Where-Object { $_ }) + $BinDir) -join ";")
        Say "added $BinDir to the user PATH (open a new terminal to use it)"
    }
    $env:Path = "$BinDir;$env:Path"
}

Say "done - run ``sloptour`` inside any repository"
