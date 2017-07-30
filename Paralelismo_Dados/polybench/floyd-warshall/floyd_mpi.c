#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

#ifndef MASTER
#define MASTER 0
#endif

#define DATA_PRINTF_MODIFIER "%0.2lf "
#define DATA_TYPE double

#ifndef N
#define N 32
#endif

#define MPI_WTIME_IS_GLOBAL 1

DATA_TYPE path[N][N];

int cmpDouble(const void *a, const void *b) {
    return (*(double *)a - *(double *)b);
}

void init_array (int n)
{
    int i, j;

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++)
            path[i][j] = ((DATA_TYPE) (i+1)*(j+1)) / n;
    }
}

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

typedef struct thread_var {
    int kin, kfim;
    int n;

    int k;
} thread_v;

void floyd(void *x) {
    thread_v *var = (thread_v *) x;

    int n = var->n;
    int i, j, k;

    for (k = var->k; k < var->k + 1; k++) {
        for (i = var->kin; i < var->kfim; i++)
            for (j = 0; j < n; j++)
                path[i][j] = path[i][j] < path[i][k] + path[k][j]
                             ? path[i][j]
                             : path[i][k] + path[k][j];
    }
}

int main()
{
    size_t n = N;

	int size, rank;

	MPI_Init(NULL, NULL);

	MPI_Comm_size(MPI_COMM_WORLD, &size);	
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);	

    int displs[size];
    int recvcout[size];

    for (size_t i = 0; i < size; i++) {
        displs[i] = 0;
        recvcout[i] = 0;
    }

    displs[0] = 0;
    recvcout[0] = (n % size) * n;

    for (size_t i = 0; i < (size_t)size; i++) {
        if (i > 0)
            displs[i] = displs[i - 1] + recvcout[i - 1];
        recvcout[i] += (n / size) * n;   
    }

    double timeCom[12];
    double timeSem[12];

    for (size_t i = 0; i < 12; i++) {
        init_array (n);

        double startTime = MPI_Wtime();
        double tempoTransmissao;
        for (size_t k = 0; k < n; k++){
            thread_v x;

            x.kin = (displs[rank] / n);
            x.kfim = (recvcout[rank] / n) + (displs[rank] / n);

            x.k = k;
            x.n = n;

            floyd(&x);

            tempoTransmissao = MPI_Wtime();
            MPI_Gatherv(path[x.kin], recvcout[rank], MPI_DOUBLE, path, recvcout, displs, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
            tempoTransmissao = MPI_Wtime() - tempoTransmissao;
        }
        timeCom[i] = MPI_Wtime() - startTime;
        timeSem[i] = MPI_Wtime() - (startTime + tempoTransmissao);
    }


    qsort(timeCom, 12, sizeof(double), cmpDouble);
    qsort(timeSem, 12, sizeof(double), cmpDouble);

    double mediaCom = 0;
    double mediaSem = 0;

    for (size_t i = 1; i < 11; i++) {
        mediaCom += timeCom[i];
        mediaSem += timeSem[i];
    }

    mediaCom /= 10;
    mediaSem /= 10;

    MPI_Send(&mediaCom, 1, MPI_DOUBLE, MASTER, 0, MPI_COMM_WORLD);
    MPI_Send(&mediaSem, 1, MPI_DOUBLE, MASTER, 0, MPI_COMM_WORLD);

    if (rank == MASTER) {
        double maiorCom = mediaCom;   
        double maiorSem = mediaSem;   
        for (size_t i = 0; i < size; i++ ){
            double temp;

            MPI_Recv(&temp, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            maiorCom = (temp > maiorCom) ? temp : maiorCom;

            MPI_Recv(&temp, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            maiorSem = (temp > maiorSem) ? temp : maiorSem;
        }

        printf("%lf %lf\n", maiorCom, maiorSem);

#       ifdef DUMP
        print_array(n);
#       endif
    }

    MPI_Finalize();
}
