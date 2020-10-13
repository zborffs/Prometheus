//
// Created by Zach Bortoff on 2020-07-03.
//

#ifndef CHESSBOTEMBEDDED_BOOK_HPP
#define CHESSBOTEMBEDDED_BOOK_HPP

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
    std::vector<BookNode> nodes;
};

class Book {
    std::unordered_map<BookNode, std::vector<BookEdge> > book_; // potentially optimize this later
    BookNode current_node_;
    void insert(const std::vector<ChessMove>& moves);

public:
    /// Initialization
    void create_book(const std::string& file_name);

    /// Getters
    BookNode lookup(Bitboard mykey);
    std::vector<ChessMove> edges(const BookNode& node);
    std::vector<ChessMove> edges(); // finds all the edges of the "current_node_"

    /// Setters
    void update_current_node(const ChessMove& move);
};

#endif //CHESSBOTEMBEDDED_BOOK_HPP
