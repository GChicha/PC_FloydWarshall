# Definições de projeto
cmake_minimum_required(VERSION 3.5.1)

project(TrabalhoPC1)

set (TrabalhoPC1_VERSION_MAJOR 1)

# Definição de variavel atribuida pelo usuario
set(USE_OPENMP FALSE CACHE STRING "Some user-specified option")
set(USE_PTHREAD FALSE CACHE STRING "Some user-specified option")
set(NUM_THREADS 8 CACHE STRING "Some user-specified option")
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

add_definitions(-DNUM_THREADS=${NUM_THREADS})

# Constroi a biblioteca do Polybench
add_library(Polybench ./polybench/utilities/polybench)

# Constroi o executavel do FloydWarshall
add_executable(FloydWarshall ./polybench/floyd-warshall/floyd-warshall.c)

# Link bibliotecas
target_link_libraries(FloydWarshall Polybench)

# Se definido o uso do OpenMP ou Pthreads
if(${USE_OPENMP})
    message(STATUS "Usando OpenMP com ${NUM_THREADS} threads")
    add_definitions(-DUSE_OPENMP)
    list(APPEND CMAKE_C_FLAGS "-fopenmp")
elseif(${USE_PTHREAD})
    message("Usando Pthreads")
    add_definitions(-DUSE_PTHREAD)
    list(APPEND CMAKE_C_FLAGS "-pthread")
else()
    message(AUTHOR_WARNING "Não usando paralelismo")
endif()
