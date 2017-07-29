/**
 * floyd-warshall.c: This file is part of the PolyBench/C 3.2 test suite.
 *
 *
 * Contact: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 * Web address: http://polybench.sourceforge.net
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

/* Include polybench common header. */
#include <polybench.h>

/* Include benchmark-specific header. */
/* Default data type is double, default size is 1024. */
#include "floyd-warshall.h"

#ifdef USE_PTHREAD
#include <pthread.h>
#endif

#ifdef USE_MPI
#include <mpi.h>

#ifndef MASTER
#define MASTER 0
#endif

#endif

#ifndef NUM_THREADS
#define NUM_THREADS 8
#endif

DATA_TYPE path[N][N];

/* Array initialization. */
static
void init_array (int n)
{
    int i, j;

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++)
            path[i][j] = ((DATA_TYPE) (i+1)*(j+1)) / n;
    }
}


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
static
void print_array(int n)

{
    int i, j;

    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++) {
            fprintf (stderr, DATA_PRINTF_MODIFIER, path[i][j]);
            if ((i * n + j) % 20 == 0) fprintf (stderr, "\n");
        }
    fprintf (stderr, "\n");
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
#if defined(USE_PTHREAD) || defined(USE_MPI)
/* Declara estrutura da thread */
typedef struct thread_var {
    int kin, kfim;
    int n;

    #ifdef USE_MPI
    int k;
    #endif
} thread_v;

void *floyd(void *x) {
    thread_v *var = (thread_v *) x;

    int n = var->n;
#else
static void kernel_floyd_warshall(int n) {
#endif
    int i, j, k;

    #ifdef USE_MPI
    for (k = var->k; k < var->k + 1; k++) {
    #else
    for (k = 0; k < n; k++) {
    #endif

	#if defined(USE_PTHREAD) || defined(USE_MPI)
        for (i = var->kin; i < var->kfim; i++)
	#else
        #ifdef USE_OPENMP
        #pragma omp parallel for private(i, j) num_threads(NUM_THREADS)
        #endif
        for (i = 0; i < n; i++)
	#endif
            for (j = 0; j < n; j++)
                path[i][j] = path[i][j] < path[i][k] + path[k][j]
                             ? path[i][j]
                             : path[i][k] + path[k][j];
    }

    #ifdef USE_PTHREAD
    pthread_exit(0);
    #elif USE_MPI
    return NULL;
    #endif
}


int main(int argc, char** argv)
{
    /* Retrieve problem size. */
    int n = N;

    size_t i = 0;

    /* Initialize array(s). */
    init_array (n);

    /* Start timer. */
    polybench_start_instruments;

 	#ifdef USE_PTHREAD
    /* Inicia pthread */
    pthread_t vetor_thread[NUM_THREADS];

    for (i = 0; i < NUM_THREADS; i++) {
        thread_v *x = (thread_v *)malloc(sizeof(thread_v));

        x->kin = (i * n)/NUM_THREADS;
        x->kfim = ((i+1) * n)/NUM_THREADS;

        x->n = n;

        pthread_create(&vetor_thread[i], NULL, floyd, x);
    }

    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(vetor_thread[i], NULL);
    }
  #elif USE_MPI
	int size, rank;

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &size);	
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);	

    /* printf("Process %d-%d on\n", rank+1, size); */

    int *displs = (int *)calloc(size, sizeof(int));
    int *recvcout = (int *)calloc(size, sizeof(int));

    displs[0] = 0;
    recvcout[0] = (n % size) * n * sizeof(DATA_TYPE);

    for (i = 0; i < size; i ++) {
        if (i > 0)
            displs[i] = displs[i - 1] + recvcout[i - 1];
        recvcout[i] += (n / size) * n * sizeof(DATA_TYPE);   
    }
    
    int k = 0;
    /* int j = 0; */
    /* printf("%d\n", getpid()); */
    for (; k < n; k++){
		// if (rank == MASTER)
		//	printf("K = %d\n", k);

        // MPI_Bcast(path, n * n * sizeof(DATA_TYPE), MPI_BYTE, MASTER, MPI_COMM_WORLD);
        // MPI_Scatter(&path[0][k], sizeof(DATA_TYPE), MPI_BYTE, &path[0][k], n * sizeof(DATA_TYPE), MPI_BYTE,  MASTER, MPI_COMM_WORLD);
        
        /* while (j == 0) */
        /*     sleep(5); */

        thread_v x;

        x.kin = recvcout[rank] / (n * sizeof(DATA_TYPE)) * rank;
        x.kfim = recvcout[rank] / (n * sizeof(DATA_TYPE)) * (rank + 1);
        x.k = k;

        x.n = n;

        floyd(&x);

        MPI_Gatherv(path[x.kin], recvcout[rank], MPI_BYTE, path, recvcout, displs, MPI_BYTE, MASTER, MPI_COMM_WORLD);
    }

    MPI_Finalize();
	
  #else
    /* Run kernel. */
    kernel_floyd_warshall(n);
  #endif

    /* Stop and print timer. */
    #ifdef USE_MPI
    if (rank == MASTER){
        polybench_stop_instruments;
        polybench_print_instruments;

        /* Prevent dead-code elimination. All live-out data must be printed
           by the function call in argument. */
        polybench_prevent_dce(print_array(n));
    }
    #else
    polybench_stop_instruments;
    polybench_print_instruments;

    /* Prevent dead-code elimination. All live-out data must be printed
       by the function call in argument. */
    polybench_prevent_dce(print_array(n));
    #endif

    /* Be clean. */
    // POLYBENCH_FREE_ARRAY(path);

    return 0;
}
