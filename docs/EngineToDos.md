Cloak Todos
===========

Notes
-----
------------------------------------------------------------------------------------------------------------------------
- I'm just now returning to the engine after a bit of time. It's hard for me to discern what's what and what is going
on
- I'm certain multiple different parts of the engine are not operating optimally or are even buggy
    - This means I need to get my hands dirty and either tune sub-optimal sections or debug buggy sections
- There are also a many features I want to implement
    - These probably shouldn't be implemented until I know the engine's on even footing
        - i.e. no known or suspected bugs. optimally running other parts
- There must be a clear and testable way of identifying and testing bugs / optimality improvements / features 
    


List of Suspected Buggy Sections
--------------------------------
------------------------------------------------------------------------------------------------------------------------
- Why are we not storing from square and to square to lower bounds in search(...) and not the best position?
- Why are we returning 0 on time constraint triggers in search(...) and not alpha?


- BUG: when Bishop value is > 325 or around there for 4th position in kaufman, the program crashes only in release mode.
  - the value is not overflowing, this won't crash the program
  - the issue is a seg fault
  - always happens on the 5th depth (after 4 iterative deepens)
  - I tried increasing Centipawns_t to int64_t, but this doesn't work
  - hypothesis: increasing the value of the bishop happens to take the engine down a line which causes a bug for a reason unrelated to the value of the bishop itself
  - because it's segfault, i'm thinking it's either transposition table or it's ChessHash in search or maybe ChessMove hash_move in hash?
  - the fact that this doesn't occur in debug is interesting...

List of Suspected Sub-Optimal Sections
--------------------------------------
------------------------------------------------------------------------------------------------------------------------
- Move ordering:
    - Optimal = minimizes total nodes, maximizes % move ordering
      - ordering.cpp / Ordering.jl / OrderingCompare.jl
    - Are the weights for each move ordering feature optimal?
    - Is the manner in which killer moves are added to the KillerMove array optimal
    - Is the manner in which history heuristics are added to the HistoryHeuristic array optimal
- Transposition Table:
    - Optimal = maximizes ELO, maximizes % move ordering, minimizes total nodes, minimizes cache misses, 
      minimizes hash misses (type I and II)
    - Use more advanced hashing, storing, searching schemes discussed in various talks
- Evaluation
    - Optimal = maximizes ELO, minimizes time to compute
    - Are the parameter values optimal?

List of Desired Features
------------------------
------------------------------------------------------------------------------------------------------------------------
- Search Stuff
   * [ ] aspiration window (https://www.chessprogramming.org/Aspiration_Windows)
   * [ ] PVS 
   * [ ] Parallel (PVS)
- Selectivity Stuff
  - Extensions
    * [x] Check Extensions
    * [ ] One Reply Extensions
    * [ ] Passed Pawn Extensions
    * [x] Mate Threat Extensions
    * [ ] SEX Algorithm
  - Pruning
    * [ ] AEL Pruning
      * [X] Adaptive Null Move Pruning
      * [ ] Extended Futility Pruning
      * [ ] Limited Razoring
    * [x] Delta Pruning
    * [ ] Futility Pruning
    * [ ] Multi-Cut
  - Reductions
    * [ ] Fail-High Reductions
    * [ ] Late Move Reductions
    * [ ] Razoring
- Move Ordering
  - Goal >0.9 every move
  * [x] MVV-LVA
  * [X] SEE
  * [x] Killer Heuristic
  * [x] History Heuristic
  * [ ] Countermove Heuristic
  * [ ] Guard Heuristic

Evaluation Ideas
----------------
- do some "preprocessing" at the beginning of each evaluation function (or simply compute the pawns first)
  - in this preprocessing / pawn function, we update a bitboard in the search_state structure that contains white & black
  pawns at the base of pawn chains as well as pawns in king's fortress
  - in mobility section of every piece but king, take intersection with own color's pieces and get defending bonnus
    - get an extra defending bonus for pawns at base
  - in mobility section of every piece but king, take intersection with enemy pieces  and get attacking bonus
    - get an extra bonus attacking enemy pawn base
    - get extra bonus for attacking king fortress