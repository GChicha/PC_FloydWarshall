DATASET_SIZE=EXTRALARGE

ARGS=-DPOLYBENCH_TIME -DPOLYBENCH_DUMP_ARRAYS -D${DATASET_SIZE}_DATASET -I ../polybench/utilities ../polybench/utilities/polybench.c ../polybench/floyd-warshall/floyd-warshall.c

all:
	mkdir -p bin
	gcc ${ARGS} -o bin/FloydWarshall-${DATASET_SIZE}-seq
	mpicc ${ARGS} -DUSE_MPI -o bin/FloydWarshall-${DATASET_SIZE}-mpi
	
clean:
	rm -rf bin
