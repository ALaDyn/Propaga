Remove-Item .\build -Force -Recurse
New-Item .\build -itemtype directory -Force
cd build
#cmake .. -G "Visual Studio 15" "-DCMAKE_TOOLCHAIN_FILE=$env:WORKSPACE\vcpkg\scripts\buildsystems\vcpkg.cmake"
cmake .. -G "Ninja" "-DCMAKE_TOOLCHAIN_FILE=$env:WORKSPACE\vcpkg\scripts\buildsystems\vcpkg.cmake" 
cmake --build .
cd ..

