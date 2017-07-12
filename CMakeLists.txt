# Definições de projeto
cmake_minimum_required(VERSION 3.0)

project(TrabalhoPC1)

set (TrabalhoPC1_VERSION_MAJOR 1)

# Pasta de saidas dos arquivos do CMake
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Bibliotecas do C
LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/res)

# Otimização do compilador
add_definitions(" -O0 ")

# Warn All
add_definitions(" -Wall ")

# Incluí a pasta utilities
include_directories(polybench/utilities)

# Exporta arquivo compile_commands.json para auxiliar linter atom
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Definição de variavel atribuida pelo usuario
set(PLIB seq CACHE STRING "Biblioteca a ser utilizada")
set_property(CACHE PLIB PROPERTY STRINGS "seq;openmp;pthread;mpi")

set(NUM_THREADS 8 CACHE STRING "Numero de threads")
set_property(CACHE NUM_THREADS PROPERTY STRINGS "2;4;8;16;32")

set(DATASET_SIZE LARGE CACHE STRING "Tamanho do dataset")
set_property(CACHE DATASET_SIZE PROPERTY STRINGS "MINI;SMALL;LARGE;EXTRALARGE")

set(STDOUT papi CACHE STRING "Tipo de saida stdout")
set_property(CACHE STDOUT PROPERTY STRINGS "papi;time")

set(DUMP OFF CACHE BOOL "Imprime dump do grafo")

# Constroi a biblioteca do Polybench
add_library(Polybench ./polybench/utilities/polybench)

# Constroi o executavel do FloydWarshall
add_executable(FloydWarshall ./polybench/floyd-warshall/floyd-warshall)

# Link bibliotecas
target_link_libraries(FloydWarshall Polybench)

# Se definido o uso do OpenMP ou Pthreads
if(${PLIB} STREQUAL "openmp")
    # Defime o numero de threads para a biblioteca paralela
    add_definitions(-DNUM_THREADS=${NUM_THREADS})

    message(STATUS "Usando OpenMP com ${NUM_THREADS} threads")
    add_definitions(-DUSE_OPENMP)
    find_package(OpenMP)
    if (OPENMP_FOUND)
        set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    endif()
    add_definitions(-fopenmp)
elseif(${PLIB} STREQUAL "pthread")
    # Defime o numero de threads para a biblioteca paralela
    add_definitions(-DNUM_THREADS=${NUM_THREADS})

    message(STATUS "Usando Pthreads com ${NUM_THREADS} threads")
    add_definitions(-DUSE_PTHREAD)
    target_link_libraries(FloydWarshall pthread)
elseif(${PLIB} STREQUAL "seq")
    message(STATUS "Rodando sequencial")
elseif(${PLIB} STREQUAL "mpi")
    message(STATUS "Rodando usando MPI")

    find_package(MPI REQUIRED)

    include_directories(${MPI_INCLUDE_PATH})

    target_link_libraries(FloydWarshall ${MPI_C_LIBRARIES})

    add_definitions( -DUSE_MPI )
else()
    message(WARNING "Biblioteca ${P_LIB} não identificada")
    message("Use a biblioteca openmp ou pthread ou defina como seq")
    message(WARNING "Não usando paralelismo")
endif()

IF(${DUMP})
    # Imprimi a saida
    add_definitions(-DPOLYBENCH_DUMP_ARRAYS)
    message(STATUS "Saida stderr = dump do grafo")
ENDIF()

# Impede que o papi e saida de tempo seja colocado ao mesmo tempo
IF(${STDOUT} STREQUAL "papi")
    # Configura PAPI
    add_definitions(-DPOLYBENCH_PAPI)
    target_link_libraries(FloydWarshall papi)
    message(STATUS "Saida stdout = PAPI")
elseif(${STDOUT} STREQUAL "time")
    # Imprimi o tempo gasto
    add_definitions(-DPOLYBENCH_TIME)
    message(STATUS "Saida stdout = tempo")
elseif()
    message(WARNING "Saida stdout = ${STDOUT} não identificada\n use time, para tempo de excução
    ou papi para imprimir os contadores papi")
ENDIF()

# Tamanho do dataset
add_definitions(-D${DATASET_SIZE}_DATASET)
message(STATUS "Usando dataset de tamanho ${DATASET_SIZE}")
