#!/bin/bash

Parallel_libs=( openmp pthread )

mkdir out

cmake -DPLIB=seq "."
make
for (( i = 0; i < 11; i++ )); do
    ./FloydWarshall >> out/seq.time 2>> out/seq.out
done

for pLibs in ${Parallel_libs[@]}; do
    for (( i = 1; i <= 32; i = i * 2 )); do
        cmake -DPLIB=$pLibs -DNUM_THREADS=$i .
        make
        for (( j = 0; j < 11; j++ )); do
            ./FloydWarshall >> out/time_${pLibs}_${i}.time 2> out/large_${pLibs}_${i}.out
        done
        diff -qs out/seq.out out/large_${pLibs}_${i}.out
    done
done
