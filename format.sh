#!/bin/bash

DIRS="src src/ml src/utils include include/ml include/utils test"
for d in $DIRS
do
    for f in $d/*
    do
        echo "Formatting $f..."
        clang-format -i f
    done
done
