# Definições de projeto
cmake_minimum_required(VERSION 3.5.1)

project(TrabalhoPC1)

set (TrabalhoPC1_VERSION_MAJOR 1)

# Set verbose output while testing CMake
# set(CMAKE_VERBOSE_MAKEFILE 1)

# Definição de variavel atribuida pelo usuario
set(PLIB NO CACHE STRING "Biblioteca a ser utilizada")
set(NUM_THREADS 8 CACHE INTEGER "Numero de threads")
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

add_definitions(-DNUM_THREADS=${NUM_THREADS})

# Constroi a biblioteca do Polybench
add_library(Polybench ./polybench/utilities/polybench)

# Constroi o executavel do FloydWarshall
add_executable(FloydWarshall ./polybench/floyd-warshall/floyd-warshall.c)

# Link bibliotecas
target_link_libraries(FloydWarshall Polybench)

# Configura PAPI
# add_definitions(-DPOLYBENCH_PAPI)
# list(APPEND CMAKE_C_FLAGS " -lpapi ")

# Imprimi o tempo gasto
add_definitions(-DPOLYBENCH_TIME)

# Imprimi a saida
add_definitions(-DPOLYBENCH_DUMP_ARRAYS)

# Tamanho do dataset
add_definitions(-DLARGE_DATASET)

# Se definido o uso do OpenMP ou Pthreads
if(${PLIB} STREQUAL "openmp")
    message(STATUS "Usando OpenMP com ${NUM_THREADS} threads")
    add_definitions(-DUSE_OPENMP)
    list(APPEND CMAKE_C_FLAGS " -fopenmp ")
elseif(${PLIB} STREQUAL "pthread")
    message(STATUS "Usando Pthreads com ${NUM_THREADS} threads")
    add_definitions(-DUSE_PTHREAD)
    target_link_libraries(FloydWarshall pthread)
elseif(${PLIB} STREQUAL "seq")
    message(STATUS "Rodando sequencial")
else()
    message(STATUS "Biblioteca ${P_LIB} não identificada")
    message("Use a biblioteca openmp ou pthread ou defina como seq")
    message(WARNING "Não usando paralelismo")
endif()
