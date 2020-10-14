# Define CMake version, project name, and C++ standard
cmake_minimum_required(VERSION 3.13)
project(ChessEngine)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Must include to avoid failing on Linux with std::thread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Import Packages
find_package(GTest REQUIRED) # Unit-Testing Library
# find_package(benchmark REQUIRED) # Benchmarking Library
find_package(spdlog CONFIG REQUIRED) # Logging Library

# Include Library Directories
include_directories(${GTEST_INCLUDE_DIR})

# Options
option(CODE_COVERAGE "Enable Code Coverage" OFF)
option(BUILD_TESTS "Build Tests" ON)
option(BENCH_TESTS "Build Benchmarks" OFF)
option(LOGGING "Enable Logging" ON) # Disable on Final Release

# If CODE_COVERAGE has been enabled, then update compilation flags
if(CODE_COVERAGE)
    message(STATUS "Enabling Code Coverage")
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -pedantic") # -Werror
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O3")
endif(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

########################################################################################################################
################################################# PROMETHEUS / ENGINE ##################################################
########################################################################################################################
set(PROMETHEUS_LIB_SRCS
        src/attack.cpp
        src/bitmask.cpp
        src/board.cpp
        src/book.cpp
        src/chess_clock.cpp
        src/chess_hash.cpp
        src/chess_move.cpp
        src/eval.cpp
        src/move_gen.cpp
        src/protocol.cpp
        src/search.cpp
        src/string_manip.cpp
        src/uci_options.cpp
        )

add_library(Prometheus-lib ${PROMETHEUS_LIB_SRCS})
set_target_properties(Prometheus-lib PROPERTIES PREFIX "")
target_include_directories(Prometheus-lib PUBLIC ${CMAKE_SOURCE_DIR}/include ${CMAKE_BINARY_DIR}/include ${GTEST_INCLUDE_DIR} ${SPDLOG_INCLUDE_DIR})
add_executable(Prometheus src/main.cpp)
target_link_libraries(Prometheus Prometheus-lib Threads::Threads spdlog::spdlog)
install(TARGETS Prometheus DESTINATION bin)
install(TARGETS Prometheus-lib ARCHIVE DESTINATION lib LIBRARY DESTINATION lib RUNTIME DESTINATION bin)

if(BUILD_TESTS)
    #    add_subdirectory(benchmark/engine)
    add_subdirectory(test)
    if (CODE_COVERAGE MATCHES ON)
        include(cmake/CodeCoverage.cmake)
        target_link_libraries(Prometheus-lib --coverage)
        append_coverage_compiler_flags()
        setup_target_for_coverage_gcovr_html(NAME AttackCoverage EXECUTABLE AttackTestRunner BASE_DIRECTORY "${PROJECT_SOURCE_DIR}/src")
        setup_target_for_coverage_gcovr_html(NAME BitmaskCoverage EXECUTABLE BitmaskTestRunner BASE_DIRECTORY "${PROJECT_SOURCE_DIR}/src")
        setup_target_for_coverage_gcovr_html(NAME BoardCoverage EXECUTABLE BoardTestRunner BASE_DIRECTORY "${PROJECT_SOURCE_DIR}/src")
        setup_target_for_coverage_gcovr_html(NAME ChessClockCoverage EXECUTABLE ChessClockTestRunner BASE_DIRECTORY "${PROJECT_SOURCE_DIR}/src")
        setup_target_for_coverage_gcovr_html(NAME ChessMoveCoverage EXECUTABLE ChessMoveTestRunner BASE_DIRECTORY "${PROJECT_SOURCE_DIR}/src")
        setup_target_for_coverage_gcovr_html(NAME StringManipCoverage EXECUTABLE StringManipTestRunner BASE_DIRECTORY "${PROJECT_SOURCE_DIR}/src")
        setup_target_for_coverage_gcovr_html(NAME UCIOptionsCoverage EXECUTABLE UCIOptionsTestRunner BASE_DIRECTORY "${PROJECT_SOURCE_DIR}/src")
        setup_target_for_coverage_gcovr_html(NAME PerftCoverage EXECUTABLE PerftTestRunner BASE_DIRECTORY "${PROJECT_SOURCE_DIR}/src")
        setup_target_for_coverage_gcovr_html(NAME EvalCoverage EXECUTABLE EvalTestRunner BASE_DIRECTORY "${PROJECT_SOURCE_DIR}/src")
    endif()
endif()