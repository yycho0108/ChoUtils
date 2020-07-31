#!/usr/bin/env bash

build(){
    ROOT=$(git rev-parse --show-toplevel)
    mkdir -p ${ROOT}/build
    pushd ${ROOT}/build
    cmake ../ -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX="${ROOT}/install/"
    make -j8
    make install
    popd
}
build
