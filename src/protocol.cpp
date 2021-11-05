#include "protocol.hpp"

namespace internal {
    /**
     * identifies engine to stdout as specified by UCI protocol
     */
    void identify_engine() noexcept {
        std::cout << "id name " << engine_name << " " << version << std::endl << "id author " << author << std::endl;
    }

    /**
     * sends "uciok" to stdout as specified by UCI protocol
     */
    void send_uciok() noexcept {
        std::cout << "uciok" << std::endl;
    }

    /**
     * sends "readyok" as specified by UCI protocol
     */
    void send_readyok() noexcept {
        std::cout << "readyok" << std::endl;
    }

    /**
     * registers engine as specified by UCI protocol
     * @param register_commands the extra commands needed to fulfill request
     */
    void register_engine(const std::vector<std::string>& register_commands) {
        const std::string name{"Zach Bortoff"};
        const std::string code{"5772156649015"}; // truncated Euler–Mascheroni constant

        /// In debug mode, assert that all incoming command conforms to protocol (i.e. this command will have 1 arg);
        /// if in release, log the error, but don't do anything about it (i.e. ignore it).
        assert(register_commands.size() == 2);
        if (register_commands.size() != 2) {
            SPDLOG_LOGGER_ERROR(spdlog::get(logger_name), "void register_engine(...) - Incorrect number of arguments. Got {}, wanted {}", register_commands.size(), 2);
            return;
        }

        if (register_commands[1] == "name") {
            std::cout << "register name " << name << std::endl;
        } else if(register_commands[1] == "code") {
            std::cout << "register code " << code << std::endl;;
        } else if(register_commands[1] == "later") {
            std::cout << "register name " << name << "code " << code << std::endl;
        } else {
            SPDLOG_LOGGER_ERROR(spdlog::get(logger_name), "void register_engine(...) - Invalid Argument(s). \"{}\"",  register_commands[1]);
        }
    }

    /**
     * parses the UCI "position" command and updates Board object
     * @param board             instance of Board object to be updated
     * @param position_command  the specifics of the command
     */
    void parse_pos(Board& board, const std::vector<std::string>& commands) {
        /// determine the location of the "moves" command
        auto moves_itr = std::find(commands.begin(), commands.end(), "moves");

        /// If user sends "position fen ... moves", then take all strings between fen and moves and process as if
        /// it were a regular fen; if "fen" is replaced by "startpos" then just create starting board
        if(commands.size() > 5 && commands[1] == "fen") {
            board.set_board(commands.begin() + 2, moves_itr); // '2' refers to the number of words ('position' and 'fen') before the fend
        } else if(commands.size() > 1 && commands[1] == "startpos") {
            board.set_board(); // no arguments means "startpos"
        }

        /// If the user has sent any moves, then process it/them.
        for(auto itr = moves_itr + 1; itr < commands.end(); ++itr) {
            auto move = parse_move(board, *itr);

            /// If the command is invalid or the move is illegal, then don't process it
            if(!move.has_value() || !is_move_legal(board, move.value())) {
#ifndef NDEBUG
                SPDLOG_LOGGER_ERROR(spdlog::get(logger_name), "User provided illegal move or invalid command: {}", *itr);
#endif // NDEBUG

                /// Otherwise, make the move
            } else {
                board.make_move(move.value());
            }


        }
    }

    /**
     * parses the UCI "go" command and updates the Board and UCIOptions objects;
     * N.B. many of the args of the "go" command are mutually exclusive, so we don't need to think about cases where
     * multiple UCIOptions game-state flags are set in one "go" command
     * @param board      instance of Board object to be updated
     * @param options    instance of UCIOptions object to be updated
     * @param go_command the rest of the "go" commands to fulfill request
     */
    void parse_go(UCIOptions& options, const std::vector<std::string>& go_command) {
        /// reset all the game-state options in the UCIOptions object at the beginning of a go command
        options.reset_game_state_vars();

        /// If we have received a "searchmoves" command, constrain search to those moves
        auto itr = std::find(go_command.begin() + 1, go_command.end(), "searchmoves"); // plus 1 b/c first element is "go"
        if (itr != go_command.end()) {
            // we received a 'searchmoves' command.
            // there should be moves after this move
            // only search after the line specified by that continuation
        }

        /// If we received a 'ponder' command, set the ponder flag to true.
        /// N.B. in ponder mode, we won't stop searching even if we find mate; this means that the last move sent to
        /// "position" command is the move that we are pondering about. If the user then sends a ponderhit, then
        /// the ponder move has been made. We can resume regular searching at this point
        itr = std::find(go_command.begin() + 1, go_command.end(), "ponder");
        options.ponder = itr != go_command.end();

        /// If we received a 'wtime' command, then update the 'wtime' variable
        itr = std::find(go_command.begin() + 1, go_command.end(), "wtime");
        options.w_time_milli = itr != go_command.end() ? std::stoi(*(itr + 1)) : -1;

        /// If we received a 'btime' command, then update the 'btime' variable
        itr = std::find(go_command.begin() + 1, go_command.end(), "btime");
        options.b_time_milli = itr != go_command.end() ? std::stoi(*(itr + 1)) : -1;

        /// If we received a 'winc' command, then update the 'winc' variable
        itr = std::find(go_command.begin() + 1, go_command.end(), "winc");
        options.w_incr_milli = itr != go_command.end() ? std::stoi(*(itr + 1)) : -1;

        /// If we received a 'binc' command, then update the 'binc' variable
        itr = std::find(go_command.begin() + 1, go_command.end(), "binc");
        options.b_incr_milli = itr != go_command.end() ? std::stoi(*(itr + 1)) : -1;

        /// If we received a "movestogo" command, then update the corresponding variable
        itr = std::find(go_command.begin() + 1, go_command.end(), "movestogo");
        if (itr != go_command.end()) {
            options.moves_to_go = std::stoi(*(itr + 1));

            /// If we haven't received a "movestogo" command, but we have set wtime and btime variables, then this is a
            /// sudden death game, so set the "sudden death" variable
        } else {
            if (options.w_time_milli != -1 && options.b_time_milli != -1) {
                options.sudden_death = true;
            }
            options.moves_to_go = MAX_DEPTH;
        }

        /// If we received a "depth" command, update corresponding variable
        itr = std::find(go_command.begin() + 1, go_command.end(), "depth");
        options.search_to_depth_x = itr != go_command.end() ? std::stoi(*(itr + 1)) : -1;

        /// If we received a "nodes" command, update corresponding variable
        itr = std::find(go_command.begin() + 1, go_command.end(), "nodes");
        options.search_x_nodes = itr != go_command.end() ? std::stoi(*(itr + 1)) : -1;

        /// If we received a "mate" command, update corresponding variable
        itr = std::find(go_command.begin() + 1, go_command.end(), "mate");
        options.search_for_mate_in_x = itr != go_command.end() ? std::stoi(*(itr + 1)) : -1;

        /// If we received a "movetime" command, update corresponding variable
        itr = std::find(go_command.begin() + 1, go_command.end(), "movetime");
        options.move_time = itr != go_command.end() ? std::stoi(*(itr + 1)) : -1; // search exactly for 'x' seconds

        /// If we received a "infinite" command, update corresponding variable
        itr = std::find(go_command.begin() + 1, go_command.end(), "infinite");
        options.infinite = itr != go_command.end(); // don't stop searching until stop command is received
    }

    /**
     * parses the "moves" part after the "position" command
     * @param board instance of Board object
     * @param move  the move input move string (ex. e2e4)
     * @return      an optional of the ChessMove (if it's an illegal move or invalid command, then returns std::nullopt)
     */
    std::optional<ChessMove> parse_move(Board& board, const std::string& move) {
        /// Create variables needed to instantiate ChessMove object
        Square_t from_sq, to_sq;
        MoveFlag flags = QUIET_MOVE;
        PieceType_t ptm, ptc;

        /// All moves must have a length: 3 < length < 6
        if (move.size() < 4 || move.size() > 5) {
            SPDLOG_LOGGER_ERROR(spdlog::get(logger_name), "std::optional<ChessMove> parse_move(...) - Argument error move.size() invalid: {}", move.size());
            return std::nullopt;
        } else {
            /// Transform input "from square" string into Square_t primitive
            File_t file = move[0] - 'a';
            Rank_t rank = move[1] - '1';
            from_sq = file + 8 * rank; // multiplication will be optimized into << 3
            assert(from_sq >= A1 && from_sq <= H8); // in debug-mode, assert to crash program
            if (from_sq < A1 || from_sq > H8) {     // in release-mode, catch return a dummy ChessMove obj and log it.
                SPDLOG_LOGGER_ERROR(spdlog::get(logger_name), "std::optional<ChessMove> parse_move(...) - Argument error from_sq out of bounds: {}", from_sq);
                return std::nullopt;
            }

            /// Transform input "to square" string into Square_t primitive
            file = move[2] - 'a';
            rank = move[3] - '1';
            to_sq = file + 8 * rank;

            assert(to_sq >= A1 && to_sq <= H8); // in debug-mode, assert to crash program
            if (to_sq < A1 || to_sq > H8) {     // in release-mode, catch return a dummy ChessMove obj and log it.
                SPDLOG_LOGGER_ERROR(spdlog::get(logger_name), "std::optional<ChessMove> parse_move(...) - Argument error to_sq out of bounds: {}", from_sq);
                return std::nullopt;
            }

            /// Use "from squre" Square_t to figure out the piece that was moved
            ptm = board.piece_type(from_sq);

            assert(ptm != PieceType::NO_PIECE); // in debug-mode, assert to crash program
            if (ptm == PieceType::NO_PIECE) {   // in release-mode, catch return a dummy ChessMove obj and log it.
                SPDLOG_LOGGER_ERROR(spdlog::get(logger_name), "std::optional<ChessMove> parse_move(...) - Argument error piece @ from_sq is NO_PIECE.");
                return std::nullopt;
            }

            /// User "to square" Square_t to figure out the piece that was captured
            ptc = board.piece_type(to_sq);
            assert(ptc == PieceType::NO_PIECE || ptc % 2 != ptm % 2); // make sure there are no captures, or the piece colors aren't the
            if (ptc != PieceType::NO_PIECE && ptc % 2 == ptm % 2) { /// make sure captured piece is not the same color as moved piece
                SPDLOG_LOGGER_ERROR(spdlog::get(logger_name), "std::optional<ChessMove> parse_move(...) - Argument error ptc is same color as ptm.");
                return std::nullopt;
            }

            /// Set the move flags: if there was a captured piece, then it's a captured move
            if (ptc != PieceType::NO_PIECE) {
                flags = CAPTURE_MOVE;

                /// If it was an en-passant move, then set the en passant flag
            } else if (((ptm == PieceType::W_PAWN && (to_sq - from_sq == 9 || to_sq - from_sq == 7)) || (ptm == PieceType::B_PAWN && (from_sq - to_sq == 9 || from_sq - to_sq == 7))) && to_sq == board.ep_sq()) {
                flags = ENPASSANT;

                /// if the moved piece was a pawn and it moved 2 squares, then set the double-push-pawn flag
            } else if ((ptm == PieceType::W_PAWN && to_sq - from_sq == 16 && from_sq / 8 == Rank::RANK2) || (ptm == PieceType::B_PAWN && from_sq - to_sq == 16 && from_sq / 8 == Rank::RANK7)) {
                flags = DOUBLE_PUSH_PAWN;

                /// If it was a castle, then set the castle flag
            } else if (ptm == PieceType::W_KING || ptm == PieceType::B_KING) {
                if ((from_sq == E1 && to_sq == G1) || (from_sq == E8 && to_sq == G8)) {
                    flags = KING_CASTLE;
                } else if ((from_sq == E1 && to_sq == C1) || (from_sq == E8 && to_sq == C8)) {
                    flags = QUEEN_CASTLE;
                }
            }

            /// If there was a fifth character, then that indicates a promotion, and we need to figure that out
            if (move.size() == 5) {
                if ((ptm == W_PAWN && to_sq / 8 == Rank::RANK8) || (ptm == B_PAWN && to_sq / 8 == Rank::RANK1)) {
                    switch (move[4]) {
                        case 'N':
                        case 'n': flags = static_cast<MoveFlag>(KNIGHT_PROMO + flags); break;
                        case 'B':
                        case 'b': flags = static_cast<MoveFlag>(BISHOP_PROMO + flags); break;
                        case 'R':
                        case 'r': flags = static_cast<MoveFlag>(ROOK_PROMO + flags); break;
                        case 'Q':
                        case 'q': flags = static_cast<MoveFlag>(QUEEN_PROMO + flags); break;
                    }
                } else {
                    SPDLOG_LOGGER_ERROR(spdlog::get(logger_name), "std::optional<ChessMove> parse_move(...) - Argument error size == 5, but from/to_sq invalid for promotion.");
                    return std::nullopt;
                }
            }
        }

        return ChessMove(from_sq, to_sq, flags, ptm, ptc);
    }

    /**
     * sets the UCI options
     * @param options  instance of UCIOptions object to be updated
     * @param commands the commands vector needed to fulfill the request
     */
    void set_options(UCIOptions& options, const std::vector<std::string>& commands) {
        if(commands[2] == "Hash") {
            int value = std::stoi(commands[4]);
            options.tt_size = value;
        } else if(commands[2] == "OwnBook") {
            if(commands[4] == "true") {
                options.own_book  = true;
            } else if(commands[4] == "false") {
                options.own_book = false;
            }
        } else if(commands[2] == "UCI_ShowCurrLine") {
            if(commands[4] == "true") {
                options.show_current_line = true;
            } else if(commands[4] == "false") {
                options.show_current_line = false;
            }
        } else if(commands[2] == "UCI_LimitStrength") {
            if(commands[4] == "true") {
                options.limit_strength = true;
            } else if(commands[4] == "false") {
                options.limit_strength = false;
            }
        } else if(commands[2] == "Threads") {
            options.threads = std::stoi(commands[3]);
        } else if(commands[2] == "UCI_Elo") {
            int elo = std::stoi(commands[4]);
            options.elo = elo;
        } else if(commands[2] == "Style") {
            if(commands[4] == "Balanced") {
                options.style = Style::BALANCED;
            } else if(commands[4] == "Aggressive") {
                options.style = Style::AGGRESSIVE;
            } else if(commands[4] == "Defensive") {
                options.style = Style::DEFENSIVE;
            }
        } else {

        }
    }
}

/**
 * enters the uci protocol loop, which processes user input and conforms to UCI protocol
 * @param board        instance of the Board class: contains board specific data and functions
 * @param options      instance of the UCIOptions struct: holds UCI protocol specific data
 * @param search_state instance of SearchState struct: contains search specific data
 * @param eval_state   instance of EvaluationState struct: contains evaluation specific data
 */
void start_uci_protocol(Board& board, UCIOptions& options, SearchState& search_state, EvaluationState& eval_state, Book& book) {
    /// Send boilerplate UCI stuff
    internal::identify_engine();
    options.send_options();
    internal::send_uciok();

    /// Set loop variables
    bool quit = false;
    std::string input{};
    std::vector<std::string> commands{};

    /// While the user hasn't sent "quit" command, process user input
    while (!quit) {
        /// Get user input, if it's empty, then do nothing, otherwise split it by ' ' and reset std::cin
        getline(std::cin, input);
        if (input.empty() || input[0] == '\n') {
            continue;
        }
        commands = split(input);
        std::cin.clear();

        if (commands[0] == "isready") {
            // If we received "isready", execute send_readyok()
            internal::send_readyok();
        } else if (commands[0] == "position") {
            // if we received "position", start the parse_pos function
            internal::parse_pos(board, commands);
        } else if (commands[0] == "ucinewgame") {
            // If we received "ucinewgame" then reset the board
            board.set_board();
        } else if (commands[0] == "go") {
            // If we received "go" command, then parse the go command, then start search
            internal::parse_go(options, commands);
            ChessMove best_move = think(board, options, search_state, eval_state, book);
            std::cout << "bestmove " << best_move.to_string() << std::endl;
        } else if (commands[0] == "uci") {
            // If we received "uci" command send back boilerplate "uci" stuff
            internal::identify_engine();
            internal::send_uciok();
        } else if (commands[0] == "quit") {
            // If we received "quit" command, then quit
            quit = true;
        } else if (commands[0] == "setoption") {
            // If we received setoption command, then set corresponding option
            internal::set_options(options, commands);
        } else if (commands[0] == "register") {
            // If we received "register" command, then register the engine
            internal::register_engine(commands);
        } else if (commands[0] == "ponderhit") {
            // If we received "ponderhit" command, unset the ponder flag
            // the enemy has played what the user expected, so continue searching but leave ponder mode.
            options.ponder = false;
        } else if (commands[0] == "board") {
            // debug command (NOT UCI COMMAND): print the board object
            std::cout << board << std::endl;
        } else if (commands[0] == "ucioptions") {
            // debug command (NOT UCI COMMAND): print the UCIOptions object
            std::cout << options << std::endl;
        } else if (commands[0] == "undo") {
            // debug command (NOT UCI COMMAND): undo the last move
            board.unmake_move();
            if (board.key() == book.top()) {
                book.unmake_move();
            }
        } else if (commands[0] == "evalstate") {
            // debug command (NOT UCI COMMAND): print the EvaluationState object
            std::cout << eval_state << std::endl;
        } else if (commands[0] == "eval") {
            // debug command (NOT UCI COMMAND): print the evaluation of current position
            std::cout << "Evaluation: " << evaluate(board, eval_state, true) << std::endl;
        } else if (commands[0] == "searchstate") {
            // debug command (NOT UCI COMMAND): print the search state
            std::cout << search_state << std::endl;
        } else if (commands[0] == "book") {
            // debug command (NOT UCI COMMAND): print the book's stack
            std::cout << book << std::endl;
        } else {
            // do nothing
        }
    }
}