#!/usr/bin/env bash

ROOT=$(git rev-parse --show-toplevel)
echo $ROOT

pushd build
cmake ../ -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX="${ROOT}/install/"
make -j8
make install
popd
