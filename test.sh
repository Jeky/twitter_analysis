#!/bin/bash
rm build/runUnitTest
echo "[Compiling].............................................................."
cmake -Dtest=ON
make
echo "[Running]................................................................"
build/runUnitTest