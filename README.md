
# Faster Parsing for Matrix Market
[![build](https://github.com/jamesETsmith/mmio/actions/workflows/build.yaml/badge.svg)](https://github.com/jamesETsmith/mmio/actions/workflows/build.yaml)

There are several great C++ projects ([PIGO](https://github.com/GT-TDAlab/PIGO) and [fast_matrix_market](https://github.com/alugowski/fast_matrix_market)) for reading matrix market files faster in serial and parallel.
However, GraphBLAS is written in ANSI C and I wanted to try and write a matrix market reader (and eventually writer) in C so it could be used side-by-side with GraphBLAS.
This is based heavily on the great C++ projects mentioned above and in several of the utility functions are lifted almost verbatim from those projects.

:warning: This project is pre-alpha and is currently unstable. Currently only reading matrix market coordinate files with doubles is supported. Please understand there will be breaking changes.

## Prerequisites
- C/C++ compiler
- Python3 and SciPy (Optional, needed to make benchmarking data)

## Build
```
git@github.com:jamesETsmith/mtxio.git
cd mtxio
export CPM_SOURCE_CACHE=$HOME/.cache/CPM # (optional)
cmake -B build
cmake --build build
ctest --test-dir build
```

## Benchmarking
```
cd data && python generate_data.py # May take a while
cd ..
perf stat -- env OMP_NUM_THREADS=12 ./build/bench/pigo_read data/n_15_0.3.mtx
perf stat -- env OMP_NUM_THREADS=12 ./build/bench/mtxio_read data/n_15_0.3.mtx
```