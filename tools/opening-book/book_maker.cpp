#include "globals.hpp"
#include "board.hpp"
#include "book.hpp"
#include "move_gen.hpp"
#include <vector>
#include <optional>
#include "string_manip.hpp"
#include "chess_clock.hpp"

std::optional<ChessMove> algnot_to_chessmove(Board& board, std::string algnot) {
    std::vector<ChessMove> movelist;
    movelist.reserve(128);
    gen_all_moves(board, movelist);

    for (int i = 0; i < movelist.size(); ++i) {
        if (algnot == movelist[i].to_algebraic_notation()) {
            return movelist[i];
        } else if (algnot[algnot.size() - 1] == '+') {
            std::string substring = std::string(algnot.begin(), algnot.end() - 1);
            if (substring == movelist[i].to_algebraic_notation()) {
                return movelist[i];
            }
        }
    }

    return std::nullopt;
}

void print_protobook(std::vector<BookNode>& pbook) {
    for (int i = 0; i < pbook.size(); ++i) {
        std::cout << pbook[i].key << std::endl;
        for (int j = 0; j < pbook[i].edges.size(); ++j) {
            std::cout << "(" << pbook[i].edges[j].move.to_string() << ", " << pbook[i].edges[j].child_node << ") | ";
        }
        std::cout << std::endl;
    }
}

class ProtoBookParser {
public:
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
                throw std::runtime_error("Runtime error in read_all_fen_from_file(const std::string&, const uint32): Badbit file.");
            }

            in.close();
        } catch (const std::exception& e) {
            throw;
        }

        return lines;
    }
};

#define LOG_FAILURE -1

bool init_logger(const std::string& path) noexcept;

int main(int argc, char** argv) {
    /// Initialize the logger variables, if it fails to initialize, then quit.
    if (!init_logger(std::string(argv[0]))) {
        return LOG_FAILURE;
    }

    /// Create local variables at the top
    std::string exec_path;
    std::string protobook_path;

    /// Get path to the input file, "BasicTests.fen", and output file, "tools/data/ordering_output.txt", from executable directory and command line arguments
    exec_path = std::string(argv[0]);
    bool first_is_slash = exec_path[0] == '/';
    auto splitvec = split(exec_path, '/');

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
    std::vector<std::string> lines = ProtoBookParser::read(protobook_path, 1, 367);

    Board board;
    ChessClock clock;
    board.set_board();

    std::vector<BookNode> proto_book;
    BookNode root;
    int current = 0;
    root.key = board.key();
    root.games_played = 0;
    root.games_won = 0;
    root.edges = {};
    proto_book.push_back(root);

    clock.start();
    for (auto & line : lines) {
        auto opening = split(line, ':');
        std::cout << opening[0] << std::endl;
        std::vector<std::string> moves = split(opening[1]);
        for (int i = 0; i < moves.size(); i++) {
            std::optional<ChessMove> move = algnot_to_chessmove(board, moves[i]);
            if (move.has_value()) {
                std::cout << move->to_string() << ", ";

                if (proto_book[current].key != board.key()) {
                    BookNode node;
                    node.key = board.key();
                    node.games_played = 0;
                    node.games_won = 0;
                    node.edges = {};
                    proto_book.push_back(node);
                    proto_book[current].edges[proto_book[current].edges.size() - 1].child_node = proto_book.size() - 1;
                    current = proto_book[current].edges[proto_book[current].edges.size() - 1].child_node;
                }

                BookEdge edge;
                edge.move = move.value();
                board.make_move(move.value());
                edge.child_node = 0;

                // find the position in the proto book whose key equals that of the board
                auto itr = std::find_if (proto_book.begin(), proto_book.end(), [&](const BookNode& node){
                    return node.key == board.key();
                });

                if (itr != proto_book.end()) {
                    // we found it in here already
                    std::size_t index = std::distance(proto_book.begin(), itr);
                    edge.child_node = index;
                    auto move_itr = std::find_if(proto_book[current].edges.begin(), proto_book[current].edges.end(), [&](const BookEdge& e) {
                        return edge.move == e.move;
                    });
                    if (move_itr == proto_book[current].edges.end()) {
                        proto_book[current].edges.push_back(edge);
                    }

                    current = index;
                } else {
                    proto_book[current].edges.push_back(edge);
                }
            } else {
                spdlog::get(logger_name)->warn("breaking. Something was mistyped in the proto book on line: {0}", line);
                break;
            }
        }
        board.set_board();
        current = 0;
        std::cout << std::endl << std::endl;
    }
    clock.stop();

    print_protobook(proto_book);
    double d = clock.duration();
    std::cout << "Book Complete! It took " << d / 1e9 << " seconds" << std::endl;

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
#ifdef WINDOWS
        std::string base(path.substr(0, arg.find_last_of("\\")));
#else
        std::string base(path.substr(0, path.find_last_of("/")));
#endif // WINDOWS
        std::string path_to_log(base + "/../../logs/Cloak-Book.log");

        auto logger = spdlog::daily_logger_st(logger_name, path_to_log, 4, 59);
        logger->set_level(spdlog::level::debug);

    } catch (const spdlog::spdlog_ex& ex) {
        std::cout << "Log initialization failed: " << ex.what() << std::endl;
        return false;
    }

    return true;
}