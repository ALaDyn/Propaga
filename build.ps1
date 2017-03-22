mkdir -Force build_vs
cd build_vs
cmake .. -G "Visual Studio 15" "-DCMAKE_TOOLCHAIN_FILE=$Home\Codice\vcpkg\scripts\buildsystems\vcpkg.cmake"
cmake --build .
cd ..
