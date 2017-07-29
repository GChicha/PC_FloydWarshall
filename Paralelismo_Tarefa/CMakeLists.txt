# Definições de projeto
cmake_minimum_required(VERSION 3.0)

project(TrabalhoPC1-ParalelismoTarefa)

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
include_directories(utilities)

# Exporta arquivo compile_commands.json para auxiliar linter atom
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(DUMP OFF CACHE BOOL "Imprime dump do grafo")

# Constroi a biblioteca do Polybench
# add_library(Polybench utilities/polybench)

# Constroi a biblioteca do FloydWarshall
# add_library(FloydWarshall floyd-warshall/floyd-warshall)

# Constroi o executavel do cluster
add_executable(Main main)

# Link bibliotecas
# target_link_libraries(FloydWarshall Polybench)

# Se definido o uso do OpenMP ou Pthreads
find_package(MPI REQUIRED)

include_directories(${MPI_INCLUDE_PATH})

target_link_libraries(Main ${MPI_C_LIBRARIES})

IF(${DUMP})
    # Imprimi a saida
    add_definitions(-DPOLYBENCH_DUMP_ARRAYS)
    message(STATUS "Saida stderr = dump do grafo")
ENDIF()

# Imprimi o tempo gasto
add_definitions(-DPOLYBENCH_TIME)
message(STATUS "Saida stdout = tempo")
