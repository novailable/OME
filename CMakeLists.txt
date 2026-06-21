cmake_minimum_required(VERSION 3.10)
project(OME VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(ome_parser src/parser/Parser.cpp)
target_include_directories(ome_parser PUBLIC include/ome/parser)

add_executable(ome main.cpp)
target_link_libraries(ome PRIVATE ome_parser)