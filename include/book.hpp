#ifndef CLOAK_BOOK_HPP
#define CLOAK_BOOK_HPP

#include "defines.hpp"
#include "extern.hpp"
#include "chess_move.hpp"
#include <cereal/types/vector.hpp>
#include <iostream>


struct BookEdge {
    ChessMove move;
    std::size_t child_node; // index of the next BookNode

    template<class Archive>
    void serialize(Archive& archive) {
        archive(move, child_node); // serialize things by passing them to the archive
    }

    friend std::ostream& operator<<(std::ostream& os, const BookEdge& edge) {
        return os << "[" << edge.move.to_string() << ", " << edge.child_node << "]";
    }
};

struct BookNode {
    PositionKey key;
    std::vector<BookEdge> edges;
    int games_played;
    int games_won;

    bool operator==(const BookNode& node) const {
        return node.key == key;
    }

    template<class Archive>
    void serialize(Archive & archive) {
        archive(key, edges, games_played, games_won); // serialize things by passing them to the archive
    }

    friend std::ostream& operator<<(std::ostream& os, const BookNode& node) {
        os << "[" << node.key << "]:";
        for (unsigned i = 0; i < node.edges.size(); ++i) {
            os << node.edges[i] << " ";
        }
        return os;
    }
};

class Book {
    std::vector<BookNode> nodes_;
    std::stack<BookNode> former_nodes_;

    friend class cereal::access;

    template <class Archive>
    void serialize(Archive& ar) {
        ar(CEREAL_NVP(nodes_));
    }

public:
    Book() {}
    Book(const std::vector<BookNode>& nodes) : nodes_(nodes) {}

    inline std::vector<BookNode> protobook() {
        return nodes_;
    }
    std::vector<BookEdge> edges(PositionKey key);
    void make_move(int index);
    void unmake_move();
    inline PositionKey top() const {
        return former_nodes_.top().key;
    }

    friend std::ostream& operator<<(std::ostream& os, Book& book) {
        std::vector<BookNode> stack(book.former_nodes_.size());
        for (std::size_t i = 0; i < book.former_nodes_.size(); ++i) {
            os << book.former_nodes_.top().key << " -> ";
            stack[i] = book.former_nodes_.top();
            book.former_nodes_.pop();
        }
        for (auto & node : stack) {
            book.former_nodes_.push(node);
        }
        return os;
    }
};

#endif // CLOAK_BOOK_HPP
