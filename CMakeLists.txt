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
add_definitions(" -O3 ")

# Incluí a pasta utilities
include_directories(polybench/utilities)

# Exporta arquivo compile_commands.json para auxiliar linter atom
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Set verbose output while testing CMake
# set(CMAKE_VERBOSE_MAKEFILE 1)

# Definição de variavel atribuida pelo usuario
set(PLIB seq CACHE STRING "Biblioteca a ser utilizada")
set(NUM_THREADS 8 CACHE INTEGER "Numero de threads")
set(DATASET_SIZE LARGE CACHE STRING "Tamanho do dataset")
set(STDOUT papi CACHE STRING "Tipo de saida stdout")
set(DUMP OFF CACHE BOOLEAN "Imprime dump do grafo")

add_definitions(-DNUM_THREADS=${NUM_THREADS})

# Constroi a biblioteca do Polybench
add_library(Polybench ./polybench/utilities/polybench)

# Constroi o executavel do FloydWarshall
add_executable(FloydWarshall ./polybench/floyd-warshall/floyd-warshall.c)

# Link bibliotecas
target_link_libraries(FloydWarshall Polybench)

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

# Se definido o uso do OpenMP ou Pthreads
if(${PLIB} STREQUAL "openmp")
    message(STATUS "Usando OpenMP com ${NUM_THREADS} threads")
    add_definitions(-DUSE_OPENMP)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
elseif(${PLIB} STREQUAL "pthread")
    message(STATUS "Usando Pthreads com ${NUM_THREADS} threads")
    add_definitions(-DUSE_PTHREAD)
    target_link_libraries(FloydWarshall pthread)
elseif(${PLIB} STREQUAL "seq")
    message(STATUS "Rodando sequencial")
else()
    message(WARNING "Biblioteca ${P_LIB} não identificada")
    message("Use a biblioteca openmp ou pthread ou defina como seq")
    message(WARNING "Não usando paralelismo")
endif()
