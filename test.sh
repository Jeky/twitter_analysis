#!/bin/bash
echo "[Compiling]............................................................................."
cmake -Dtest=ON
make
build/runUnitTest