#!/bin/sh
[ -z $CONFIG ] && config=Release || config="$CONFIG"

cmake --version

    # -G Ninja \
cmake \
    -S . \
    -B build \
    -DBUILD_TESTING=OFF \
    -DCMAKE_BUILD_TYPE=$config \
&&
cd build \
&&
make -j$(nproc)
