# Define CMake version, project name, and C++ standard
cmake_minimum_required(VERSION 3.14)
project(Prometheus CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Must include to avoid failing on Linux with std::thread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Import Packages
find_package(GTest REQUIRED) # Unit-Testing Library
find_package(benchmark REQUIRED) # Benchmarking Library
find_package(spdlog REQUIRED) # Logging Library

# Include Library Directories
include_directories(${GTEST_INCLUDE_DIR})
include_directories(include)

#
if (USE_SSE)
    add_compile_definitions(USE_SSE)
    add_compile_definitions(__BMI2__)
    message("-- Compiling using SSE")
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -pedantic -march=native") # -Werror
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g") # -O3
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
    src/chess_clock.cpp
    src/chess_hash.cpp
    src/chess_move.cpp
    src/eval.cpp
    src/magic.cpp
    src/move_gen.cpp
    src/protocol.cpp
    src/search.cpp
    src/string_manip.cpp
    src/uci_options.cpp
    src/transposition_table.cpp
    src/book.cpp
)

# Create library, link external libraries against it
add_library(Prometheus-lib ${PROMETHEUS_LIB_SRCS})
target_include_directories(Prometheus-lib PUBLIC ${CMAKE_SOURCE_DIR}/include ${CMAKE_BINARY_DIR}/include ${GTEST_INCLUDE_DIR} ${SPDLOG_INCLUDE_DIR})
target_link_libraries(Prometheus-lib Threads::Threads spdlog::spdlog)

# Create Main Executable
add_executable(prometheus src/main.cpp)
target_link_libraries(prometheus Prometheus-lib Threads::Threads spdlog::spdlog)

# Add all the subdirectories
add_subdirectory(test) # correctness testing
add_subdirectory(tools) # miscellaneous code
add_subdirectory(benchmark) # performance testing through benchmarking