#!/usr/bin/env bash

build(){
    ROOT=$(git rev-parse --show-toplevel)
    mkdir -p ${ROOT}/build
    pushd ${ROOT}/build

    # LOCAL INSTALL ...
    CC=clang CXX=clang++ cmake ../ -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX="${ROOT}/install/"
    make -j4 && make install

    # SYSTEM INSTALL ...
    #CC=clang CXX=clang++ cmake ../ -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX="/usr/local/"
    #make -j4 # && make install
    #make package && sudo dpkg -i ChoUtil-*-Linux.deb

    popd
}
build
