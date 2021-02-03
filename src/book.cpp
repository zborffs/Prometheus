#include "book.hpp"


std::vector<BookEdge> Book::edges(PositionKey key) {
    if (!former_nodes_.empty()) {
        if (former_nodes_.top().key == key) {
            return former_nodes_.top().edges;
        }
    }

    for (int i = 0; i < nodes_.size(); ++i) {
        if (nodes_[i].key == key) {
            former_nodes_.push(nodes_[i]);
            return nodes_[i].edges;
        }
    }

    return {};
}

void Book::make_move(int index) {
    auto node_index = former_nodes_.top().edges[index].child_node;
    former_nodes_.push(nodes_[node_index]);
}

void Book::unmake_move() {
    former_nodes_.pop();
}