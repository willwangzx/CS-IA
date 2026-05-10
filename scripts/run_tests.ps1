param(
    [string]$BuildDir = "build-tests",
    [string]$Configuration = "Debug"
)

$ErrorActionPreference = "Stop"

cmake -S . -B $BuildDir -DBUILD_GUI_APP=OFF -DBUILD_TESTING=ON
cmake --build $BuildDir --config $Configuration
ctest --test-dir $BuildDir -C $Configuration --output-on-failure
