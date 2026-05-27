#!/bin/sh
set -e

config="${CONFIG:-Release}"

cmake --version

cmake \
    -S /code \
    -B /code/build \
    -G Ninja \
    -DBUILD_TESTING=OFF \
    -DCMAKE_BUILD_TYPE="${config}"

cmake --build /code/build --parallel "$(nproc)"
