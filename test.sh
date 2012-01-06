#!/bin/bash

cd `dirname $0`

rm -rf cmake_tmp
mkdir cmake_tmp
cd cmake_tmp

LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`
CFLAGS="-g $CFLAGS" LDFLAGS="-Wl,--as-needed -Wl,--hash-style=both" cmake .. -DCMAKE_INSTALL_PREFIX=/usr && make test && echo "Test OK. No error."

