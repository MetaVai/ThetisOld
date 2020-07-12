#!/bin/bash

rm -Rvf build
rm -Rvf build_release
mkdir build
mkdir build_release
pushd build_release
cmake -DCMAKE_BUILD_TYPE=Release -DUSE_CUDA=OFF -DUSE_JULIA=OFF .. || exit 1
make -j6 || exit 1
./TestThetis || exit 1
popd
zip thetis.zip build_release/libThetis.so build_release/TestThetis testData/* licenses LICENSE
echo pwd is $PWD
ls -lh *.zip
echo ls -lh /home/runner/work/Thetis/Thetis/thetis.zip
ls -lh /home/runner/work/Thetis/Thetis/thetis.zip
