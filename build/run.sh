#!/bin/bash

DATASET_SIZE=$1

make DATASET_SIZE=${DATASET_SIZE}

./bin/FloydWarshall-${DATASET_SIZE}-seq >> out/seq-${DATASET_SIZE}.time 2> out/seq-${DATASET_SIZE}.out
mpirun ./bin/FloydWarshall-${DATASET_SIZE}-mpi >> out/mpi-${DATASET_SIZE}.time 2> out/mpi-${DATASET_SIZE}.out

diff -qs out/mpi-${DATASET_SIZE}.out out/seq-${DATASET_SIZE}.out
