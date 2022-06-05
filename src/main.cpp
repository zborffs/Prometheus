/// Global Variable include -- must be included first
#include "globals.hpp"

/// STL Includes
#include <thread>
#include <iostream>
#include <memory>

/// Third Party Includes
#include <cereal/archives/binary.hpp>

/// Project Includes
#include "eval.hpp"
#include "protocol.hpp"
#include "search.hpp"
#include "book.hpp"

void create_book(const std::string& filename, Book& book);

int main(const int argc, char* argv[]) {
    std::string path(argv[0]); // the "prometheus" executable expects a path to book be given to it from command line
#ifdef WINDOWS
    std::string base(path.substr(0, arg.find_last_of("\\")));
#else
    std::string base(path.substr(0, path.find_last_of('/')));
#endif // WINDOWS

    // Initialize the logger variables, if it fails to initialize, then quit.
    std::string bookpath(base + "/../data/PrometheusOpening.book");

    // Instantiate all state objects
    Board board; // struct storing board state (position of pieces etc.)
    UCIOptions options; // stores UCI state (movetime, engine options, etc.)
    SearchState search_state(32 * 1000000 / sizeof(ChessHash)); // stores search state (number of nodes, etc.)
    EvaluationState eval_state; // stores eval state (type of position, etc.)
    Book book; // opening book data structure
    create_book(bookpath, book);

    // Initialize loop variables
    bool quit = false;
    std::string input{};

    while (!quit) {
        getline(std::cin, input); // uses std::cin to populate the string "input"
        if (input == "uci") {
            start_uci_protocol(board, options, search_state, eval_state, book);
            quit = true;
        } else if (input == "quit") {
            quit = true;
        }
    }

    return 0;
}

/**
 * deserializes a serialized instance of the "Book" object from memory using the "cereal" library
 * TODO: investigate other serialization libraries, since this one (I believe) leads to executable bloat
 * @param filename  a string representing the relative path and file name of the serialized book object
 * @param book      reference of the book object we want to populate with the serialized version
 */
void create_book(const std::string& filename, Book& book) {
    // TODO: some error checking? what if there is no opening book binary?
    std::ifstream f(filename, std::ios::binary);
    cereal::BinaryInputArchive iarchive(f);
    iarchive(book);
}