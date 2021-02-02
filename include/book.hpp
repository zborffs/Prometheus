#ifndef CLOAK_BOOK_HPP
#define CLOAK_BOOK_HPP

#include "defines.hpp"
#include "extern.hpp"
#include "chess_move.hpp"

struct BookEdge {
    ChessMove move;
    std::size_t child_node; // index of the next BookNode
};

struct BookNode {
    PositionKey key;
    std::vector<BookEdge> edges;
    int games_played;
    int games_won;

    bool operator==(const BookNode& node) const {
        return node.key == key;
    }
};

class Book {
    std::vector<BookNode> nodes_;
    std::stack<BookNode> former_nodes_;

public:
    Book();
    BookNode* book_move(PositionKey key);
    void make_book_move(std::size_t index);
    void unmake_book_move();
};

#endif // CLOAK_BOOK_HPP
