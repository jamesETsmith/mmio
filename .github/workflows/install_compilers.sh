#!/usr/bin/env bash

FAMILY=$1
VERSION=$2

# https://docs.github.com/en/actions/using-workflows/workflow-commands-for-github-actions#setting-an-environment-variable

# Prepare for install
sudo apt update
sudo apt install -y build-essential
if [[ "${FAMILY}" == gcc ]]; then
    sudo apt install gcc-${VERSION} g++-${VERSION}
    echo "CC=gcc-${VERSION}" >> "$GITHUB_ENV"
    echo "CXX=g++-${VERSION}" >> "$GITHUB_ENV"
elif [[ "${FAMILY}" == llvm ]]; then
    wget https://apt.llvm.org/llvm.sh
    chmod +x llvm.sh
    sudo ./llvm.sh ${VERSION} all
    echo "CC=clang-${VERSION}" >> "$GITHUB_ENV"
    echo "CXX=clang++-${VERSION}" >> "$GITHUB_ENV"
else
    echo "[ERROR]: compiler family name not supported"
    exit -1
fi
