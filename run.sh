#!/bin/bash
rm build/analysis
echo "[Compiling]............................................................................."
cmake -Dtest=OFF
make
echo "[Running]..............................................................................."
build/analysis
