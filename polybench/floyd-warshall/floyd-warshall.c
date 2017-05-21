/**
 * This version is stamped on May 10, 2016
 *
 * Contact:
 *   Louis-Noel Pouchet <pouchet.ohio-state.edu>
 *   Tomofumi Yuki <tomofumi.yuki.fr>
 *
 * Web address: http://polybench.sourceforge.net
 */
/* floyd-warshall.c: this file is part of PolyBench/C */

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#ifdef USE_PTHREAD
#include <pthread.h>
#endif


#ifndef NUM_THREADS
#define NUM_THREADS 8
#endif

/* Include polybench common header. */
#include "../utilities/polybench.h"

/* Include benchmark-specific header. */
#include "floyd-warshall.h"

/* Array initialization. */
static void init_array(int n, DATA_TYPE POLYBENCH_2D(path, N, N, n, n)) {
    int i, j;

    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++) {
            path[i][j] = i * j % 7 + 1;
            if ((i + j) % 13 == 0 || (i + j) % 7 == 0 || (i + j) % 11 == 0)
                path[i][j] = 999;
        }
}

/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
static void print_array(int n, DATA_TYPE POLYBENCH_2D(path, N, N, n, n))

{
    int i, j;

    POLYBENCH_DUMP_START;
    POLYBENCH_DUMP_BEGIN("path");
    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++) {
            if ((i * n + j) % 20 == 0)
                fprintf(POLYBENCH_DUMP_TARGET, "\n");
            fprintf(POLYBENCH_DUMP_TARGET, DATA_PRINTF_MODIFIER, path[i][j]);
        }
    POLYBENCH_DUMP_END("path");
    POLYBENCH_DUMP_FINISH;
}


#ifdef USE_PTHREAD
/* Declara estrutura da thread */
typedef struct thread_var {
    int n;
    int kin, kfim;
    DATA_TYPE *path;
    pthread_barrier_t *barrier;
} thread_v;

/* Função thread */
void *floyd(void *x) {
    thread_v *var = (thread_v *) x;

    int i, j, k;

    for (k = 0; k < var->n; k++) {
        // pthread_barrier_wait(var->barrier);

        for (i = var->kin; i < var->kfim; i++) {
            for (j = 0; j < var->n; j++) {
                if (var->path[i + j*var->n] > var->path[i + k*var->n]
                                            + var->path[k + j*var->n]) {
                        var->path[i + j*var->n] = var->path[i + k*var->n]
                                                + var->path[k + j*var->n];
                    }
                }
            }
    }

    free(x);

    pthread_exit(NULL);
}
#else
/* Main computational kernel. The whole function will be timed,
   including the call and return. */
static void kernel_floyd_warshall(int n,
                                  DATA_TYPE POLYBENCH_2D(path, N, N, n, n)) {
    int i, j, k;

    for (k = 0; k < _PB_N; k++) {
        #ifdef USE_OPENMP
        #pragma omp parallel for private(i, j) num_threads(NUM_THREADS)
        #endif
        for (i = 0; i < _PB_N; i++)
            for (j = 0; j < _PB_N; j++)
                path[i][j] = path[i][j] < path[i][k] + path[k][j]
                             ? path[i][j]
                             : path[i][k] + path[k][j];
    }
}
#endif

int main(int argc, char **argv) {
    /* Retrieve problem size. */
    int n = N;

    /* Variable declaration/allocation. */
    POLYBENCH_2D_ARRAY_DECL(path, DATA_TYPE, N, N, n, n);

    /* Initialize array(s). */
    init_array(n, POLYBENCH_ARRAY(path));

    /* Start timer. */
    polybench_start_instruments;

    #ifdef USE_PTHREAD
    /* Inicia pthread */
    pthread_t vetor_thread[NUM_THREADS];
    pthread_barrier_t barrier_floyd;

    pthread_barrier_init(&barrier_floyd, NULL, NUM_THREADS);

    size_t i = 0;

    for (i = 0; i < NUM_THREADS; i++) {
        thread_v *x = (thread_v *)malloc(sizeof(thread_v));

        x->n = n;
        /* Foi necessario tratar a matriz bidimensional como um vetor
        unidimensional pois a matriz está armazenada estaticamente na
        pilha e struct não é capaz de armazenalá */
        x->path = (DATA_TYPE *)POLYBENCH_ARRAY(path);
        x->barrier = &barrier_floyd;

        x->kin = (i * n)/NUM_THREADS;
        x->kfim = ((i+1) * n)/NUM_THREADS;

        pthread_create(&vetor_thread[i], NULL, floyd, x);
    }

    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(vetor_thread[i], NULL);
    }
    #else
    /* Run kernel. */
    kernel_floyd_warshall(n, POLYBENCH_ARRAY(path));
    #endif

    /* Stop and print timer. */
    polybench_stop_instruments;
    polybench_print_instruments;

    /* Prevent dead-code elimination. All live-out data must be printed
       by the function call in argument. */
    polybench_prevent_dce(print_array(n, POLYBENCH_ARRAY(path)));

    /* Be clean. */
    POLYBENCH_FREE_ARRAY(path);

    return 0;
}
