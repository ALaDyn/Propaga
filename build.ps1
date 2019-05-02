#!/usr/bin/env pwsh

Remove-Item .\build -Force -Recurse
New-Item .\build -itemtype directory -Force
Set-Location build
cmake -G "Ninja" "-DCMAKE_TOOLCHAIN_FILE=$env:WORKSPACE\vcpkg\scripts\buildsystems\vcpkg.cmake" ..
cmake --build . --target install
Set-Location ..
