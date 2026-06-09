cmake_minimum_required(VERSION 4.1)
project(Projekt_4)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "0e5b6991-d74f-4b3d-a41c-cf096f0d4a45")
add_compile_options(-O3 -march=native -flto -fno-plt)
add_link_options(-flto -O3 -march=native)

add_executable(Projekt_3 main.cpp)

target_sources(Projekt_3 PUBLIC
        FILE_SET CXX_MODULES FILES
        Abstractions/IAtspAlgorithm.ixx
        Abstractions/IDataProvider.ixx
        Algorithms/NN.ixx
        Algorithms/GeneticAlgorithm.ixx
        Infrastructure/AlgorithmTester.ixx
        Infrastructure/CsvWriter.ixx
        Data/FileDataProvider.ixx
        Data/RandomDataGenerator.ixx
        DefinitelyNotADataStructures/DefinitelyNotAVector.ixx
        Models/TestData.ixx
        Models/AlgorithmResult.ixx
        Models/GAConfig.ixx
)
