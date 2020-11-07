# Create the executable
add_executable(OrderingBenchmarkRunner ordering.cpp)

# Link the libraries
target_link_libraries(OrderingBenchmarkRunner Prometheus-lib Threads::Threads)
