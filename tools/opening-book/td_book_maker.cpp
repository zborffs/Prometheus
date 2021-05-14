#include "globals.hpp"
#include "board.hpp"
#include "book.hpp"
#include "move_gen.hpp"
#include <vector>
#include <optional>
#include "string_manip.hpp"
#include "chess_clock.hpp"
#include "cereal/archives/binary.hpp"
#include <fstream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

/**
 * generate a ChessMove given a string in algebraic notation
 * @param board  the current state of the board
 * @param algnot the algebraic notation string
 * @return       a ChessMove object associated with the algebraic notation string or nullopt if no match was found
 */
std::optional<ChessMove> algnot_to_chessmove(Board& board, std::string algnot) {
    std::vector<ChessMove> movelist;
    movelist.reserve(128);
    gen_all_moves(board, movelist); // generate all the chess moves from the given locations

    for (int i = 0; i < movelist.size(); ++i) {
        if (algnot == movelist[i].to_algebraic_notation()) {
            // the the chess move's algebraic notation string matches the input string, then return that chessmove
            return movelist[i];
        } else if (algnot[algnot.size() - 1] == '+') {
            // the ChessMove algebraic notation interface doesn't put '+' to denote checks, so it is possible that
            // the input string corresponds with this ChessMove, but the nominal string check won't check for that.
            // so in that case remove the '+' from the input move and check for correspondence again
            std::string substring = std::string(algnot.begin(), algnot.end() - 1);
            if (substring == movelist[i].to_algebraic_notation()) {
                return movelist[i];
            }
        }
    }

    return std::nullopt;
}

/**
 * prints the vector of BookNodes to the terminal for visual verification of structure
 * @param pbook vector of BookNodes that makes up the core of Book object
 */
void print_protobook(std::vector<BookNode>& pbook) {
    for (int i = 0; i < pbook.size(); ++i) {
        std::cout << pbook[i].key << std::endl;
        for (int j = 0; j < pbook[i].edges.size(); ++j) {
            std::cout << "(" << pbook[i].edges[j].move.to_string() << ", " << pbook[i].edges[j].child_node << ") | ";
        }
        std::cout << std::endl;
    }
}

/**
 * class for readings the lines of the given ProtoBookParser
 */
class ProtoBookParser {
public:
    /**
     * reads all the lines of a protobook between some arbitrary line numbers
     * @param filename   the path and filename of the protobook text file
     * @param start_line the line to start reading from
     * @param end_line   the line to read up to (exclusive)
     * @return           a vector of strings representing the lines
     */
    static std::vector<std::string> read(const std::string& filename, int start_line, int end_line) {
        std::vector<std::string> lines;
        std::string line;

        try {
            std::fstream in(filename);
            in.exceptions(std::ifstream::badbit);
            if (!in.is_open()) {
                std::string err("Failed to open file: ");
                err += filename;
                throw std::runtime_error(err);
            }

            for (int i = start_line; std::getline(in, line) && i < end_line; ++i) {
                lines.push_back(line);
            }

            if (in.bad()) {
                throw std::runtime_error("Runtime error in ProtoBookParser::read(const std::string& filename, int start_line, int end_line): Badbit file.");
            }

            in.close();
        } catch (const std::exception& e) {
            spdlog::get(logger_name)->error("Failed to parse the file: {}", e.what());
            throw;
        }

        return lines;
    }
};

#define LOG_FAILURE -1

bool init_logger(const std::string& path) noexcept;

int main(int argc, char** argv) {
    std::string path(argv[0]);
#ifdef WINDOWS
    std::string base(path.substr(0, path.find_last_of("\\")));
#else
    std::string base(path.substr(0, path.find_last_of('/')));
#endif // WINDOWS

    /// Initialize the logger variables, if it fails to initialize, then quit.
    std::string logfile_path(base + "/../../logs/TDBookMaker.log");
    if (!init_logger(logfile_path)) {
        return LOG_FAILURE;
    }
    std::string protobook_path; // path from project home directory to protobook file
    std::string output_path; // path to final opening book binary

    /// determine the path to the executable and the project home directory
    path = std::string(argv[0]);
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
    protobook_path = std::string(s + "/../../" + argv[1]);
    output_path = std::string(s + "/../../" + argv[2]);

    /// read the protobook's lines and pass them into lines member variable
    std::vector<std::string> lines = ProtoBookParser::read(protobook_path, 1, 400);

    Board board; // used for keys and updating state
    ChessClock clock; // used to time duration of creating book
    board.set_board(); // initialize the board to 'startpos'
    std::vector<BookNode> proto_book; // the protobook constituting core of Book object (to be serialized)
    BookNode root; // the root node corresponding to the startpos
    int current = 0; // keeps track of the current index of BookNode
    root.key = board.key();
    root.games_lost = 0;
    root.games_drawn = 0;
    root.games_won = 0;
    root.edges = {};
    proto_book.push_back(root);

    clock.start();
    for (auto & line : lines) {
        auto opening = split(line, ':'); // break the line into two parts separated by ':'
        spdlog::get(logger_name)->info(opening[0]);
        std::vector<std::string> moves = split(opening[1]); // further split the moves into substrings by ' '
        for (int i = 0; i < moves.size(); i++) {
            // find the ChessMove associated with the string in 'moves' vector at position i
            std::optional<ChessMove> move = algnot_to_chessmove(board, moves[i]);

            // if the move is legal, then go on processing it, otherwise break from this loop and go to the next opening
            if (move.has_value()) {
                // print the moves string representation to the screen
                spdlog::get(logger_name)->info(move->to_string());

                if (proto_book[current].key != board.key()) {
                    // if the current BookNode's index doesn't correspond to the board's key, then we must add the
                    // BookNode to the protobook and update the current BookNode's last edge (i.e. the Move that was
                    // just added to the current book node)'s index so that it points to the newly created BookNode
                    BookNode node;
                    node.key = board.key();
                    node.games_lost = 0;
                    node.games_drawn = 0;
                    node.games_won = 0;
                    node.edges = {};
                    proto_book.push_back(node);
                    proto_book[current].edges[proto_book[current].edges.size() - 1].child_node = proto_book.size() - 1;
                    current = proto_book[current].edges[proto_book[current].edges.size() - 1].child_node;
                }

                // create a book node with nominal value of no child node with current move
                BookEdge edge;
                edge.move = move.value();
                board.make_move(move.value());
                edge.child_node = 0;
                edge.q_value = 0;

                // find the position in the proto book whose key equals that of the board
                auto itr = std::find_if (proto_book.begin(), proto_book.end(), [&](const BookNode& node){
                    return node.key == board.key();
                });

                if (itr != proto_book.end()) {
                    // in this case, current position is already in the book, so check if the move is in the book
                    std::size_t index = std::distance(proto_book.begin(), itr);
                    edge.child_node = index;

                    // try to find the new move in the current BookNode's edges
                    auto move_itr = std::find_if(proto_book[current].edges.begin(), proto_book[current].edges.end(), [&](const BookEdge& e) {
                        return edge.move == e.move;
                    });

                    // if the new move isn't in the current BookNode's edges, then add it to the edges
                    if (move_itr == proto_book[current].edges.end()) {
                        proto_book[current].edges.push_back(edge);
                    }

                    // update the current index to correspond with the current board state
                    current = index;
                } else {
                    // if the node is already in the book, then we must have gotten here from a new move, so just push
                    // the move to the BookNode's edge
                    proto_book[current].edges.push_back(edge);
                }
            } else {
                spdlog::get(logger_name)->warn("breaking. Something was mistyped in the proto book on line: {0}", line);
                break;
            }
        }

        // reset the board to the 'startpos' and the index to the 'startpos' index
        board.set_board();
        current = 0;
    }
    clock.stop();

//    print_protobook(proto_book);
    double d = clock.duration();
    double acc = d;
    spdlog::get(logger_name)->info("Finished parsing proto book! It took {} seconds", d / 1e9);

    std::stringstream ss;
//    std::ofstream output_book_fstream("tools/data/OpeningBook.cereal", std::ios::binary);
    clock.start();
    {
        cereal::BinaryOutputArchive oarchive(ss); // Create an output archive
        Book book(proto_book);
        oarchive(book);
    } // archive goes out of scope, ensuring all contents are flushed
    clock.stop();
    d = clock.duration();
    acc += d;
    spdlog::get(logger_name)->info("Finished testing the serialization of the opening book! It took {} seconds", d / 1e9);

    std::vector<BookNode> proto_book_ar;
    clock.start();
    {
        cereal::BinaryInputArchive iarchive(ss); // Create an input archive
        Book book;
        iarchive(book); // Read the data from the archive
        proto_book_ar = book.protobook();
    }
    clock.stop();
    d = clock.duration();
    acc += d;
    spdlog::get(logger_name)->info("Finished testing the deserialization of the opening book! It took {} seconds", d / 1e9);


//    print_protobook(proto_book_ar);
    assert(proto_book_ar.size() == proto_book.size());
//    for (int i = 0; i < proto_book_ar.size(); ++i) {
//        std::cout << proto_book_ar[i] << " == " << proto_book[i] << std::endl;
//    }

    clock.start();
    std::ofstream output_book_fstream(output_path, std::ios::binary);
    {
        cereal::BinaryOutputArchive oarchive(output_book_fstream);
        Book book(proto_book);
        oarchive(book);
    }
    clock.stop();
    acc += clock.duration();
    spdlog::get(logger_name)->info("Created Opening Book! It took {} seconds", acc / 1e9);

    return 0;
}

/**
 * initializes the logger
 * @param logfile_path path to the logfile
 * @return             whether the function successfully setup the logger
 */
bool init_logger(const std::string& logfile_path) noexcept {
    try {
        /// Setup the console sink
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::warn);
        console_sink->set_pattern("[%D %H:%M:%S] [%^%l%$] [Thread %t] [File:Line %@] [Function: %!] %v");
        console_sink->set_color_mode(spdlog::color_mode::always);

        /// setup the file sink
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logfile_path, true);
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