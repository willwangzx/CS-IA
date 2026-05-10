param(
    [int64]$Count = 10000000,
    [string]$Pattern = "permuted",
    [int64]$RemoveEvery = 0,
    [string]$BuildDir = "build-tests",
    [string]$Configuration = "Release"
)

$ErrorActionPreference = "Stop"

cmake -S . -B $BuildDir -DBUILD_GUI_APP=OFF -DBUILD_TESTING=ON
cmake --build $BuildDir --config $Configuration --target rbtree_stress_test

$exe = Join-Path $BuildDir "rbtree_stress_test"
if (-not (Test-Path $exe)) {
    $exe = Join-Path $BuildDir "rbtree_stress_test.exe"
}
if (-not (Test-Path $exe)) {
    $exe = Join-Path $BuildDir (Join-Path $Configuration "rbtree_stress_test.exe")
}

& $exe $Count $Pattern $RemoveEvery
