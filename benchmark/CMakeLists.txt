set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O3") # -O3

# Create the executable
add_executable(TestSuiteBenchmarker test_suite_benchmark.cpp)

# Link the libraries
target_link_libraries(TestSuiteBenchmarker Prometheus-lib Threads::Threads)
