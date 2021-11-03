set(EIGEN_DIR /usr/local/include/eigen3)
include_directories(${EIGEN_DIR})
find_package(Matplot++ REQUIRED)


add_executable(BookMaker opening-book/book_maker.cpp)
add_executable(TrainBook opening-book/train_book.cpp)
add_executable(Tuner tune/tuner.cpp)


target_link_libraries(BookMaker Prometheus-lib Threads::Threads)
target_link_libraries(TrainBook Prometheus-lib Threads::Threads)
target_link_libraries(Tuner Matplot++::matplot Prometheus-lib Threads::Threads)
