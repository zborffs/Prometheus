# Description of Tests
This file has descriptions of all the tests.

## Bratko-Kopec
The Bratko-Kopec Test (BK-Test) was designed by Ivan Bratko and Danny Kopec in 1982 at the University of Edinburgh. Despite having been created in the early 80s, the BK-Test remains current, being widely discussed, updated, and revisited on internet forums. This is the first line of defense to evaluate an engine's ability; failing anyone of these test positions is not acceptable. The engine gets 30 seconds per position.

## CCR One Hour Test
CCR One Hour Test by Larry Kaufman was published 1994 in Computer Chess Reports, Vol.4, No.2. Similar to the BK-Test suite, this test suite evaluates general ability. Therefore, any failure will not be tolerated. The engine receives 10 seconds per position.

## Kaufman Test
In 1993, as editor of ICD's Computer Chess Reports, Larry Kaufman proposed a 25 problem test suite for chess playing programs. Another test suite whose primary objective is to evaluate the strength of an engine's evaluation. The engine allocates 1 second per position.

# Win at Chess
Win at Chess (WAC), a collection of 300 chess positions first compiled and published by Fred Reinfeld in 1958. The tactical postions were taken from actual tournament play, contain scores of traps, sacrifices, mates, winning combinations, and subtle exchanges that will help sharpen players' eyes and test their skills against the masters, and were (or even are) a quite popular test set for chess engines as a sanity check. Run through a simple tactical test like WAC at 5 seconds/move every time you change something important in your search. Don't try to optimize the results, but just make sure that the score doesn't suddenly drop dramatically.

## Eigenmann Endgame Test
Eigenmann Endgame Test, (Eigenmann Endspiel Test) is an endgame test suite compiled by Walter Eigenmann. It consists of 100 positions and 60 seconds are allocated to the engine to search them. How well an engine performs is proportional to the number of tests it successfully solves. The central conceit of this test is to test the speed at which an engine can evaluate while retaining the 100% accuracy. The best scoring engine made use of EGTBs and incredibly fast hardware only to achieve a pretty mediocre score, so scoring poorly is not the end of the world.  

## NullMove Test-Positions
NullMove Test-Positions is a test suite where null moves may fail due to zugzwangs.

## Silent But Deadly
This is an EPD test created by Dann Corbit originally designed to help tune chess engines because of the property that no position should illicit a capture, but rather a positional change. Evaluation should be below a pawn for a one minute search. There are a handful of positions where the evaluation is above a pawn and a few positions where there are multiple solutions.

## Studies
This test consists of a combination of famous positions published in various magazines as well as positions administered in famous chess studies. They are ordered thus, Behting study, HAKMEM 70, Lasker-Reichhelm Position, Réti Endgame Study, and Szén Position. I don't know the correct answers yet. This should just be a little fun thing. The engine will get 1 minute per position.

## Strategic Test Suite (STS)
This is a series of themed tests originally curated by Dann Corbit and Swaminathan Natarajan and latterly converted by Ferdinand Mosca to be used by an analysis tool. The themes are as follows:

1.) Undermining - Capturing a piece that's guarding another piece, whose also being attacked. A quick quiescence search or witty combination should easily detect these.

2.) Open Files and Diagonals -

3.) Knight Outposts -

4.) Square Vacancy -

5.) Bishop vs. Knight -

6.) Re-Capturing -

7.) Offer of Simplification -

8.) Advancement of f/g/h Pawns -

9.) Advancement of a/b/c Pawns -

10.) Simplification -

11.) Activity of the King -

12.) Center Control -

13.) Pawn Play in the Center -

14.) Queens and Rooks to the 7th Rank -

15.) Avoid Pointless Exchange -

## positions.perft and positions2.perft
Perft positions that have tripped me up in the past. 