set(EIGEN_DIR /usr/local/include/eigen3)

include_directories(${EIGEN_DIR})
find_package(Matplot++ REQUIRED)
#find_package(HDF5 REQUIRED COMPONENTS CXX HL)

add_executable(BookMaker opening-book/book_maker.cpp)
add_executable(TrainBook opening-book/train_book.cpp)
add_executable(Tuner tune/tuner.cpp)

#include_directories(${HDF5_INCLUDE_DIRS})

target_link_libraries(BookMaker Prometheus-lib Threads::Threads)
target_link_libraries(TrainBook Prometheus-lib Threads::Threads)
target_link_libraries(Tuner Prometheus-lib Threads::Threads)
