set(EIGEN_DIR /usr/local/include/eigen3)
include_directories(${EIGEN_DIR})

add_executable(BookMaker opening-book/book_maker.cpp)
add_executable(Tuner tune/tune.cpp tune/tune_runner.cpp)
target_link_libraries(BookMaker Cloak-lib Threads::Threads)
target_link_libraries(Tuner Cloak-lib Threads::Threads)