#ifndef CLOAK_BOOK_HPP
#define CLOAK_BOOK_HPP

#include "defines.hpp"
#include "extern.hpp"
#include "chess_move.hpp"

struct BookEdge;
struct BookNode {
    PositionKey key;
    std::vector<BookEdge> edges;
    int games_played;
    int games_won;
};

struct BookEdge {
    ChessMove move;
    std::vector<unsigned> child_nodes; // indices
};

class Book {
    std::vector<BookNode> nodes_;

public:

};

#endif // CLOAK_BOOK_HPP
