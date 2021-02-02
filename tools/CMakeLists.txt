add_executable(BookMaker opening-book/book_maker.cpp)
target_link_libraries(BookMaker Cloak-lib Threads::Threads)