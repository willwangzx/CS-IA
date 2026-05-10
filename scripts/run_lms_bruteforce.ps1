param(
    [int64[]]$Counts = @(100, 500, 1000, 2000),
    [string]$BuildDir = "build-tests",
    [string]$Configuration = "Release",
    [string]$OutputDir = "docs"
)

$ErrorActionPreference = "Stop"

cmake -S . -B $BuildDir -DBUILD_GUI_APP=OFF -DBUILD_TESTING=ON
cmake --build $BuildDir --config $Configuration --target lms_bruteforce_test

$exe = Join-Path $BuildDir "lms_bruteforce_test"
if (-not (Test-Path $exe)) {
    $exe = Join-Path $BuildDir "lms_bruteforce_test.exe"
}
if (-not (Test-Path $exe)) {
    $exe = Join-Path $BuildDir (Join-Path $Configuration "lms_bruteforce_test.exe")
}

New-Item -ItemType Directory -Force -Path $OutputDir | Out-Null
$csv = Join-Path $OutputDir "lms_bruteforce_results.csv"
if (Test-Path $csv) {
    Remove-Item -LiteralPath $csv -Force
}

$append = $false
foreach ($count in $Counts) {
    if ($append) {
        & $exe $count $OutputDir --append
    } else {
        & $exe $count $OutputDir
        $append = $true
    }
}
