#!/bin/bash
echo "make sure things are built in build_release ..."
mkdir build_release
pushd build_release
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j8
popd
swiftc -I thetis runThetis.swift -Lbuild_release -lThetisGPU
LD_LIBRARY_PATH=$PWD/build_release:$LD_LIBRARY_PATH ./runThetis