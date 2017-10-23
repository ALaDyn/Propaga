rm -rf build
mkdir build
cd build
cmake .. -G "Ninja"
cmake --build . --target install
cd ..
