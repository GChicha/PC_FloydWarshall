# Definições de projeto
cmake_minimum_required(VERSION 3.5.1)

project(TrabalhoPC1)

set (TrabalhoPC1_VERSION_MAJOR 1)

# Definição de variavel atribuida pelo usuario
set(P_LIB NO CACHE STRING "Biblioteca a ser utilizada")
set(NUM_THREADS 8 CACHE INTEGER "Numero de threads")
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

add_definitions(-DNUM_THREADS=${NUM_THREADS})

# Constroi a biblioteca do Polybench
add_library(Polybench ./polybench/utilities/polybench)

# Constroi o executavel do FloydWarshall
add_executable(FloydWarshall ./polybench/floyd-warshall/floyd-warshall.c)

# Link bibliotecas
target_link_libraries(FloydWarshall Polybench)

# Se definido o uso do OpenMP ou Pthreads
if(${P_LIB} STREQUAL "openmp")
    message(STATUS "Usando OpenMP com ${NUM_THREADS} threads")
    add_definitions(-DUSE_OPENMP)
    list(APPEND CMAKE_C_FLAGS "-fopenmp")
elseif(${P_LIB} STREQUAL "pthread")
    message(STATUS "Usando Pthreads com ${NUM_THREADS} threads")
    add_definitions(-DUSE_PTHREAD)
    target_link_libraries(FloydWarshall pthread)
else()
    message(STATUS "Biblioteca ${P_LIB} não identificada")
    message("Use a biblioteca openmp ou pthread")
    message(WARNING "Não usando paralelismo")
endif()
