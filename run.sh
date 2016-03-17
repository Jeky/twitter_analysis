#!/bin/bash
rm build/analysis
echo "[Compiling].............................................................."
~/Applications/CMake.app/Contents/Bin/cmake -Dtest=OFF
make
echo "[Running]................................................................"
script -q /dev/null time build/analysis|tee log.txt
