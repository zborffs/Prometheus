#ifndef PROMETHEUS_BOOK_HPP
#define PROMETHEUS_BOOK_HPP

#include "defines.hpp"
#include "extern.hpp"
#include "chess_move.hpp"
#include <cereal/types/vector.hpp>
#include <iostream>
#include <utility>


struct BookEdge {
    ChessMove move;
    std::size_t child_node; // index of the next BookNode
    double q_value;

    template<class Archive>
    void serialize(Archive& archive) {
        archive(move, child_node, q_value); // serialize things by passing them to the archive
    }

    friend std::ostream& operator<<(std::ostream& os, const BookEdge& edge) {
        return os << "[" << edge.move.to_string() << ", " << edge.child_node << ", " << edge.q_value << "]";
    }
};

struct BookNode {
    PositionKey key{};
    std::vector<BookEdge> edges;
    int games_lost{}; // as white
    int games_won{}; // as white
    int games_drawn{}; // as white (doesn't matter)
    // double score

    bool operator==(const BookNode& node) const {
        return node.key == key;
    }

    template<class Archive>
    void serialize(Archive& archive) {
        archive(key, edges, games_won, games_lost, games_drawn); // serialize things by passing them to the archive
    }

    friend std::ostream& operator<<(std::ostream& os, const BookNode& node) {
        os << "[" << node.key << " (w: " << node.games_won << ", l: " << node.games_lost << ", d: " << node.games_drawn  << ")]:";
        for (auto edge : node.edges) {
            os << edge << " ";
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
    Book() = default;
    Book(std::vector<BookNode> nodes) : nodes_(std::move(nodes)) {}

    inline std::vector<BookNode> protobook() {
        return nodes_;
    }
    [[nodiscard]] inline BookNode current_node() const {
        return former_nodes_.top();
    }
    [[nodiscard]] inline unsigned num_nodes() const {
        return nodes_.size();
    }
    std::vector<BookEdge> edges(PositionKey key);
    void make_move(int index);
    void unmake_move();
    [[nodiscard]] inline PositionKey top() const {
        return former_nodes_.top().key;
    }
    void update_q_value(unsigned index, double new_q) {
        former_nodes_.top().edges[index].q_value = new_q;
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

#endif // PROMETHEUS_BOOK_HPP
