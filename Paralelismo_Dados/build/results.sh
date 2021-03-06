#!/bin/bash

Parallel_libs=( openmp pthread )

mkdir out

DATASET_SIZE=EXTRALARGE

MAX_THREADS=8

PAPI=1
TIME=0

rm -rf out/${DATASET_SIZE}
mkdir out/${DATASET_SIZE}

cmake -DPLIB=seq -DDUMP=ON -DSTDOUT=time -DDATASET_SIZE=${DATASET_SIZE} .. >> /dev/null
make >> /dev/null
./bin/FloydWarshall >> /dev/null 2> out/${DATASET_SIZE}/seq.out

if [[ ${TIME} == 1 ]]; then
    cmake -DPLIB=seq -DDUMP=OFF -DSTDOUT=time -DDATASET_SIZE=${DATASET_SIZE} .. >> /dev/null
    make >> /dev/null
    for (( i = 0; i < 10; i++ )); do
        ./bin/FloydWarshall >> out/${DATASET_SIZE}/seq.time 2> /dev/null
    done

    sort out/${DATASET_SIZE}/seq.time -o out/${DATASET_SIZE}/seq.time
    sed -i '1,1d' out/${DATASET_SIZE}/seq.time
    sed -i '$ d' out/${DATASET_SIZE}/seq.time
    awk '{s+=$1}END{print s/NR}' RS=" " out/${DATASET_SIZE}/seq.time > out/${DATASET_SIZE}/seq.time.avg
    rm out/${DATASET_SIZE}/seq.time
fi


if [[ ${PAPI} == 1 ]]; then
    # Contadores PAPI
    cmake -DPLIB=seq -DDUMP=OFF -DSTDOUT=papi -DDATASET_SIZE=${DATASET_SIZE} .. > /dev/null
    make >> /dev/null
    ./bin/FloydWarshall > /dev/null 2> /dev/null
    for (( j = 0; j < 10; j++ )); do
        ./bin/FloydWarshall >> out/${DATASET_SIZE}/seq.papi 2> /dev/null
    done
    awk '{s+=$1;s1+=$2;s2+=$3}END{print s/NR, s1/NR, s2/NR}' out/${DATASET_SIZE}/seq.papi >> out/${DATASET_SIZE}/seq.papi.avg
    rm out/${DATASET_SIZE}/seq.papi
fi

for pLibs in ${Parallel_libs[@]}; do
    for (( i = 2; i <= ${MAX_THREADS}; i = i * 2 )); do
        if [[ ${TIME} == 1 ]]; then
            # Tempo de execução / speed-up
            cmake -DPLIB=$pLibs -DDUMP=ON -DSTDOUT=time -DDATASET_SIZE=${DATASET_SIZE} -DNUM_THREADS=$i .. > /dev/null
            make >> /dev/null
            ./bin/FloydWarshall > /dev/null 2> /dev/null
            for (( j = 0; j < 10; j++ )); do
                ./bin/FloydWarshall >> out/${DATASET_SIZE}/${pLibs}_${i}.time 2> out/${DATASET_SIZE}/${pLibs}_${i}.out
                diff -qs out/${DATASET_SIZE}/seq.out out/${DATASET_SIZE}/${pLibs}_${i}.out
            done
            rm out/${DATASET_SIZE}/${pLibs}_${i}.out
            sort out/${DATASET_SIZE}/${pLibs}_${i}.time -o out/${DATASET_SIZE}/${pLibs}_${i}.time
            sed -i '1,1d' out/${DATASET_SIZE}/${pLibs}_${i}.time
            sed -i '$ d' out/${DATASET_SIZE}/${pLibs}_${i}.time
            awk '{s+=$1}END{print '${i}',s/NR}' RS=" " out/${DATASET_SIZE}/${pLibs}_${i}.time >> out/${DATASET_SIZE}/${pLibs}.time.avg
            rm out/${DATASET_SIZE}/${pLibs}_${i}.time
        fi

        if [[ ${PAPI} == 1 ]]; then
            # Contadores PAPI
            cmake -DPLIB=$pLibs -DDUMP=ON -DSTDOUT=papi -DDATASET_SIZE=${DATASET_SIZE} -DNUM_THREADS=$i .. > /dev/null
            make >> /dev/null
            ./bin/FloydWarshall > /dev/null 2> /dev/null

            for (( j = 0; j < 10; j++ )); do
                ./bin/FloydWarshall >> out/${DATASET_SIZE}/${pLibs}_${i}.papi 2> out/${DATASET_SIZE}/${pLibs}_${i}.out
                diff -qs out/${DATASET_SIZE}/seq.out out/${DATASET_SIZE}/${pLibs}_${i}.out
            done
            rm out/${DATASET_SIZE}/${pLibs}_${i}.out
            awk '{s+=$1;s1+=$2;s2+=$3}END{print '${i}', s/NR, s1/NR, s2/NR}' out/${DATASET_SIZE}/${pLibs}_${i}.papi >> out/${DATASET_SIZE}/${pLibs}.papi.avg
            rm out/${DATASET_SIZE}/${pLibs}_${i}.papi
        fi
    done
done

rm out/${DATASET_SIZE}/seq.out
