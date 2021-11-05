Prometheus
==========

This is a chess engine written in C++.

How to Use
----------
To download and build:
```shell
git clone https://gitlab.com/zborffs/Prometheus
mkdir -p Prometheus/build
pushd Prometheus/build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j
```
At this point, you ought to have the Cloak-lib.a static library and the Cloak executable built for your host.

To run in terminal:
```shell
./prometheus
```
if the program immediately terminates, something went wrong. Try adding a logs/ directory in the project home directory. 
Otherwise, you'll be prompted with an empty terminal. Type "uci" for UCI mode. 


Engine Features List
--------------------
- Search Stuff
    * [ ] aspiration window (https://www.chessprogramming.org/Aspiration_Windows)
    * [ ] Parallel PVS
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
    * [X] SEE // redo
    * [x] Killer Heuristic // optimize
    * [x] History Heuristic // optimize
    * [ ] Countermove Heuristic


Book Learning
-------------
- Q-Learning on previous performance
  - Test that after a certain number of wins/losses that the wins/losses values are updated
  - test that if, after a bunch of wins/losses, the qvalue is updated according to algorithm correctly
  - test that after the qvalue is learned well, that the algorithm plays the better qlearned positions
  - compute performance increase

Tuning / Training
-----------------
- C++ responsible for doing texel tuning with Adam optimizer

Regression / Tournaments
------------------------
- C++ executable wraps c-chess-cli and dispatches torunament requests
- c-chess-cli returns a file for tuning and file with outcome (potentially with fens)
- C++ executable performs statistical analysis (summary statistics / hypothesis testing / graph+table generation) on outcome
  - most importantly LOS tables and ELO

Test Suites
-----------
- test suites are used to quickly test the performance of the engine after making a small change to see if we have broken anything
  - perform hypothesis testing to determine whether the change results in statistically significant improvements/detractions

Procedure for Development
-------------------------
1. Fix bugs / REMOVE functionality until something works consistently without weird bugs (but maybe just slow)
2. Capture data related to performance in 4 test suites (1 end game, 2 generic, 1 quiet)
3. Perform regression test (get baseline performance)
4. Make a (small) change to the engine
5. validate that the small change makes a statistically noticeable improvement in performance of at least 1 or 2 test suites
6. once we can prove that there is an improvement, perform regression test
7. repeat steps 4,5,6 until all features we want to develop are done

Bugs
----
1. When playing against engine, I say to search for 5000 milliseconsd, it instantly returns an answer without going for 5
seconds, when I keep executing the same command, the pv got shorter, then reduced to two moves but depth 5. the depth didn't change
in the PV after executing the same command over and over.
   1. Could have something to do with how we handle quits (checking quit conditions / how they are reset)?
   2. Could also have something to do with always-replace
   3. Could also have somthing to do with when we handle quits, we do something weird to the transposition table
   4. Table is becoming overfull, and the original PV is being overwritten or can't write in the first place
      1. add a condition to remove keys exact keys
2. Pathological Zobrist hashing on one of the Kaufman test positions causes there to be several zobrist hash collisions 
for that search (probably the same one)
   - this increases in frequency as the thing gets more full? maybe because there are more opportunities for clashing (not always a bad thing)
3. Game crashes with "Assertion failed: (from_sq != to_sq), function ChessMove, file chess_move.cpp, line 26." if asked to search 'go movetime 5000' on a mate! -> but only sometimes?

Problem Areas: Tranposition Table is losing PVs -- DONE
Quiescence search takes WAYY too long, maybe doesn't even work as expected, because basic losing exchanges are fucked up and there is a major horizon effect going on