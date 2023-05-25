#!/usr/bin/env bash

for SCALE in {10..15}; do

    echo "Starting SCALE=${SCALE}"
    perf stat --output scale_${SCALE}.log -x "," \
        -- env OMP_NUM_THREADS=6 ./build/bench/io data/n_${SCALE}.mtx

done