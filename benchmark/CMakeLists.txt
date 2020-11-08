#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O3") # -O3

# Create the executable
add_executable(OrderingBenchmarkRunner ordering.cpp)

# Link the libraries
target_link_libraries(OrderingBenchmarkRunner Prometheus-lib Threads::Threads)
