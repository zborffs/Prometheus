Prometheus
==========
This is a chess engine written in C++.

Installation
------------
To download and build:
```shell
git clone https://gitlab.com/zborffs/Prometheus
mkdir -p Prometheus/build
pushd Prometheus/build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j
```

To run in terminal:
```shell
./prometheus
```

To build with SSE commands enabled, instead run the following "cmake":
```shell
cmake -DCMAKE_BUILD_TYPE=Release -DUSE_SSE=On
```

I have only tested this on Unix machines, so I don't guarantee that it will run on Windows.

Bugs
----
1. ~~Promotion illegal move bug? Perhaps just everything is upper case?~~
2. ~~Weird unreproducible assert(from_sq != to_sq moves)... not sure what's going on here. maybe related to TT~~
3. ~~three decimal place bug for "info ... ordering ..."~~
4. counting raw nodes and counting fail highs

Search Features List
--------------------
List of all the search, selectivity, and move-ordering features I have / want to implement.

- Search Stuff
    * [ ] aspiration window
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
    * [x] Goal >0.9 every move (DID IT!)
    * [x] MVV-LVA
    * [X] SEE // redo
    * [x] Killer Heuristic // optimize
    * [x] History Heuristic // optimize
    * [ ] Guard Heuristic

Evaluation Function
-------------------
List of all the evaluation function features I want to implement.

Pawns:
- Should consider:
  - [ ] controlling center squares
  - [ ] defending pawns that control center squares
  - [ ] file and rank the pawn is on
  - [ ] being or not a passer, potential passer or unopposed passer
  - [ ] being or not a backward pawn, possibly even part of the king shelter
  - [ ] its status as a double or isolated pawn; and is it isolated horizontally or vertically
  - [ ] pertaining to or not a group of pawns
  - [ ] status as a lead or root pawn; lead pawn is a weak spot, possibly part of the enemy king shelter
  - [ ] controlling weak spots
  - [ ] being or not part of the king shelter; specific square of the shelter
  - [ ] fixing enemy pawns (forcing them to not move anymore)
  - [ ] possibility to attack the enemy king position
- Eval
  - [x] Piece-square tables (positioning)
  - [x] base-value * bonus/penalty game_stage
  - [ ] pass pawn bonus
  - [ ] unopposed passer bonus
  - [ ] supported passer (by rook)
  - [ ] backward pawn penalty
  - [ ] isolated pawn penalty
  - [ ] double pawn penalty
  - [ ] pawn island tactical bonus
  - [ ] Connectivity 
  - [ ] bonus for pawn in other team's half of the board
  - [ ] mobility bonus/penalty (check if pinned, penalizes being halted by enemy pawn) (small bonus, small penalty if halted by enemy, small penalty if halted by friend, medium penalty if halted by friend on third rank in opening, big penalty if pinned)

- Rooks:
  - [X] positioning (PST)
  - [ ] bonus for protecting passed pawn
  - [ ] bonus for being on open file
  - [ ] bonus for connecting
  - [ ] bonus for being on 7th rank
  - [ ] bonus for 8th rank
  - [ ] bonus on semi-open files (1 pawn not 2)
  - [ ] connectivity
  - [ ] mobility (account for pins)

- Bishops:
  - [x] positioning (PST)
  - [ ] connectivity
  - [ ] mobility (account for pins)
  - [ ] ~~bishop pair bonus~~

- Knights:
  - [x] positioning (PST)
  - [ ] connectivity
  - [ ] mobility (account for pins)

- King:
  - eval:
    - [ ] Penalty for losing castling rights (if not castled already)
    - [ ] Penalty for insufficient pawns in pawn shelter
    - [ ] connectivity bonus for pawn shelter
    - [ ] connectivitiy bonus for pawn shelter root
    - [ ] penality for placement of pawns in shelter (f3 = -50, h3 = -25, g3 = 0)
    - [ ] doubled pawns in pawn shelter, or open rank in pawn shelter
    - [ ] pawn storm penalty (+40 for enemy pawn on fourth rank, +60 for fifth rank and +80 for sixth rank)
    - [ ] (only in end game) king mobility

- Opening:
  - Eval:
    - [x] Penalty for developing queen before 3 minor pieces
    - [ ] Bonus for Castling
    - [ ] Penalty Moving the same piece twice in the opening

Connectivity
------------
- Similar to guard heuristic. Basically, for each square on the board, we compute a number. The number is the weighted
sum of all the pieces attacking (or defending) that square. If a pawn attacks (or defends) a square, then the weight is 
high, but if a queen attacks (or defends) a square, the weight is small. The weights are more or less the inverse of the
base value of the piece. this encourages protecting or attacking with pawns over other minor pieces and especially 
queens. Scoring of these minor and major piece types can be compensated (if compensation even makes sense) by noticing 
that queens have a major mobility bonus etc. etc. Now we have a static 8x8 or 64x1 vector corresponding to how well 
defended / attacked each square on the board is. We take the dot product of this board, by the dot product of another 
board denoting the importance of different regions of the board. In effect, this incorporates another weighting to the
origin matrix. The "importance" board is computed by assigning high weights to regions that are deemed weak or 
otherwise important for positional reasons. Examples might include assigning high weights the center 4 squares, or 
assigning high weights to the king shelter (3x3 or 4x4 region around where the king is if castled). This would assign 
high scores to positions that have many pieces attacking or defending places like the middle of the board, the enemies 
king shelter or our own king shelter (or countless other "weak" or "important" areas of te board). It would assign low 
values to positions in which pieces are not supporting each other at all or positions in which the pieces might be 
supporting each other, but the pieces being supported are in irrelevant positions on the board. In some sense, the piece
weight corresponds to the position-independent but piece-dependent, and the position weight is piece-independent and 
position-dependent. It could might theoretically encourage capturing free space. If a square in the middle of the board 
is currently controlled by the enemy, and moving a pawn forward to free up space behind it would reduce enemy's control
of the square, and the squares currently controlled by the pawn are sufficiently defended so that the marginal of moving
the pawn to the new position is positive, then it's a good move to take the space. We have to create the importance 
matrix dynamically. This will identify roots of a pawn chain (which are weak spots)


Book Learning
-------------
I have implemented (though haven't rigorously tested the performance of) Q-Learning in the opening book. Learning the 
opening book is referred to as "book learning".
  - Test that after a certain number of wins/losses that the wins/losses values are updated
  - test that if, after a bunch of wins/losses, the qvalue is updated according to algorithm correctly
  - test that after the qvalue is learned well, that the algorithm plays the better qlearned positions
  - compute performance increase

Tuning / Training
-----------------
- C++ responsible for doing texel tuning with Adam optimizer.

Regression / Tournaments
------------------------
- C++ executable wraps c-chess-cli and dispatches tournament requests
- c-chess-cli returns a file for tuning and file with outcome (potentially with fens)
- C++ executable performs statistical analysis (summary statistics / hypothesis testing / graph+table generation) on outcome
  - most importantly LOS tables and ELO

Test Suites
-----------
- test suites are used to quickly test the performance of the engine after making a small change to see if we have 
broken anything. 
  - perform hypothesis testing to determine whether the change results in statistically significant 
  improvement/deterioration