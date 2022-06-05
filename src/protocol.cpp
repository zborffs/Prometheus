#include "protocol.hpp"

namespace internal {
    /**
     * parses the UCI "position" command and updates Board object
     * @param board  instance of Board object
     * @param position_command  the options of the command "fen ... moves ..."
     */
    void parse_pos(Board& board, const std::vector<std::string>& commands) {
        // determine the location of the "moves" command
        auto moves_itr = std::find(commands.begin(), commands.end(), "moves");

        // If user sends "position fen ... moves", then take all strings between fen and moves and process as if
        // it were a regular fen; if "fen" is replaced by "startpos" then just create starting board
        if(commands.size() > 5 && commands[1] == "fen") { // "5", because FENs have 5 ' ' delimited things
            board.set_board(commands.begin() + 2, moves_itr); // '2', because number of words ('position' and 'fen') before the FEN data
        } else if(commands.size() > 1 && commands[1] == "startpos") {
            board.set_board(); // no arguments means "startpos"
        }

        // If the user has sent any moves, then process them
        for (auto itr = moves_itr + 1; itr < commands.end(); ++itr) {
            // transform the user's move string into a ChessMove object
            auto move = parse_move(board, *itr);
            if(!move.has_value() || !is_move_legal(board, move.value())) {
                // If the string is malformed, or it's illegal, then crash the program (or just alert the user)
                assert(false); // crash program
                std::cout << "ERROR: In \"parse_pos(...)\" user provided illegal move or invalid command" << std::endl; // in release
            } else {
                // otherwise, make the move
                board.make_move(move.value());
            }
        }
    }

    /**
     * parses the UCI "go" command
     * @param board      instance of Board object
     * @param options    instance of UCIOptions object
     * @param go_command all the commands after the "go" command in the terminal
     */
    void parse_go(UCIOptions& options, const std::vector<std::string>& go_command) {
        // reset all the game-state options (things like how much time we have to search, not the hash table size)
        options.reset_game_state_vars();

        // If we received a 'ponder' command, set the ponder flag to true.
        // N.B. in ponder mode, we won't stop searching even if we find mate; this means that the last move sent to
        // "position" command is the move that we are pondering about. If the user then sends a ponderhit, then
        // the ponder move has been made. We can resume regular searching at this point
        auto itr = std::find(go_command.begin() + 1, go_command.end(), "ponder");
        options.ponder = itr != go_command.end();

        // If we received a 'wtime' command, then update the 'wtime' variable
        itr = std::find(go_command.begin() + 1, go_command.end(), "wtime");
        options.w_time_milli = itr != go_command.end() ? std::stoi(*(itr + 1)) : -1;

        // If we received a 'btime' command, then update the 'btime' variable
        itr = std::find(go_command.begin() + 1, go_command.end(), "btime");
        options.b_time_milli = itr != go_command.end() ? std::stoi(*(itr + 1)) : -1;

        // If we received a 'winc' command, then update the 'winc' variable
        itr = std::find(go_command.begin() + 1, go_command.end(), "winc");
        options.w_incr_milli = itr != go_command.end() ? std::stoi(*(itr + 1)) : -1;

        // If we received a 'binc' command, then update the 'binc' variable
        itr = std::find(go_command.begin() + 1, go_command.end(), "binc");
        options.b_incr_milli = itr != go_command.end() ? std::stoi(*(itr + 1)) : -1;

        // If we received a "movestogo" command, then update the corresponding variable
        itr = std::find(go_command.begin() + 1, go_command.end(), "movestogo");
        if (itr != go_command.end()) {
            options.moves_to_go = std::stoi(*(itr + 1));
        } else {
            // If we haven't received a "movestogo" command, but we have set wtime and btime variables, then this is a
            // sudden death game, so set the "sudden death" variable
            if (options.w_time_milli != -1 && options.b_time_milli != -1) {
                options.sudden_death = true;
            }
            options.moves_to_go = MAX_DEPTH;
        }

        // If we received a "depth" command, update corresponding variable
        itr = std::find(go_command.begin() + 1, go_command.end(), "depth");
        options.search_to_depth_x = itr != go_command.end() ? std::stoi(*(itr + 1)) : -1;

        // If we received a "nodes" command, update corresponding variable
        itr = std::find(go_command.begin() + 1, go_command.end(), "nodes");
        options.search_x_nodes = itr != go_command.end() ? std::stoi(*(itr + 1)) : -1;

        // If we received a "mate" command, update corresponding variable
        itr = std::find(go_command.begin() + 1, go_command.end(), "mate");
        options.search_for_mate_in_x = itr != go_command.end() ? std::stoi(*(itr + 1)) : -1;

        // If we received a "movetime" command, update corresponding variable
        itr = std::find(go_command.begin() + 1, go_command.end(), "movetime");
        options.search_for_time_x = itr != go_command.end() ? std::stoi(*(itr + 1)) : -1; // search exactly for 'x' milliseconds

        // If we received an "infinite" command, update corresponding variable
        itr = std::find(go_command.begin() + 1, go_command.end(), "infinite");
        options.infinite = itr != go_command.end(); // don't stop searching until stop command is received
    }

    /**
     * parses a single user move string from the "moves" part after the "position" command
     * @param board  instance of Board object
     * @param move  the move input move string (ex. e2e4)
     * @return  an optional of the ChessMove (if it's an illegal move or invalid command, then returns std::nullopt)
     */
    std::optional<ChessMove> parse_move(Board& board, const std::string& move) {
        // Create variables needed to instantiate ChessMove object
        Square_t from_sq, to_sq;
        MoveFlag flags = QUIET_MOVE;
        PieceType_t ptm, ptc;

        // All moves must have a length: 4 <= length <= 5; checking this let's us make this assumption later in function
        if (move.size() < 4 || move.size() > 5) {
            assert(false); // in debug, just crash the programs
            return std::nullopt; // all move commands need to have at least size 4 and at most size 5
        }


        // Transform input "from square" string into Square_t primitive
        File_t file = move[0] - 'a';
        Rank_t rank = move[1] - '1';
        from_sq = file + 8 * rank; // multiplication will be optimized into << 3
        if (from_sq < A1 || from_sq > H8) {
            assert(from_sq >= A1 && from_sq <= H8); // in debug-mode, crash
            return std::nullopt;
        }

        // Transform input "to square" string into Square_t primitive
        file = move[2] - 'a';
        rank = move[3] - '1';
        to_sq = file + 8 * rank;
        if (to_sq < A1 || to_sq > H8) {     // in release-mode, catch return a dummy ChessMove obj and log it.
            assert(to_sq >= A1 && to_sq <= H8); // in debug-mode, crash
            return std::nullopt;
        }

        // Use "from square" to figure out the piece that was moved
        ptm = board.piece_type(from_sq);
        if (ptm == PieceType::NO_PIECE) {   // in release-mode, catch return a dummy ChessMove obj and log it.
            assert(ptm != PieceType::NO_PIECE); // in debug-mode, crash
            return std::nullopt;
        }

        // Use "to square" to figure out the piece that was captured (if any was)
        ptc = board.piece_type(to_sq);
        if (ptc != PieceType::NO_PIECE && ptc % 2 == ptm % 2) {
            // make sure that if it was a capture, then we don't capture piece of same color
            assert(ptc == PieceType::NO_PIECE || ptc % 2 != ptm % 2);
            return std::nullopt;
        }

        // set the move flag
        if (ptc != PieceType::NO_PIECE) {
            // if there was a captured piece, then it's a captured move
            flags = CAPTURE_MOVE;
        } else if (((ptm == PieceType::W_PAWN && (to_sq - from_sq == 9 || to_sq - from_sq == 7)) || (ptm == PieceType::B_PAWN && (from_sq - to_sq == 9 || from_sq - to_sq == 7))) && to_sq == board.ep_sq()) {
            // If it was an en-passant move, then set the en passant flag and change captured piece type to be the opposite colored pawn as was moved
            if (ptm == PieceType::W_PAWN) {
                ptc = PieceType::B_PAWN;
            } else {
                ptc = PieceType::W_PAWN;
            }
            flags = ENPASSANT; // TODO: TEST FOR BUG...
        } else if ((ptm == PieceType::W_PAWN && to_sq - from_sq == 16 && from_sq / 8 == Rank::RANK2) || (ptm == PieceType::B_PAWN && from_sq - to_sq == 16 && from_sq / 8 == Rank::RANK7)) {
            // if the moved piece was a pawn and it moved 2 squares, then set the double-push-pawn flag
            flags = DOUBLE_PUSH_PAWN;
        } else if (ptm == PieceType::W_KING || ptm == PieceType::B_KING) {
            // If it was a castle, then set the castle flag
            if ((from_sq == E1 && to_sq == G1) || (from_sq == E8 && to_sq == G8)) {
                flags = KING_CASTLE;
            } else if ((from_sq == E1 && to_sq == C1) || (from_sq == E8 && to_sq == C8)) {
                flags = QUEEN_CASTLE;
            }
        }

        // if there was a fifth character, then that indicates a promotion, and we need to figure that out
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
                    default: assert(false);
                }
            } else {
                assert((ptm == W_PAWN && to_sq / 8 == Rank::RANK8 && from_sq / 8 == Rank::RANK7) || (ptm == B_PAWN && to_sq / 8 == Rank::RANK1 && from_sq / 8 == Rank::RANK2));
                return std::nullopt;
            }
        }

        return ChessMove(from_sq, to_sq, flags, ptm, ptc);
    }

    /**
     * sets the UCIOptions object based on user's command
     * @param options  instance of UCIOptions object
     * @param commands the commands vector needed to fulfill the request
     */
    void set_options(UCIOptions& options, const std::vector<std::string>& commands, SearchState& search_state) {
        if(commands[2] == "Hash") {
            int value = std::stoi(commands[4]);
            options.tt_size = value;
            search_state.tt = TranspositionTable(options.tt_size * 1000000 / sizeof(ChessHash));
        } else if (commands[2] == "Threads") {
            options.threads = std::stoi(commands[3]);
        } else {
            // do nothing if user attempts to set an option that doesn't exist
        }
    }
}

/**
 * processes uci commands
 * @param board  instance of the Board object
 * @param options  instance of the UCIOptions object
 * @param search_state  instance of SearchState object
 * @param eval_state  instance of EvaluationState object
 */
void start_uci_protocol(Board& board, UCIOptions& options, SearchState& search_state, EvaluationState& eval_state, Book& book) {
    // send boilerplate UCI stuff
    std::cout << "id name " << engine_name << " " << version << std::endl << "id author " << author << std::endl;
    options.send_options();
    std::cout << "uciok" << std::endl;

    // set loop variables
    bool quit = false;
    std::string input{};
    std::vector<std::string> commands{};

    // while the user hasn't sent "quit" command, process user input
    while (!quit) {
        getline(std::cin, input);
        if (input.empty() || input[0] == '\n') {
            // if the input string is empty or carriage return don't process anything
            continue;
        }
        commands = split(input); // 'split' the input string by ' ' delimiter
        std::cin.clear(); // clear the std::cin (not sure if necessary)

        if (commands[0] == "isready") {
            std::cout << "readyok" << std::endl;
        } else if (commands[0] == "position") {
            internal::parse_pos(board, commands); // update internal representation of the board
        } else if (commands[0] == "ucinewgame") {
            board.set_board(); // update the current representation of the board to be a new game
        } else if (commands[0] == "go") {
            internal::parse_go(options, commands); // update the game state options
            ChessMove best_move = think(board, options, search_state, eval_state, book); // start search
            std::cout << "bestmove " << best_move.to_string() << std::endl; // print the best move
        } else if (commands[0] == "uci") {
            std::cout << "id name " << engine_name << " " << version << std::endl << "id author " << author << std::endl;
            std::cout << "uciok" << std::endl;
        } else if (commands[0] == "quit") {
            quit = true;
        } else if (commands[0] == "setoption") {
            internal::set_options(options, commands, search_state);
        } else if (commands[0] == "ponderhit") {
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
                // undo the increment of the book for faster book searching
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