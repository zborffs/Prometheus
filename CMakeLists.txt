# Define CMake version, project name, and C++ standard
cmake_minimum_required(VERSION 3.13)
project(AsterionEngine)
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

# Put all source files excluding main.cpp into library source variable
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

# Create library, link external libraries against it
add_library(Prometheus-lib ${PROMETHEUS_LIB_SRCS})
set_target_properties(Prometheus-lib PROPERTIES PREFIX "")
target_include_directories(Prometheus-lib PUBLIC ${CMAKE_SOURCE_DIR}/include ${CMAKE_BINARY_DIR}/include ${GTEST_INCLUDE_DIR} ${SPDLOG_INCLUDE_DIR})

# Create Main Executable
add_executable(Prometheus src/main.cpp)
target_link_libraries(Prometheus Prometheus-lib Threads::Threads spdlog::spdlog)
install(TARGETS Prometheus DESTINATION bin)
install(TARGETS Prometheus-lib ARCHIVE DESTINATION lib LIBRARY DESTINATION lib RUNTIME DESTINATION bin)

# If we want to build a test, then build them
if(BUILD_TESTS)
    #    add_subdirectory(benchmark/engine)
    add_subdirectory(test)

    # If we want to do code coverage, then do it
    if (CODE_COVERAGE MATCHES ON)
        include(cmake/CodeCoverage.cmake)
        target_link_libraries(Prometheus-lib --coverage)
        append_coverage_compiler_flags()

        setup_target_for_coverage_lcov(NAME BitmaskCoverage EXECUTABLE BitmaskTestRunner BASE_DIRECTORY "${PROJECT_SOURCE_DIR}/src")
        setup_target_for_coverage_lcov(NAME BoardCoverage EXECUTABLE BoardTestRunner BASE_DIRECTORY "${PROJECT_SOURCE_DIR}/src")
        setup_target_for_coverage_lcov(NAME ChessClockCoverage EXECUTABLE ChessClockTestRunner BASE_DIRECTORY "${PROJECT_SOURCE_DIR}/src")
        setup_target_for_coverage_lcov(NAME ChessMoveCoverage EXECUTABLE ChessMoveTestRunner BASE_DIRECTORY "${PROJECT_SOURCE_DIR}/src")
        setup_target_for_coverage_lcov(NAME StringManipCoverage EXECUTABLE StringManipTestRunner BASE_DIRECTORY "${PROJECT_SOURCE_DIR}/src")
        setup_target_for_coverage_lcov(NAME UCIOptionsCoverage EXECUTABLE UCIOptionsTestRunner BASE_DIRECTORY "${PROJECT_SOURCE_DIR}/src")
        setup_target_for_coverage_lcov(NAME PerftCoverage EXECUTABLE PerftTestRunner BASE_DIRECTORY "${PROJECT_SOURCE_DIR}/src" EXECUTABLE_ARGS data/perft/coverage.perft)
        setup_target_for_coverage_lcov(NAME EvalCoverage EXECUTABLE EvalTestRunner BASE_DIRECTORY "${PROJECT_SOURCE_DIR}/src" EXECUTABLE_ARGS data/eval-test/FullEvalTest.fen data/eval-test/NearlyFullEvalTest.fen)
        setup_target_for_coverage_lcov(NAME SearchCoverage EXECUTABLE SearchTestRunner BASE_DIRECTORY "${PROJECT_SOURCE_DIR}/src" EXECUTABLE_ARGS data/puzzles/mate-in-2.puz)
    endif()
endif()