#!/bin/bash

Parallel_libs=( openmp pthread )

rm -rf out
mkdir out

DATASET_SIZE=MINI

cmake -DPLIB=seq -DDATASET_SIZE=${DATASET_SIZE} .. >> /dev/null
make >> /dev/null
./bin/FloydWarshall >> /dev/null 2> /dev/null
for (( i = 0; i < 10; i++ )); do
    ./bin/FloydWarshall >> out/${DATASET_SIZE}_seq.time 2> out/${DATASET_SIZE}_seq.out
done

for pLibs in ${Parallel_libs[@]}; do
    for (( i = 1; i <= 32; i = i * 2 )); do
        cmake -DPLIB=$pLibs -DNUM_THREADS=$i .. >> /dev/null
        make >> /dev/null
        ./bin/FloydWarshall >> /dev/null 2> /dev/null
        for (( j = 0; j < 10; j++ )); do
            ./bin/FloydWarshall >> out/${DATASET_SIZE}_${pLibs}_${i}.time 2> out/${DATASET_SIZE}_${pLibs}_${i}.out
            diff -qs out/${DATASET_SIZE}_seq.out out/${DATASET_SIZE}_${pLibs}_${i}.out
        done
        rm out/${DATASET_SIZE}_${pLibs}_${i}.out
        sort out/${DATASET_SIZE}_${pLibs}_${i}.time -o out/${DATASET_SIZE}_${pLibs}_${i}.time
        sed -i '1,1d' out/${DATASET_SIZE}_${pLibs}_${i}.time
        sed -i '$ d' out/${DATASET_SIZE}_${pLibs}_${i}.time
        awk '{s+=$1}END{print '${i}',s/NR}' RS=" " out/${DATASET_SIZE}_${pLibs}_${i}.time >> out/${DATASET_SIZE}_${pLibs}.avg
    done
done

rm out/${DATASET_SIZE}_seq.out
