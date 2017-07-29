#ifndef _MAIN_HEADER
#define _MAIN_HEADER
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#ifndef ARQ_NOME
#define ARQ_NOME "input.txt"
#endif

#define TRUE 1
#define FALSE 0

typedef struct mensagem {
    size_t sizeProblem;
    int processado;
    size_t tof;
} mensagem;

#define CURTIME time(&curTime)
#define lTime localtime(&curTime)

#define LOG(COR, STR, ...)  CURTIME;printf("[%02d:%02d:%02d]" COR STR COLOR_RESET "\n", lTime->tm_hour, lTime->tm_min, lTime->tm_sec, ##__VA_ARGS__)

#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_RESET   "\x1b[0m"

#define KILL_TAG 2

// Definição dos Nós da Malha
#define SERVIDOR 9

#define MALHA_2_0 6
#define MALHA_2_1 7
#define MALHA_1_0 3
#define MALHA_2_2 8
#define MALHA_1_1 4
#define MALHA_1_2 5
#define MALHA_0_0 0
#define MALHA_0_1 1
#define MALHA_0_2 2

#define WORKER_TOPO_ESQUERDO 10
#define WORKER_TOPO_DIREITO 11
#define WORKER_BASE_DIREITO 12

#endif
