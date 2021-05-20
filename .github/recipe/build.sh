#!/bin/bash

mkdir build
cd build

cmake \
  -DCMAKE_INSTALL_PREFIX=$PREFIX \
  -DCMAKE_PREFIX_PATH=$PREFIX \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=OFF \
  -DSTANDARDESE_BUILD_TEST=OFF \
  ..

make -j${CPU_COUNT} VERBOSE=1
make install
