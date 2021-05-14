/// Project Includes
#include "defines.hpp"
#include "globals.hpp"
#include "board.hpp"
#include "chess_clock.hpp"
#include "search.hpp"
#include "book.hpp"

/// Standard Library Includes
#include <iostream>
#include <string>

/// Third-Party Includes
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <cereal/archives/binary.hpp>
#include <spdlog/sinks/basic_file_sink.h>

#define LOG_FAILURE -1 // flesh out all the quit flags later

bool init_logger(const std::string& path) noexcept;
void create_book(const std::string& filename, Book& book);
void learn_opening_book(Book& book);
double r_value(Book& book, PositionKey key);

int main([[maybe_unused]] int argc, char **argv) {
    std::string path(argv[0]);
#ifdef WINDOWS
    std::string base(path.substr(0, path.find_last_of("\\")));
#else
    std::string base(path.substr(0, path.find_last_of('/')));
#endif // WINDOWS

    /// Initialize the logger variables, if it fails to initialize, then quit.
    std::string logfile_path(base + "/../../logs/TestSuiteBenchmark.log");
    if (!init_logger(logfile_path)) {
        return LOG_FAILURE;
    }

    /// Create local variables at the top
    std::string basic_tester_path;
    std::string output_file_path;
    std::string book_path;
    Board board;
    ChessClock chess_clock;
    SearchState search_state(16384);
    EvaluationState eval_state;
    UCIOptions options;
    Book book;

    /// Get path to the input file, "BasicTests.fen", and output file, "tools/data/ordering_output.txt", from executable directory and command line arguments
    bool first_is_slash = path[0] == '/';
    auto splitvec = split(path, '/');

    std::string s{""};
    assert(!splitvec.empty());

    if (first_is_slash) {
        s += "/" + splitvec[0];
    } else {
        s += splitvec[0];
    }

    for (int i = 1; i < splitvec.size() - 1; i++) {
        s += std::string("/" + splitvec[i]);
    }
    book_path = std::string(s + "/../../data/CloakOpening_tdlambda.book");
//    create_book(book_path, book);



    return 0;
}

/**
 * initializes the logger; assumes that the folder containing the executable is in the same directory as the folder
 * storing the log text files.
 * @param path the path to the executable of this program
 * @return     boolean representing the success of the function
 */
bool init_logger(const std::string& path) noexcept {
    try {
        /// Setup the console sink
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::trace);
        console_sink->set_pattern("[%D %H:%M:%S] [%^%l%$] [Thread %t] [File:Line %@] [Function: %!] %v");
        console_sink->set_color_mode(spdlog::color_mode::always);

        /// setup the file sink
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path, true);
        file_sink->set_level(spdlog::level::trace);

        /// setup the logger using both sinks
        spdlog::logger logger(logger_name, {console_sink, file_sink});
        logger.set_level(spdlog::level::debug);
        spdlog::set_default_logger(std::make_shared<spdlog::logger>(logger_name, spdlog::sinks_init_list({console_sink, file_sink})));
    } catch (const spdlog::spdlog_ex& ex) {
        std::cout << "Logger initialization failed: " << ex.what() << std::endl;
        return false; // if we fail to initialize the logger, return false
    }

    return true;
}

/**
 *
 * @param filename
 * @param book
 */
void create_book(const std::string& filename, Book& book) {
    std::ifstream f(filename, std::ios::binary);
    cereal::BinaryInputArchive iarchive(f);
    iarchive(book);
}

void learn_opening_book(Book& book, double gamma = 0.7, unsigned num_epochs = 1000) {
    // randomly start in tree num_epochs time, an epoch ends if you reach a leaf of the tree
    // - uniform random distribution index between [0-nodes.size()-1] inclusive
    // N.B. q values already initialized in Book
    unsigned seed = std::chrono::steady_clock::now().time_since_epoch().count();
    std::default_random_engine gen(seed);
    std::uniform_int_distribution<std::size_t> node_distribution(0, book.num_nodes() - 1);
    std::vector<BookNode> nodes = book.protobook();

    for (unsigned i = 0; i < num_epochs; ++i) {
        unsigned random_start_node_index = node_distribution(gen);
        BookNode start_node = nodes[random_start_node_index];
        std::vector<BookEdge> edges = book.edges(start_node.key);

        // starting from the random node, randomly select a move
        // - uniform random distribution index between [0-node.edges.size()-1] inclusive
        unsigned move_counter{0};
        while (!edges.empty()) { // if this edges were empty, then we would have randomly started in a goal state
            std::uniform_int_distribution<std::size_t> move_distribution(0, edges.size() - 1);
            auto random_move_index = move_distribution(gen);

            // next; make the move; this updates the book so that the current position is that which was achieved after having
            // made the randomly chosen move.
            book.make_move(random_move_index);
            ++move_counter;

            // get the q values for all the edges in that node.
            std::vector<BookEdge> next_moves = book.edges(book.top());
            double min_q{1000000000.0}; // take the MINIMUM (not max as is defined by literature) of the q_values
            for (unsigned j = 0; j < next_moves.size(); ++j) {
                if (min_q > next_moves[i].q_value) {
                    min_q = next_moves[i].q_value;
                }
            }

            // multiply that minimum score by gamma
            // add product to the r_value of the current POSITION
            // q_value for the random move we just made becomes this sum
            book.update_q_value(random_move_index, r_value(book, book.top()) + min_q * gamma);

            // if the !edges.empty(), then repeat this inner loop, randomly selecting a move, etc. this will repeat until the
            // edges vector is empty
            edges = next_moves;
        }

        // leave the inner loop and unmake all moves to restore original state
        while (move_counter > 0) {
            --move_counter;
            book.unmake_move();
        }

        // 1 epoch is complete so go back to the top of the loop, where we start from another random node
        // repeat this num_epochs number of times, then we're done!
    }
}

double r_value(Book& book, PositionKey key) { // potentially change PositionKey key to BookNode& node
    std::vector<BookEdge> edges = book.edges(key);

    if (edges.empty()) {
        return NAN;
    }

    // edges not empty
    Color_t side2move = edges[0].move.moved % 2 == 0 ? Color::WHITE : Color::BLACK; // get the side that's moving
    BookNode current_node = book.current_node(); // get the current node (access Reward function parameters)
    unsigned w = current_node.games_won;
    unsigned l = current_node.games_lost;
    unsigned d = current_node.games_drawn;

    double r{0.0};

    if (side2move == Color::WHITE) {
        // TODO: add gain term ((w+l+d)) to incentivize games which have been played more?
        r = (w + 0.5 * d) / (w + l + d) - 0.5;
    } else {
        r = (l + 0.5 * d) / (w + l + d) - 0.5;
    }

    return r;
}

double q_value(Book& book, BookEdge& edge) {

}