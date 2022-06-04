set(EIGEN_DIR /usr/local/include/eigen3)

include_directories(${EIGEN_DIR})

add_executable(BookMaker opening-book/book_maker.cpp)
add_executable(TrainBook opening-book/train_book.cpp)

target_link_libraries(BookMaker Prometheus-lib Threads::Threads)
target_link_libraries(TrainBook Prometheus-lib Threads::Threads)
