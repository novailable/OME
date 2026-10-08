cmake_minimum_required(VERSION 3.14)

project(OME VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

find_package(Threads REQUIRED)


# ============================================================
# Core
# ============================================================

set(OME_CORE_SOURCES
    src/core/Ome.cpp
	src/test/parser_test.cpp
)


# ============================================================
# Concurrency
# ============================================================

# set(OME_CONCURRENCY_SOURCES
#     src/concurrency/Spscqueue.cpp
# )


# ============================================================
# FIX
# ============================================================

set(OME_FIX_SOURCES
    # src/fix/FixMsg.cpp
    src/fix/Parser.cpp
)


# ============================================================
# Network
# ============================================================

set(OME_NETWORK_SOURCES
    src/network/Acceptor.cpp
    src/network/Client.cpp
    src/network/ClientManager.cpp
    src/network/Server.cpp
)


# ============================================================
# Order
# ============================================================

# set(OME_ORDER_SOURCES
#     src/order/Order.cpp
#     src/order/OrderManager.cpp
# )


# ============================================================
# Reactor
# ============================================================

set(OME_REACTOR_SOURCES
    src/reactor/Reactor.cpp
)

if(APPLE)
    list(APPEND OME_REACTOR_SOURCES src/reactor/Kqueue.cpp)
else()
    list(APPEND OME_REACTOR_SOURCES src/reactor/Epoll.cpp)
endif()


# ============================================================
# All sources
# ============================================================

set(OME_SOURCES
    ${OME_CORE_SOURCES}
    ${OME_CONCURRENCY_SOURCES}
    ${OME_FIX_SOURCES}
    ${OME_NETWORK_SOURCES}
    # ${OME_ORDER_SOURCES}
    ${OME_REACTOR_SOURCES}
)


# ============================================================
# OME Library
# ============================================================

add_library(ome_core ${OME_SOURCES})

target_include_directories(ome_core
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/include/core
        ${PROJECT_SOURCE_DIR}/include/fix
        ${PROJECT_SOURCE_DIR}/include/network
        ${PROJECT_SOURCE_DIR}/include/order
        ${PROJECT_SOURCE_DIR}/include/reactor
		${PROJECT_SOURCE_DIR}/include/concurrency
)

target_link_libraries(ome_core
    PUBLIC
        Threads::Threads
)

target_compile_options(ome_core
    PRIVATE
        -Wall #-Wextra -Wpedantic
)


# ============================================================
# OME Application
# ============================================================

add_executable(ome main.cpp)

target_link_libraries(ome
    PRIVATE
        ome_core
)


# ============================================================
# Tests
# ============================================================

option(OME_BUILD_TESTS "Build OME tests" OFF)

if(OME_BUILD_TESTS)

    enable_testing()

    find_package(GTest REQUIRED)

    add_executable(ome_tests
        src/test/parser_test.cpp
        # src/test/spscqueue_test.cpp   # add once the file exists
    )

    target_link_libraries(ome_tests
        PRIVATE
            ome_core
            GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(ome_tests)

endif()
