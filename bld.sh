#!/bin/bash
cmake -S . -B build
pushd build
cmake --build . --parallel 9 
popd