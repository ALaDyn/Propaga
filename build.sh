#!/bin/bash

if [[ "$OSTYPE" == "darwin"* && "$1" == "gcc" ]]; then
  export CC="/usr/local/bin/gcc-8"
  export CXX="/usr/local/bin/g++-8"
fi

mkdir -p build
cd build
cmake ..
#cmake .. -G "Ninja"
cmake --build . --target install
cd ..
