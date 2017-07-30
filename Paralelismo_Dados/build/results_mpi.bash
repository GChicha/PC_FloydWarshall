EXEC=floyd_mpi
SIZE=$1
NUM_PROC=(1 2 4 8)

mkdir out

for N_P in ${NUM_PROC} do
    mpicc -DN=${SIZE} -DDUMP floyd_mpi.c -o Floyd_MPI
    mpirun -n ${N_P} ./Floyd_MPI >> out/mpi_${N_P}.time 2> out/mpi_${N_P}.out
done
