#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O3") # -O3

# Create the executable
add_executable(OrderingBenchmarkRunner ordering.cpp)
add_executable(TestSuiteRunner testsuite.cpp)

# Link the libraries
target_link_libraries(OrderingBenchmarkRunner Cloak-lib Threads::Threads)
target_link_libraries(TestSuiteRunner Cloak-lib Threads::Threads)
