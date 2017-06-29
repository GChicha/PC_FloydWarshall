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
#if !defined(USE_PTHREAD) || !(USE_MPI)
static void kernel_floyd_warshall(int n) {

#else
/* Declara estrutura da thread */
typedef struct thread_var {
    int kin, kfim;
} thread_v;

void *floyd(void *x) {
    thread_v *var = (thread_v *) x;

#endif
    int i, j, k;

    for (k = 0; k < _PB_N; k++) {
	#ifndef USE_PTHREAD

	#ifdef USE_OPENMP
        #pragma omp parallel for private(i, j) num_threads(NUM_THREADS)
	#endif
        for (i = 0; i < _PB_N; i++)
	#else
        for (i = var->kin; i < var->kfim; i++)
	#endif
            for (j = 0; j < _PB_N; j++)
                path[i][j] = path[i][j] < path[i][k] + path[k][j]
                             ? path[i][j]
                             : path[i][k] + path[k][j];
    }
}


int main(int argc, char** argv)
{
    /* Retrieve problem size. */
    int n = N;

    /* Initialize array(s). */
    init_array (n);

    /* Start timer. */
    polybench_start_instruments;

   #ifdef USE_PTHREAD
    /* Inicia pthread */
    pthread_t vetor_thread[NUM_THREADS];

    size_t i = 0;

    for (i = 0; i < NUM_THREADS; i++) {
        thread_v *x = (thread_v *)malloc(sizeof(thread_v));

        x->kin = (i * n)/NUM_THREADS;
        x->kfim = ((i+1) * n)/NUM_THREADS;

        pthread_create(&vetor_thread[i], NULL, floyd, x);
    }

    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(vetor_thread[i], NULL);
    }
  #ifdef USE_MPI
	int size, rank;

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &size);	
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);	
	
	thread_v *x = (thread_v *)malloc(sizeof(thread_v));

	if(rank == MASTER) {
		size_t i;
		for (i = 0; i < size; i++) {
			if (i != MASTER) {
				x->kin = (i * n)/size;
				x->kfim = ((i+1) * n)/size;

				MPI_Send(x, 2, MPI_INT, i, 0, MPI_COMM_WORLD); 
			}
		}

		x->kin = (i * n)/size;
		x->kfim = ((i+1) * n)/size;

		floyd(x);

		MPI_
	}
	else {
		MPI_Recv(x, 2, MPI_INT, 0, 0, MPI_COMM_WORLD);
		
		floyd(x);		

		MPI_Send(path, n*n*sizeof(DATA_TYPE), MPI_BYTE, 0, 1, MPI_COMM_WORLD); 
	}
  #else
    /* Run kernel. */
    kernel_floyd_warshall(n);
  #endif

    /* Stop and print timer. */
    polybench_stop_instruments;
    polybench_print_instruments;

    /* Prevent dead-code elimination. All live-out data must be printed
       by the function call in argument. */
    polybench_prevent_dce(print_array(n));

    /* Be clean. */
    // POLYBENCH_FREE_ARRAY(path);

    return 0;
}
