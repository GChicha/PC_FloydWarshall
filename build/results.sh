#!/bin/bash

Parallel_libs=( openmp pthread )

mkdir out

DATASET_SIZE=MEDIUM

cmake -DPLIB=seq -DDATASET_SIZE=${DATASET_SIZE} .. >> /dev/null
make >> /dev/null
for (( i = 0; i < 11; i++ )); do
    ./bin/FloydWarshall >> out/${DATASET_SIZE}_seq.time 2> out/${DATASET_SIZE}_seq.out
done

for pLibs in ${Parallel_libs[@]}; do
    for (( i = 1; i <= 32; i = i * 2 )); do
        cmake -DPLIB=$pLibs -DNUM_THREADS=$i .. >> /dev/null
        make >> /dev/null
        for (( j = 0; j < 11; j++ )); do
            ./bin/FloydWarshall >> out/${DATASET_SIZE}_${pLibs}_${i}.time 2> out/${DATASET_SIZE}_${pLibs}_${i}.out
            diff -qs out/${DATASET_SIZE}_seq.out out/${DATASET_SIZE}_${pLibs}_${i}.out
        done
        rm out/${DATASET_SIZE}_${pLibs}_${i}.out
    done
done

rm out/${DATASET_SIZE}_seq.out
