Tools Readme
============
Thoughts on tools stuff

Todos
-----
- tuning functionality in tools/ (something akin to texel tuning).
    - Potentially NNUE functionality in the future, but this seems a long way off, and is trained with the classical
      eval, so as long as the classical eval is shit, what's the point?
    - tuning data can be acquired easily with c-chess-cli tool
- tournament functionality (https://github.com/lucasart/c-chess-cli)
    - Julia script that wraps up the c-chess-cli script
    - Julia script will pass some gauntlet parameters to the c-chess-cli, which will return different sorts of data
      for tuning etc.
    - Julia parses tournament output to determine engine elo / skill and shit
- test suite analysis (general engine analysis)
    - test suites will give easy way of marking progress
    - julia scripts can monitor how changes affect things like ordering, time, success rates, etc.
    - scripts can be aggregated into library, which in turn

Test Suite Analysis
-------------------
### Questions
What kind of information do I want?
- I want the overall ordering for the each test suite as well as for every test suite
    - table + graph for each test suite
    - single table + graph combined test suites
- I want the ordering broken down by game stage and by position type
    - covariance between game stage and ordering and game stage and position type
    - graph of overall ordering of all test suites for different game stages and different position types
- I want to know the relationship between total nodes and leaf nodes, and total nodes and time
- I want to know the effect of game stage and position type on time and on number of nodes
- I want to know time on success
- I want to know game stage and position type on success
- I want to know ordering on success
### Goals
- from the look of things, I should be able to consolidate the move-ordering
  scripts into a library files and then some scripts.
- the test suite scripts will be performing ordering analysis, so in a sense
  I can kill two birds with one stone and do ordering analysis at the same time
  I do test suite analysis
- But i'll save the move-ordering scripts for now, because they work and are
  good references

Tournaments - Regression + Tuning Data Generation
-------------------------------------------------
- Julia script wraps c-chess-cli and dispatches tournament requests
- c-chess-cli returns with file for tuning and file with outcome (potentially with fens)
- Julia script reads the outcome file and performs statistical analysis (ranking etc.) on the engines supplemented with
  a priori data about the elos of each engine (if that's important)
- Julia prints beautiful graph + table of results for user to inspect and share

Tuning
------
- tune_runner.cpp performs texel (or something) tuning

Opening Book
------------
### Client Interface

- actual book data is deserialized from some file somewhere
- client asks the book for all the moves from a given position
- client gets the moves, applies some function for selecting which of the moves to do, and tells the engine which of
  the BookEdges he selected
- book looks at index of BookEdge in current BookNodes array corresponding to the move that the user has said he
  sekected, then looks at the index associated with that move (which is stored in the BookEdge), and pushes that
  BookNode onto a stack
- the player undoes the book move, the stack gets popped
  - the stack must be initialized with the starting position
- if the player starts from a position when the initial position is not the 'startpos', then it must do a search
  - until I think of a better strategy that'll just be a brute force search

### Book Maker

- make the first node the board.key() from the starting position and set both the games played and games won to zero
  and the book edges to null
- parse the protobook
  - convert the algebraic notation to a ChessMove
  - add the ChessMove object to the current node's list, setting the index to 0, i.e. the startpos
  - if the vector of parsed algebraic notations is over, then go to the next line, otherwise make the move and repeat

### Book Learning Approaches


1. Back Propogation
  1. play a game, storing the book moves made in the stack
  2. once the game is over, white has either one (1.0), lost (-1.0), or tied (0)
  3. We pop the last book move made from the stack, and update its value (running mean etc.) with the new outcome
  4. continually pop the stack, updating each value by a similar mechanism until the root is reached

2. Alpha Beta
  1. play a game, storing the book moves made in the stack
  2. once the game is over, white has either one (1.0), lost (-1.0), or tied (0)
  3. We pop the last book move made from the stack, and update its value (running mean etc.) with the new outcome
  4. We perform an alpha-beta search on the tree, and negamax the values of the leafs up to the root, now modified
     by the new leaf node value

3.)