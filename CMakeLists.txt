# Definições de projeto
cmake_minimum_required(VERSION 3.0)

project(TrabalhoPC1)

set (TrabalhoPC1_VERSION_MAJOR 1)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_definitions(" -O3 ")

set(CMAKE_C_COMPILER /usr/bin/gcc)

# Set verbose output while testing CMake
# set(CMAKE_VERBOSE_MAKEFILE 1)

# Definição de variavel atribuida pelo usuario
set(PLIB NO CACHE STRING "Biblioteca a ser utilizada")
set(NUM_THREADS 8 CACHE INTEGER "Numero de threads")
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(DATASET_SIZE LARGE CACHE STRING "Tamanho do dataset")

add_definitions(-DNUM_THREADS=${NUM_THREADS})

# Constroi a biblioteca do Polybench
add_library(Polybench ./polybench/utilities/polybench)

# Constroi o executavel do FloydWarshall
add_executable(FloydWarshall ./polybench/floyd-warshall/floyd-warshall.c)

# Link bibliotecas
target_link_libraries(FloydWarshall Polybench)

# Configura PAPI
add_definitions(-DPOLYBENCH_PAPI)
add_definitions(" -lpapi ")
message(STATUS "Rodando com PAPI")

# Imprimi o tempo gasto
add_definitions(-DPOLYBENCH_TIME)
message(STATUS "Saida stdout = tempo")

# Imprimi a saida
add_definitions(-DPOLYBENCH_DUMP_ARRAYS)
message(STATUS "Saida stderr = dump do grafo")

# Tamanho do dataset
add_definitions(-D${DATASET_SIZE}_DATASET)
message(STATUS "Usando dataset de tamanho ${DATASET_SIZE}")

# Se definido o uso do OpenMP ou Pthreads
if(${PLIB} STREQUAL "openmp")
    message(STATUS "Usando OpenMP com ${NUM_THREADS} threads")
    add_definitions(-DUSE_OPENMP)
    add_definitions(" -fopenmp ")
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
