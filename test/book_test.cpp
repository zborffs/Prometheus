/// Internal Includes
#include "globals.hpp"
#include "book.hpp"
#include "board.hpp"

/// STL Includes
#include <fstream>

/// external includes
#include <gtest/gtest.h>
#include <cereal/archives/binary.hpp>

std::string bookpath;

class BookTester : public ::testing::Test {
protected:
    Book book;
    Board board;
    std::string filename{};

    void SetUp() override {
        // in the setup to all tests, load the book object from memory
        board.set_board();
        std::ifstream f(bookpath, std::ios::binary);
        cereal::BinaryInputArchive iarchive(f); // Create an input archive
        iarchive(book); // Read the data from the archive
        std::cout << "Start of Test" << std::endl;
    }

};

TEST_F(BookTester, AllFirstMoves) {
    auto edges = book.edges(board.key());
    for (int i = 0; i < edges.size(); ++i) {
        std::cout << edges[i] << std::endl;
    }
    std::cout << std::endl;

    book.make_move(edges[0].child_node);
    board.make_move(edges[0].move);
    auto edges2 = book.edges(board.key());
    for (int i = 0; i < edges2.size(); ++i) {
        std::cout << edges2[i] << std::endl;
    }
    EXPECT_EQ(0, 0);
}

// Execute: "./BoardTestRunner data/PrometheusOpening.book"
int main(int argc, char** argv) {
    std::string path(argv[0]);
#ifdef WINDOWS
    std::string base(path.substr(0, path.find_last_of("\\")));
#else
    std::string base(path.substr(0, path.find_last_of('/')));
#endif // WINDOWS

    path = std::string(argv[0]);
    bool first_is_slash = path[0] == '/';
    auto splitvec = split(path, '/');
    assert(!splitvec.empty());
    std::string s{""};
    if (first_is_slash) {
        s += "/" + splitvec[0];
    } else {
        s += splitvec[0];
    }

    for (int i = 1; i < splitvec.size() - 1; i++) {
        s += std::string("/" + splitvec[i]);
    }
    bookpath = std::string(s + "/../../" + argv[1]);

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}