cmake_minimum_required(VERSION 3.10)
project(OME VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(ome_parser
    src/Acceptor.cpp
    src/ClientManager.cpp
    src/Ome.cpp
    src/Reactor.cpp
    src/Client.cpp
    src/Epoll.cpp
    # src/Parser.cpp
    src/Server.cpp)

target_include_directories(ome_parser PUBLIC include/ome)

add_executable(ome main.cpp)
target_link_libraries(ome PRIVATE ome_parser)

# # Tests

# find_package(GTest REQUIRED)

# add_executable(tester
#     tester.cpp)

# target_link_libraries(tester
# PRIVATE
#     ome_parser
#     GTest::gtest_main)

# include(GoogleTest)
# gtest_discover_tests(tester)