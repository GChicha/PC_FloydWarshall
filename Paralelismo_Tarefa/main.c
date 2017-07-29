#include "main.h"

#ifndef MPI_COMM_WORLD
#define MPI_COMM_WORLD 0
#endif

void init_array (double vetor[], int n) {
    int i, j;

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++)
            vetor[i * n + j] = ((double) (i+1)*(j+1)) / n;
    }
}

void kernel_floyd (double vetor[], int n) {
    int i, j, k;
    for (k = 0; k < n; k++) {
        for (i = 0; i < n; i++)
            for (j = 0; j < n; j++)
            vetor[i * n + j] = vetor[i * n + j] < vetor[i * n + k] + vetor[k * n + j]
                                ? vetor[i * n + j]
                                : vetor[i * n + k] + vetor[k * n + j];
    }
}

int main() {
    MPI_Init(NULL, NULL);
    double startPoint = MPI_Wtime();

    srand(MPI_Wtime());

    int rankWorld;
    int sizeWorld;

    MPI_Comm_rank(MPI_COMM_WORLD, &rankWorld);
    MPI_Comm_size(MPI_COMM_WORLD, &sizeWorld);

    MPI_Group worldGroup;
    MPI_Comm_group(MPI_COMM_WORLD, &worldGroup);

    MPI_Group localGroup[4];
    MPI_Comm comunicadores[4];

    int numeroComunicadores = 0;

    int ranks[2];

    int comToSend = -1;

    if (rankWorld == SERVIDOR) {
        ranks[0] = SERVIDOR;
        ranks[1] = MALHA_2_0;
        MPI_Group_incl(worldGroup, 2, ranks, &localGroup[numeroComunicadores]);
        MPI_Comm_create_group(MPI_COMM_WORLD, localGroup[numeroComunicadores], 0, &comunicadores[numeroComunicadores]);
        numeroComunicadores++;
    }
    else if (rankWorld == MALHA_2_0) {
        ranks[0] = MALHA_2_1;
        ranks[1] = MALHA_2_0;
        MPI_Group_incl(worldGroup, 2, ranks, &localGroup[numeroComunicadores]);
        MPI_Comm_create_group(MPI_COMM_WORLD, localGroup[numeroComunicadores], 0, &comunicadores[numeroComunicadores]);
        numeroComunicadores++;

        ranks[0] = MALHA_1_0;
        ranks[1] = MALHA_2_0;
        MPI_Group_incl(worldGroup, 2, ranks, &localGroup[numeroComunicadores]);
        MPI_Comm_create_group(MPI_COMM_WORLD, localGroup[numeroComunicadores], 0, &comunicadores[numeroComunicadores]);
        numeroComunicadores++;

        ranks[0] = SERVIDOR;
        ranks[1] = MALHA_2_0;
        MPI_Group_incl(worldGroup, 2, ranks, &localGroup[numeroComunicadores]);
        MPI_Comm_create_group(MPI_COMM_WORLD, localGroup[numeroComunicadores], 0, &comunicadores[numeroComunicadores]);
        numeroComunicadores++;

        comToSend = 2;
    }
    else if (rankWorld == MALHA_2_1){
        ranks[0] = MALHA_2_2;
        ranks[1] = MALHA_2_1;
        MPI_Group_incl(worldGroup, 2, ranks, &localGroup[numeroComunicadores]);
        MPI_Comm_create_group(MPI_COMM_WORLD, localGroup[numeroComunicadores], 0, &comunicadores[numeroComunicadores]);
        numeroComunicadores++;

        ranks[0] = MALHA_2_1;
        ranks[1] = MALHA_2_0;
        MPI_Group_incl(worldGroup, 2, ranks, &localGroup[numeroComunicadores]);
        MPI_Comm_create_group(MPI_COMM_WORLD, localGroup[numeroComunicadores], 0, &comunicadores[numeroComunicadores]);
        numeroComunicadores++;

        ranks[0] = MALHA_1_1;
        ranks[1] = MALHA_2_1;
        MPI_Group_incl(worldGroup, 2, ranks, &localGroup[numeroComunicadores]);
        MPI_Comm_create_group(MPI_COMM_WORLD, localGroup[numeroComunicadores], 0, &comunicadores[numeroComunicadores]);
        numeroComunicadores++;

        comToSend = 1;
    }
    else if (rankWorld == MALHA_2_2){
        ranks[0] = MALHA_2_2;
        ranks[1] = MALHA_2_1;
        MPI_Group_incl(worldGroup, 2, ranks, &localGroup[numeroComunicadores]);
        MPI_Comm_create_group(MPI_COMM_WORLD, localGroup[numeroComunicadores], 0, &comunicadores[numeroComunicadores]);
        numeroComunicadores++;

        ranks[0] = MALHA_1_2;
        ranks[1] = MALHA_2_2;
        MPI_Group_incl(worldGroup, 2, ranks, &localGroup[numeroComunicadores]);
        MPI_Comm_create_group(MPI_COMM_WORLD, localGroup[numeroComunicadores], 0, &comunicadores[numeroComunicadores]);
        numeroComunicadores++;

        ranks[0] = WORKER_BASE_DIREITO;
        ranks[1] = MALHA_2_2;
        MPI_Group_incl(worldGroup, 2, ranks, &localGroup[numeroComunicadores]);
        MPI_Comm_create_group(MPI_COMM_WORLD, localGroup[numeroComunicadores], 0, &comunicadores[numeroComunicadores]);
        numeroComunicadores++;

        comToSend = 0;
    }
    else if (rankWorld == MALHA_1_2) {
        ranks[0] = MALHA_1_1;
        ranks[1] = MALHA_1_2;
        MPI_Group_incl(worldGroup, 2, ranks, &localGroup[numeroComunicadores]);
        MPI_Comm_create_group(MPI_COMM_WORLD, localGroup[numeroComunicadores], 0, &comunicadores[numeroComunicadores]);
        numeroComunicadores++;

        ranks[0] = MALHA_1_2;
        ranks[1] = MALHA_2_2;
        MPI_Group_incl(worldGroup, 2, ranks, &localGroup[numeroComunicadores]);
        MPI_Comm_create_group(MPI_COMM_WORLD, localGroup[numeroComunicadores], 0, &comunicadores[numeroComunicadores]);
        numeroComunicadores++;

        ranks[0] = MALHA_1_2;
        ranks[1] = MALHA_0_2;
        MPI_Group_incl(worldGroup, 2, ranks, &localGroup[numeroComunicadores]);
        MPI_Comm_create_group(MPI_COMM_WORLD, localGroup[numeroComunicadores], 0, &comunicadores[numeroComunicadores]);
        numeroComunicadores++;

        comToSend = 1;
    }
    else if (rankWorld == MALHA_1_1) {
        ranks[0] = MALHA_1_1;
        ranks[1] = MALHA_1_2;
        MPI_Group_incl(worldGroup, 2, ranks, &localGroup[numeroComunicadores]);
        MPI_Comm_create_group(MPI_COMM_WORLD, localGroup[numeroComunicadores], 0, &comunicadores[numeroComunicadores]);
        numeroComunicadores++;

        ranks[0] = MALHA_0_1;
        ranks[1] = MALHA_1_1;
        MPI_Group_incl(worldGroup, 2, ranks, &localGroup[numeroComunicadores]);
        MPI_Comm_create_group(MPI_COMM_WORLD, localGroup[numeroComunicadores], 0, &comunicadores[numeroComunicadores]);
        numeroComunicadores++;

        ranks[0] = MALHA_1_0;
        ranks[1] = MALHA_1_1;
        MPI_Group_incl(worldGroup, 2, ranks, &localGroup[numeroComunicadores]);
        MPI_Comm_create_group(MPI_COMM_WORLD, localGroup[numeroComunicadores], 0, &comunicadores[numeroComunicadores]);
        numeroComunicadores++;

        ranks[0] = MALHA_1_1;
        ranks[1] = MALHA_2_1;
        MPI_Group_incl(worldGroup, 2, ranks, &localGroup[numeroComunicadores]);
        MPI_Comm_create_group(MPI_COMM_WORLD, localGroup[numeroComunicadores], 0, &comunicadores[numeroComunicadores]);
        numeroComunicadores++;

        comToSend = 3;
    }
    else if (rankWorld == MALHA_1_0) {
        ranks[0] = MALHA_1_0;
        ranks[1] = MALHA_0_0;
        MPI_Group_incl(worldGroup, 2, ranks, &localGroup[numeroComunicadores]);
        MPI_Comm_create_group(MPI_COMM_WORLD, localGroup[numeroComunicadores], 0, &comunicadores[numeroComunicadores]);
        numeroComunicadores++;

        ranks[0] = MALHA_1_0;
        ranks[1] = MALHA_1_1;
        MPI_Group_incl(worldGroup, 2, ranks, &localGroup[numeroComunicadores]);
        MPI_Comm_create_group(MPI_COMM_WORLD, localGroup[numeroComunicadores], 0, &comunicadores[numeroComunicadores]);
        numeroComunicadores++;

        ranks[0] = MALHA_1_0;
        ranks[1] = MALHA_2_0;
        MPI_Group_incl(worldGroup, 2, ranks, &localGroup[numeroComunicadores]);
        MPI_Comm_create_group(MPI_COMM_WORLD, localGroup[numeroComunicadores], 0, &comunicadores[numeroComunicadores]);
        numeroComunicadores++;

        comToSend = 2;
    }
    else if (rankWorld == MALHA_0_0) {
        ranks[0] = MALHA_1_0;
        ranks[1] = MALHA_0_0;
        MPI_Group_incl(worldGroup, 2, ranks, &localGroup[numeroComunicadores]);
        MPI_Comm_create_group(MPI_COMM_WORLD, localGroup[numeroComunicadores], 0, &comunicadores[numeroComunicadores]);
        numeroComunicadores++;

        ranks[0] = MALHA_0_1;
        ranks[1] = MALHA_0_0;
        MPI_Group_incl(worldGroup, 2, ranks, &localGroup[numeroComunicadores]);
        MPI_Comm_create_group(MPI_COMM_WORLD, localGroup[numeroComunicadores], 0, &comunicadores[numeroComunicadores]);
        numeroComunicadores++;

        ranks[0] = WORKER_TOPO_ESQUERDO;
        ranks[1] = MALHA_0_0;
        MPI_Group_incl(worldGroup, 2, ranks, &localGroup[numeroComunicadores]);
        MPI_Comm_create_group(MPI_COMM_WORLD, localGroup[numeroComunicadores], 0, &comunicadores[numeroComunicadores]);
        numeroComunicadores++;

        comToSend = 0;
    }
    else if (rankWorld == MALHA_0_1) {
        ranks[0] = MALHA_0_1;
        ranks[1] = MALHA_0_2;
        MPI_Group_incl(worldGroup, 2, ranks, &localGroup[numeroComunicadores]);
        MPI_Comm_create_group(MPI_COMM_WORLD, localGroup[numeroComunicadores], 0, &comunicadores[numeroComunicadores]);
        numeroComunicadores++;

        ranks[0] = MALHA_0_1;
        ranks[1] = MALHA_0_0;
        MPI_Group_incl(worldGroup, 2, ranks, &localGroup[numeroComunicadores]);
        MPI_Comm_create_group(MPI_COMM_WORLD, localGroup[numeroComunicadores], 0, &comunicadores[numeroComunicadores]);
        numeroComunicadores++;

        ranks[0] = MALHA_0_1;
        ranks[1] = MALHA_1_1;
        MPI_Group_incl(worldGroup, 2, ranks, &localGroup[numeroComunicadores]);
        MPI_Comm_create_group(MPI_COMM_WORLD, localGroup[numeroComunicadores], 0, &comunicadores[numeroComunicadores]);
        numeroComunicadores++;

        comToSend = 2;
    }
    else if (rankWorld == MALHA_0_2) {
        ranks[0] = MALHA_0_1;
        ranks[1] = MALHA_0_2;
        MPI_Group_incl(worldGroup, 2, ranks, &localGroup[numeroComunicadores]);
        MPI_Comm_create_group(MPI_COMM_WORLD, localGroup[numeroComunicadores], 0, &comunicadores[numeroComunicadores]);
        numeroComunicadores++;

        ranks[0] = MALHA_1_2;
        ranks[1] = MALHA_0_2;
        MPI_Group_incl(worldGroup, 2, ranks, &localGroup[numeroComunicadores]);
        MPI_Comm_create_group(MPI_COMM_WORLD, localGroup[numeroComunicadores], 0, &comunicadores[numeroComunicadores]);
        numeroComunicadores++;

        ranks[0] = WORKER_TOPO_DIREITO;
        ranks[1] = MALHA_0_2;
        MPI_Group_incl(worldGroup, 2, ranks, &localGroup[numeroComunicadores]);
        MPI_Comm_create_group(MPI_COMM_WORLD, localGroup[numeroComunicadores], 0, &comunicadores[numeroComunicadores]);
        numeroComunicadores++;

        comToSend = 1;
    }
    else if (rankWorld == WORKER_TOPO_ESQUERDO) {
        ranks[0] = WORKER_TOPO_ESQUERDO;
        ranks[1] = MALHA_0_0;
        MPI_Group_incl(worldGroup, 2, ranks, &localGroup[numeroComunicadores]);
        MPI_Comm_create_group(MPI_COMM_WORLD, localGroup[numeroComunicadores], 0, &comunicadores[numeroComunicadores]);
        numeroComunicadores++;
    }
    else if (rankWorld == WORKER_TOPO_DIREITO) {
        ranks[0] = WORKER_TOPO_DIREITO;
        ranks[1] = MALHA_0_2;
        MPI_Group_incl(worldGroup, 2, ranks, &localGroup[numeroComunicadores]);
        MPI_Comm_create_group(MPI_COMM_WORLD, localGroup[numeroComunicadores], 0, &comunicadores[numeroComunicadores]);
        numeroComunicadores++;
    }
    else if (rankWorld == WORKER_BASE_DIREITO) {
        ranks[0] = WORKER_BASE_DIREITO;
        ranks[1] = MALHA_2_2;
        MPI_Group_incl(worldGroup, 2, ranks, &localGroup[numeroComunicadores]);
        MPI_Comm_create_group(MPI_COMM_WORLD, localGroup[numeroComunicadores], 0, &comunicadores[numeroComunicadores]);
        numeroComunicadores++;
    }

    #ifdef DEBUG
    int i = 7;
    while (i == 7) 
        sleep(5);
    #endif   

    if (rankWorld == SERVIDOR) {
        double startTime, actualTime;
        actualTime = MPI_Wtime();
        startTime = actualTime;

        FILE *arquivoEntrada = fopen(ARQ_NOME, "r");

        int sended = 0;
        double timeWait = 0;

        int myRank, otoRank;
        MPI_Comm_rank(comunicadores[0], &myRank);

        otoRank = (myRank == 0) ? 1 : 0;

        // Buffers para recepção
        MPI_Request receiver;
        mensagem bufferMensagem;

        // Inicia a recepção das respostas
        MPI_Irecv(&bufferMensagem, sizeof(mensagem), MPI_BYTE, otoRank, 0, comunicadores[0], &receiver);

        do {
            actualTime = MPI_Wtime();

            if (actualTime -  startTime >= timeWait) {
                mensagem x;
                x.processado = 0;

                if (fscanf(arquivoEntrada, "%zu %lf", &x.sizeProblem, &timeWait) > 1) {
                    LOG(COLOR_BLUE, "Enviando problema de tamanho %zu e espera %2.3lf", x.sizeProblem, timeWait);

                    double vetor[x.sizeProblem * x.sizeProblem];
                    init_array(vetor, x.sizeProblem); // Inicializa o array conforme o Polybench
                    MPI_Send(&x, sizeof(mensagem), // Envia a mensagem inicial do processamento
                            MPI_BYTE, otoRank, 0, comunicadores[0]);
                    MPI_Send(vetor, x.sizeProblem * x.sizeProblem, // Envia o vetor criado para processamento
                            MPI_DOUBLE, otoRank, 0, comunicadores[0]);
                    startTime = MPI_Wtime(); // Inicia o contador de tempo para enviar proxima mensagem
                    sended++; // Incrementa o numero de mensagens enviadas para saber o tempo de parar o codigo
                }
            }

            int flag = 0; // Flag se recebe alguma

            MPI_Test(&receiver, &flag, MPI_STATUS_IGNORE);

            if (flag) { // Se recebeu então processa e decrementa o contador de mensagens
                LOG(COLOR_YELLOW, "Confirmação de processamento do problema de tamanho %zu", bufferMensagem.sizeProblem);

                double vetor[bufferMensagem.sizeProblem * bufferMensagem.sizeProblem];

                MPI_Recv(vetor, bufferMensagem.sizeProblem * bufferMensagem.sizeProblem,
                        MPI_DOUBLE, otoRank, 0, comunicadores[0], MPI_STATUS_IGNORE);

                // Processa a informação recebida em bufferMensagem 

                // Reinializa a recepção
                MPI_Irecv(&bufferMensagem, sizeof(mensagem), MPI_BYTE, otoRank, 0, comunicadores[0], &receiver);

                sended--;
            }
        } while (actualTime - startTime < timeWait || sended > 0);

        fclose(arquivoEntrada);

        LOG(COLOR_RED, "Inicia a finalização do programa");

        int killFlag = 1;
        MPI_Send(&killFlag, 1, MPI_INT, otoRank, KILL_TAG, comunicadores[0]);
    }
    else if (rankWorld >= MALHA_0_0 && rankWorld <= MALHA_2_2) { // Loop da malha
        int killFlag = 0;

        mensagem bufferMensagem[numeroComunicadores];
        MPI_Request handlers[numeroComunicadores];

        MPI_Request killRequest[numeroComunicadores];

        int workerWorking = FALSE;

        // Inicializa os receptor do fim e de mensagens
        for (size_t i = 0; i < numeroComunicadores; i++) {
            int myRank;
            MPI_Comm_rank(comunicadores[i], &myRank);
            int otoRank = (myRank == 0) ? 1 : 0;

            MPI_Irecv(&killFlag, 1, MPI_INT, otoRank, KILL_TAG, comunicadores[i], &killRequest[i]);

            MPI_Irecv(&bufferMensagem[i], sizeof(mensagem),
                    MPI_BYTE, otoRank, 0, comunicadores[i], &handlers[i]);
        }

        int flagKill;

        while(MPI_Testall(numeroComunicadores, killRequest, &flagKill, MPI_STATUSES_IGNORE) == MPI_SUCCESS && !killFlag) {
            for (size_t i = 0; i < numeroComunicadores; i++) { // Loop para buscar e transmitir as mensagens
                int flagRecv = 0; // Flag para marcar o recebimento de uma mensagem, inicialmente 0

                int myRank;
                MPI_Comm_rank(comunicadores[i], &myRank);
                int otoRank = (myRank == 0) ? 1 : 0;

                MPI_Test(&handlers[i], &flagRecv, MPI_STATUS_IGNORE);

                if (flagRecv) {
                    if (i == numeroComunicadores - 1)
                        workerWorking = FALSE;

                    LOG(COLOR_CYAN, "Rank: %d recebeu o problema de tamanho %zu com flag de procesamento %d e tempo de vida %zu",
                            rankWorld,
                            bufferMensagem[i].sizeProblem,
                            bufferMensagem[i].processado,
                            bufferMensagem[i].tof);

                    double vetor[bufferMensagem[i].sizeProblem * bufferMensagem[i].sizeProblem];
                    MPI_Recv(vetor, bufferMensagem[i].sizeProblem * bufferMensagem[i].sizeProblem,
                            MPI_DOUBLE, otoRank, 0, comunicadores[i], MPI_STATUS_IGNORE);

                    // Processa o vetor que acabou de chegar no bufferMensagem[i]

                    bufferMensagem[i].tof++; // Incrementa o tempo de vida

                    // Reinicializa a recpção
                    MPI_Irecv(&bufferMensagem[i], sizeof(mensagem),
                            MPI_BYTE, otoRank, 0, comunicadores[i], &handlers[i]);

                    if (!bufferMensagem[i].processado) {
                        if (!workerWorking && 
                                (rankWorld == MALHA_0_0 ||
                                 rankWorld == MALHA_0_2 ||
                                 rankWorld == MALHA_2_2)) {
                            int myNewRank;
                            MPI_Comm_rank(comunicadores[numeroComunicadores - 1], &myNewRank);
                            int otoNewRank = (myNewRank == 0) ? 1 : 0;

                            MPI_Send(&bufferMensagem[i], sizeof(mensagem),
                                    MPI_BYTE, otoNewRank, 0, comunicadores[numeroComunicadores - 1]);
                            MPI_Send(vetor, bufferMensagem[i].sizeProblem * bufferMensagem[i].sizeProblem,
                                    MPI_DOUBLE, otoNewRank, 0, comunicadores[numeroComunicadores - 1]);
                            workerWorking = TRUE;
                        }
                        else {
                            int max = numeroComunicadores - 1;
                            int nextCom;

                            if (rankWorld == MALHA_0_0 ||
                                    rankWorld == MALHA_2_2 ||
                                    rankWorld ==  MALHA_0_2 ||
                                    rankWorld == MALHA_2_0)
                                max--;

                            do {
                                nextCom = rand() % (max + 1);
                            } while (nextCom == i);

                            int myNewRank;
                            MPI_Comm_rank(comunicadores[nextCom], &myNewRank);
                            int otoNewRank = (myNewRank == 0) ? 1 : 0;

                            MPI_Send(&bufferMensagem[i], sizeof(mensagem),
                                    MPI_BYTE, otoNewRank, 0, comunicadores[nextCom]);
                            MPI_Send(vetor, bufferMensagem[i].sizeProblem * bufferMensagem[i].sizeProblem,
                                    MPI_DOUBLE, otoNewRank, 0, comunicadores[nextCom]);
                        }
                    }
                    else {
                            int myNewRank;
                            MPI_Comm_rank(comunicadores[comToSend], &myNewRank);
                            int otoNewRank = (myNewRank == 0) ? 1 : 0;

                            MPI_Send(&bufferMensagem[i], sizeof(mensagem),
                                    MPI_BYTE, otoNewRank, 0, comunicadores[comToSend]);
                            MPI_Send(vetor, bufferMensagem[i].sizeProblem * bufferMensagem[i].sizeProblem,
                                    MPI_DOUBLE, otoNewRank, 0, comunicadores[comToSend]);
                    }
                }
            }
        }

        for (size_t i = 0; i < numeroComunicadores; i++) {
            MPI_Cancel(&handlers[i]);
            MPI_Cancel(&killRequest[i]);

            int myRank;
            MPI_Comm_rank(comunicadores[0], &myRank);
            int otoRank = (myRank == 0) ? 1 : 0;

            MPI_Send(&killFlag, 1, MPI_INT, otoRank, KILL_TAG, comunicadores[i]);
        }
    }
    else if (rankWorld > MALHA_2_2) {
        int killFlag = 0;

        int myRank;
        MPI_Comm_rank(comunicadores[0], &myRank);
        int otoRank = (myRank == 0) ? 1 : 0;

        MPI_Request killRequest, recvRequest;
        MPI_Irecv(&killFlag, 1, MPI_INT, otoRank, KILL_TAG, comunicadores[0], &killRequest);

        mensagem x;

        MPI_Irecv(&x, sizeof(mensagem),
                MPI_BYTE, otoRank, 0, comunicadores[0], &recvRequest);

        int flag, flagRecv = 0;

        while (MPI_Test(&killRequest, &flag, MPI_STATUS_IGNORE) == MPI_SUCCESS && !killFlag) {
            if (MPI_Test(&recvRequest, &flagRecv, MPI_STATUS_IGNORE) == MPI_SUCCESS && flagRecv) {
                double vetor[x.sizeProblem * x.sizeProblem];
                MPI_Recv(vetor, x.sizeProblem * x.sizeProblem,
                        MPI_DOUBLE, otoRank, 0, comunicadores[0], MPI_STATUS_IGNORE);

                LOG(COLOR_MAGENTA, "Processador %d iniciou processamento do problema de tamanho %zu", rankWorld, x.sizeProblem);

                kernel_floyd(vetor, x.sizeProblem);

                x.processado = TRUE;

                MPI_Send(&x, sizeof(mensagem), MPI_BYTE, otoRank, 0, comunicadores[0]);
                MPI_Send(vetor, x.sizeProblem * x.sizeProblem, MPI_DOUBLE, otoRank, 0, comunicadores[0]);

                MPI_Irecv(&x, sizeof(mensagem),
                        MPI_BYTE, otoRank, 0, comunicadores[0], &recvRequest);
            }
        }

        MPI_Cancel(&recvRequest);
        MPI_Send(&killFlag, 1, MPI_INT, otoRank, KILL_TAG, comunicadores[0]);
    }

    LOG(COLOR_RED, "Rank %d Encerrou", rankWorld);

    MPI_Finalize();
    return 0;
}
