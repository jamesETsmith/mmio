#!/usr/bin/env bash

set -xeu -o pipefail

GRB_VERSION="7.4.1"
GRB_HASH="hcb278e6"
GRB_EXTENSION="conda"

mkdir graphblas-binaries
cd graphblas-binaries
wget --quiet https://anaconda.org/conda-forge/graphblas/${GRB_VERSION}/download/linux-64/graphblas-${GRB_VERSION}-${GRB_HASH}_0.${GRB_EXTENSION}
if [ ${GRB_EXTENSION} == "tar.bz2" ]; then
tar xf graphblas-${GRB_VERSION}-${GRB_HASH}_0.${GRB_EXTENSION}
else
unzip graphblas-${GRB_VERSION}-${GRB_HASH}_0.${GRB_EXTENSION}
tar xf pkg-graphblas-${GRB_VERSION}-${GRB_HASH}_0.tar.zst
fi
cd ..