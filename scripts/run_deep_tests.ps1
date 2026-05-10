param(
    [int64]$FullCount = 10000000,
    [string]$BuildDir = "build-tests",
    [string]$Configuration = "Release",
    [switch]$SkipFullStress
)

$ErrorActionPreference = "Stop"

cmake -S . -B $BuildDir -DBUILD_GUI_APP=OFF -DBUILD_TESTING=ON
cmake --build $BuildDir --config $Configuration
ctest --test-dir $BuildDir -C $Configuration --output-on-failure

$exe = Join-Path $BuildDir "rbtree_stress_test"
if (-not (Test-Path $exe)) {
    $exe = Join-Path $BuildDir "rbtree_stress_test.exe"
}
if (-not (Test-Path $exe)) {
    $exe = Join-Path $BuildDir (Join-Path $Configuration "rbtree_stress_test.exe")
}

& $exe 1000 ordered 10
& $exe 1000 reverse 10
& $exe 100000 permuted 97

if (-not $SkipFullStress) {
    & $exe $FullCount permuted 0
}
